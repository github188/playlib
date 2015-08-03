// Generated by Neo

#include <sstream>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <stddef.h>
#include <string.h>

#include "utils/threads.h"
#include "utils/commons.h"

#include <nplayer/nplayer.h>
#include <nplayer/handler.h>

#ifdef _USE_OPENAL_
#include <alu/openal_utils.h>
#else
#include <alu/audio_track.h>
#endif

#include <json.h>
#include <JADEC.h>

#define MAX_FRAME_COUNT 1000

using namespace Json;

bool g_is_stat_mode;
bool hls_over_callback;

void rcb(ReportWhat what, ReportArg1 arg1, ReportArg2 arg2, const void* data) {
	LOGV("report: %d, %d, %d, %p", what, arg1, arg2, data);
}

/**
 * 统计线程
 *
 */
void* onStat(void* _stat) {
//	LOGW("%s [%p]: E", LOCATE_PT);

	float delayed = 0.0f;
	long long last_delay = currentMillisSec();

	int adjust_index[MAX_WINDOW_COUNT] = { 0 };
	bool adjust_checker[MAX_WINDOW_COUNT] = { false };

	int calc[MAX_WINDOW_COUNT][ADJUST_SIZE] = { { 0 } };
	int left[MAX_WINDOW_COUNT][ADJUST_SIZE] = { { 0 } };

	Value values, item;
	FastWriter writer;

	while (g_is_stat_mode) {
		values.clear();
		delayed = (int) (currentMillisSec() - last_delay) / 1000.0f;

		if (delayed > 0.1f) {
			for (int i = 0; i < MAX_WINDOW_COUNT; i++) {
				int window = array2Window(i);
				player_suit* player = g_player[i];

				if (NULL == player || window < 0) {
					continue;
				}

				stat_suit* stat = player->stat;
				player_core* core = player->core;
				video_meta* meta = player->vm_normal;

				pthread_mutex_unlock(&(stat->mutex));
				if (player->is_playback_mode) {
					meta = player->vm_playback;
				}

				if (meta->video_width > 0 && meta->video_height > 0
						&& (stat->video_network_count > 0
								|| stat->audio_network_count > 0)) {
					float jump_fps = stat->video_jump_count / delayed;
					float network_fps = stat->video_network_count / delayed;
					float audio_network_fps = stat->audio_network_count
							/ delayed;

					float decorder_fps = stat->video_decoder_count / delayed;
					float decorder_delay = 0.0f;
					float audio_decorder_fps = stat->audio_decoder_count
							/ delayed;
					float audio_decorder_delay = 0.0f;

					size_t audio_left =
#ifdef _USE_OPENAL_
							(NULL != player->alu) ? player->alu->left() : 0;
#else
							(NULL != player->track) ? player->track->left() : 0;
#endif
					size_t queue_left = core->video_queue_handle->size();

					float render_fps = stat->video_decoder_count / delayed;
					float render_delay = 0.0f;
					float audio_play_fps = (stat->audio_decoder_count
							- audio_left) / delayed;
					float audio_play_delay = 0.0f;

					audio_left += core->audio_queue_handle->size();

					if (decorder_fps) {
						decorder_delay = stat->video_decoder_delayed
								/ decorder_fps;
					}

					if (render_fps > 0) {
						render_delay = stat->video_render_delayed / render_fps;
					}

					if (audio_decorder_fps > 0) {
						audio_decorder_delay = stat->audio_decoder_delayed
								/ audio_decorder_fps;
					}

					if (audio_play_fps > 0) {
						audio_play_delay = stat->audio_play_delayed
								/ audio_play_fps;
					}

					float video_kbps = 0.0f;
					float audio_kbps = 0.0f;

					if (delayed > 0.1f) {
						video_kbps = stat->video_network_bytes / 1024 / delayed;
						audio_kbps = stat->audio_network_bytes / 1024 / delayed;
					}

#ifdef SHOW_STAT
					LOGS(
							"stat[%d], delay: %.1fs, kbps: %.2f/%.2f", window, delayed, video_kbps, audio_kbps);
#endif

					item["window"] = window;
					item["delay"] = delayed;
					item["kbps"] = video_kbps;
					item["audio_kbps"] = audio_kbps;

#ifdef SHOW_STAT
					LOGS(
							"stat[%d], fps: %.1f/%.1f+%.1f, %.1f/%.1f", window, network_fps, decorder_fps, jump_fps, audio_network_fps, audio_decorder_fps);
#endif

					item["network_fps"] = network_fps;
					item["decoder_fps"] = decorder_fps;
					item["audio_network_fps"] = audio_network_fps;
					item["audio_decoder_fps"] = audio_decorder_fps;

#ifdef SHOW_STAT
					LOGS(
							"stat[%d], ms: %.2f, %.2f; %.2f, %.2f", window, decorder_delay, render_delay, audio_decorder_delay, audio_play_delay);
#endif

					item["decoder_delay"] = decorder_delay;
					item["render_delay"] = render_delay;
					item["audio_decoder_delay"] = audio_decorder_delay;
					item["audio_play_delay"] = audio_play_delay;

					// [Neo] TODO
					size_t audio_jump = stat->audio_jump_count;

#ifdef SHOW_STAT
					LOGS(
							"stat[%d], left: %d/%d(%d), %dx%d", window, queue_left, audio_left, audio_jump, meta->video_width, meta->video_height);
#endif

					item["audio_left"] = audio_left;
					item["video_left"] = queue_left;

					item["video_jump_fps"] = jump_fps;
					item["audio_jump_fps"] = audio_jump;

					item["video_space"] = stat->video_frame_space;

					item["width"] = meta->video_width;
					item["height"] = meta->video_height;
					item["audio_type"] = meta->audio_type;
					item["audio_enc_type"] = meta->audio_enc_type;

					item["is_omx"] = player->try_omx;
					item["is_turn"] = player->is_turn;
					item["is_playback"] = player->is_playback_mode;
					values.append(item);

					if (false == meta->is_wait_by_ts) {
						// [Neo] adding
						calc[i][adjust_index[i] % ADJUST_SIZE] = network_fps;
						left[i][adjust_index[i] % ADJUST_SIZE] = queue_left;

//						LOGX(
//								"cal:  %3d, %3d, %3d, %3d, %3d\nleft: %3d, %3d, %3d, %3d, %3d", calc[i][0], calc[i][1], calc[i][2], calc[i][3], calc[i][4], left[i][0], left[i][1], left[i][2], left[i][3], left[i][4]);

						adjust_index[i]++;

						if (0 == adjust_index[i] % ADJUST_SIZE) {
							adjust_index[i] = 0;
							adjust_checker[i] = true;
						}

						// [Neo] start to adjust
						if (adjust_checker[i] && decorder_delay > 0) {
							int sum_fps = 0;
							int sum_left = 0;

							for (int j = 0; j < ADJUST_SIZE; j++) {
								sum_fps += calc[i][j];
								sum_left += left[i][j];
							}

							// [Neo] network fps
							int fps = sum_fps / ADJUST_SIZE;

							// [Neo] MIN line
							if (fps < MIN_FPS) {
								fps = MIN_FPS;
							}

							// [Neo] little adjust
							sum_left = sum_left / ADJUST_SIZE;
							if (sum_left > 2) {
								fps += sum_left / 2;
							}

							// [Neo] MAX line
							int max = stat->video_frame_space;
							if (max < meta->video_frame_rate_backup) {
								max = meta->video_frame_rate_backup;
							}
							max *= MAX_FPS_FACTOR;

							sum_fps = 1000 / decorder_delay;
							if (max > sum_fps) {
								max = sum_fps;
							}

							if (fps > max) {
								fps = max;
							}

							if (fps > 0) {
								meta->video_frame_rate = fps;
								meta->video_frame_period = 1000 / fps;
#ifdef SHOW_STAT
								LOGS(
										"stat[%d], fps = %.1f, period = %d", window, meta->video_frame_rate, meta->video_frame_period);
#endif
							}
						}
					}
				}

				stat->video_decoder_count = 0;
				stat->audio_decoder_count = 0;
				stat->video_decoder_delayed = 0l;
				stat->audio_decoder_delayed = 0l;
				stat->video_render_delayed = 0l;
				stat->audio_play_delayed = 0l;
				stat->video_jump_count = 0;
				stat->audio_jump_count = 0;
				stat->video_network_bytes = 0l;
				stat->audio_network_bytes = 0l;
				stat->video_network_count = 0;
				stat->audio_network_count = 0;
				pthread_mutex_unlock(&(stat->mutex));

			}

			if (false == values.empty()) {
				jboolean needDetach = JNI_FALSE;
				JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
						&needDetach);
				if (NULL != env && NULL != g_handle && NULL != g_notifyid) {
					jstring jmsg = env->NewStringUTF(
							writer.write(values).c_str());
					env->CallVoidMethod(g_handle, g_notifyid, CALL_STAT_REPORT,
							(jint) 0, (jint) 0, jmsg);
					env->DeleteLocalRef(jmsg);
				}
				if (JNI_TRUE == needDetach) {
					g_jvm->DetachCurrentThread();
				}
			}
		}

		last_delay = currentMillisSec();
		msleep(1000);
	}

//	LOGW("%s [%p]: X", LOCATE_PT);
	return NULL;
}

/**
 * 连接后的播放线程
 *
 */
void* onPlayVideo(void* _index) {
	int index = (int) _index;
	LOGI("on play vide start ----> index: %d", index);

	int window = array2Window(index);
	LOGX("%s [%p]: E, window = %d", LOCATE_PT, window);

	int delayFrame = 0;
	H264_PACKET h264Pkt = { 0 };
	VO_IN_YUV Vo = { { { 0 } }, 0 };

	int queue_left = 0;
	bool need_jump = true;

	bool is_new_show = true;
	bool is_last_try_omx = false;
	bool is_last_play_back = false;
	bool is_no_picture_from_i = false;
	hls_over_callback = false;

	bool is_buffer_for_rtmp = false;
	float buffer_percent = 0.f;

	long long delayed = 0l;
	long long delayed2 = 0l;

	int need_delay = 0;

	long long timeStamp = -1;

	player_suit* player = g_player[index];

	if (NULL == player) {
		LOGE("%s [%p]: %d has disconnected!!", LOCATE_PT, window);

		jboolean needDetach = JNI_FALSE;
		JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6, &needDetach);
		if (NULL != env) {
			if (NULL != g_handle && NULL != g_notifyid) {
				// [Neo] call back
				env->CallVoidMethod(g_handle, g_notifyid, CALL_CONNECT_CHANGE,
						(jint) window, (jint)BAD_NOT_CONNECT, NULL);
			}

			if (JNI_TRUE == needDetach) {
				g_jvm->DetachCurrentThread();
			}
		}
		return NULL;
	}

	player->is_connecting = true;
	stat_suit* stat = player->stat;
	player_core* core = player->core;
	video_meta* meta = player->vm_normal;

	unsigned char* legacy_payload = NULL;

	if (player->is_playback_mode) {
		meta = player->vm_playback;
	} else {
		meta = player->vm_normal;
	}

	meta->delta_ts = 0ULL;

	int bad_status = BAD_STATUS_NOOP;
	VideoType last_video_type = kVTypeUnknown;

	if (kVTypeH264 == meta->video_type || kVTypeH265 == meta->video_type) {
		if (NULL == core->decoder_handle && BAD_STATUS_NOOP == bad_status) {
			core->decoder_handle = JVD05_DecodeOpenEx(meta->video_type - 1);
			if (NULL == core->decoder_handle) {
				bad_status = BAD_STATUS_FFMPEG;
				LOGE(
						"%s [%p]: window = %d FF open Failed!!", LOCATE_PT, window);
			} else {
				last_video_type = meta->video_type;
			}
		}
	}

	if (BAD_STATUS_NOOP == bad_status && NULL != player->core->opengl_window
			&& false == glOpen(player)) {
		bad_status = BAD_STATUS_OPENGL;
		LOGE("%s [%p]: window = %d OpenGL open failed!!", LOCATE_PT, window);
	}

	// [Neo] started
	is_last_try_omx = player->try_omx;

	while ((player->is_connecting || player->is_connected)
			&& BAD_STATUS_NOOP == bad_status) {

//		LOGI("while video------>bad_status:%d BAD_STATUS_NOOP:%d", bad_status, BAD_STATUS_NOOP);
		// [Neo] fetch current play meta
		if (player->is_playback_mode) {
			meta = player->vm_playback;
		} else {
			meta = player->vm_normal;
		}

		if (!is_hls_offer_end && meta->is_wait_by_ts && is_buffer_for_rtmp) {
			queue_left = get_video_left(player);
			LOGI("queue_left-------> %d", queue_left);
			if (queue_left < meta->video_frame_buffer_count) {
				buffer_percent = queue_left / meta->video_frame_buffer_count
						* 100.0f;

				jboolean needDetach = JNI_FALSE;
				JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
						&needDetach);
				if (NULL != env) {
					if (NULL != g_handle && NULL != g_notifyid) {
						env->CallVoidMethod(g_handle, g_notifyid,
								CALL_PLAY_BUFFER, (jint) window,
								(jint) buffer_percent, NULL);
					}
					if (JNI_TRUE == needDetach) {
						g_jvm->DetachCurrentThread();
					}
				}

				msleep(100);
				continue;
			}
		}

		frame* f = poll_video_frame(player);
		queue_left = get_video_left(player);
		LOGI("video queue left :%d, meta->video_frame_buffer_count :%d",queue_left, meta->video_frame_buffer_count);
		if (OPENGL_TRY_CLOSE == core->opengl_status) {
			glClose(player);
		}

		if (OPENGL_TRY_OPEN == core->opengl_status) {
			glOpen(player);
			need_jump = true;
			is_new_show = true;
		}

		// [Neo] bad boy or paused
		if (NULL == f || NULL == f->buf || queue_left < 0) {
			need_jump = true;
			destroy(f);
			continue;
		}

		// [Neo] dummy ones
		if (DUMMY_FRAME_SIZE == f->size) {

			switch (f->buf[0]) {
			case DUMMY_FRAME_0_O: {
				need_jump = true;
				is_new_show = true;

				if (kVType04 == meta->video_type) {
					if (NULL != core->legacy_handle) {
						if (NULL != legacy_payload) {
							core->legacy_handle->p_payload = legacy_payload;
						}

						JVD04_DecodeClose(core->legacy_handle);
						legacy_payload = NULL;
					}

					JDEC_param_t param;
					memset(&param, 0, sizeof(JDEC_param_t));
					param.i_frame_width = meta->video_width;
					param.i_frame_height = meta->video_height;

					core->legacy_handle = (JDEC_param_t*) JVD04_DecodeOpen(
							&param);

					if (NULL != core->legacy_handle) {
						legacy_payload = core->legacy_handle->p_payload;
					} else {
						bad_status = BAD_STATUS_LEGACY;
						LOGE(
								"%s [%p]: window = %d, legacy open failed!!", LOCATE_PT, window);
					}
				} else {
					if (last_video_type != meta->video_type) {
						if (NULL != core->decoder_handle) {
							JVD05_DecodeCloseEx(core->decoder_handle);
							core->decoder_handle = NULL;
						}

						core->decoder_handle = JVD05_DecodeOpenEx(
								meta->video_type - 1);
						if (NULL == core->decoder_handle) {
							bad_status = BAD_STATUS_FFMPEG;
							LOGE(
									"%s [%p]: window = %d FF open Failed!!", LOCATE_PT, window);
						} else {
							last_video_type = meta->video_type;
						}
					}

					reopenOmx(index);
				}
				break;
			}

			case DUMMY_FRAME_0_FIN: {

				if (player->is_playback_mode) {
					glColor(player, 0, 0, 0, 0);

					jboolean needDetach = JNI_FALSE;
					JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
							&needDetach);
					if (NULL != env) {
						if (NULL != g_handle && NULL != g_notifyid) {
							env->CallVoidMethod(g_handle, g_notifyid,
									CALL_PLAY_DOOMED, (jint) window,
									(jint) PLAYBACK_DONE, NULL);
						}
						if (JNI_TRUE == needDetach) {
							g_jvm->DetachCurrentThread();
						}
					}
				}

				break;
			}

			case DUMMY_FRAME_DIRTY: {
				is_new_show = true;
				if (NULL != core->yuv) {
					core->yuv->i_chroma += 20;
					glRender(player, core->yuv);
				}
				break;
			}

			case DUMMY_FRAME_THUMB: {
				if (NULL != core->yuv && core->yuv->i_height > 0
						&& core->yuv->i_width > 0) {
					if (NULL == core->yuv_thumb) {
						int target_height = (int) ((float) g_thumb_width
								* core->yuv->i_height / core->yuv->i_width);
						target_height = target_height >> 1 << 1;
						core->yuv_thumb = genYUV(g_thumb_width,
								g_thumb_width / 2, g_thumb_width,
								target_height);
					}

					if (core->yuv_thumb->i_height > 0
							&& core->yuv_thumb->i_width > 0) {
						yuv_convert(core->yuv_thumb, core->yuv);
						screenshot(index, core->yuv_thumb, true);
					}
				}
				break;
			}

			case DUMMY_FRAME_COLOR: {
				color_suit* color = (color_suit*) (f->buf + 1);
				glColor(player, color->red, color->green, color->blue,
						color->alpha);
				break;
			}
			case DUMMY_FRAME_HLS_END: {
				LOGI("DUMMY_FRAME_HLS_END--->");
				player->is_connecting = false;
				player->is_connected = false;
				if(hls_over_callback ==false){
					hls_over_callback = true;
					jboolean needDetach = JNI_FALSE;
					JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
							&needDetach);
					if (NULL != env) {
						if (NULL != g_handle && NULL != g_notifyid) {
							env->CallVoidMethod(g_handle, g_notifyid,
									CALL_HLS_PLAY_OVER, (jint) window,
									(jint) 0, NULL);
						}
						if (JNI_TRUE == needDetach) {
							g_jvm->DetachCurrentThread();
						}
					}
				}

				break;
			}

			default:
				break;
			}

			destroy(f);
			continue;
		}

		if (meta->delta_ts < 1) {
			meta->delta_ts = currentMillisSec() - f->ts;
#ifdef DEBUG_TS
			LOGXX("set delta: %10d, %llu (origin)", f->ts, meta->delta_ts);
#endif /* DEBUG_TS */
		}

		if (!meta->is_hls_player_over && is_buffer_for_rtmp
				&& queue_left >= meta->video_frame_buffer_count) {
			LOGI("video buffer finish queue left :%d, video_frame_min_count :%d", queue_left, meta->video_frame_min_count);
			jboolean needDetach = JNI_FALSE;
			JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6, &needDetach);
			if (NULL != env) {
				if (NULL != g_handle && NULL != g_notifyid) {
					env->CallVoidMethod(g_handle, g_notifyid, CALL_PLAY_BUFFER,
							(jint) window, BUFFER_FINISH, NULL);
				}
				if (JNI_TRUE == needDetach) {
					g_jvm->DetachCurrentThread();
				}
			}

			meta->delta_ts = currentMillisSec() - f->ts;
#ifdef DEBUG_TS
			LOGXX(">>>>>>>>>: %10d, %llu", f->ts, meta->delta_ts);
#endif /* DEBUG_TS */
			is_buffer_for_rtmp = false;
		}

		if (!is_hls_offer_end && !meta->is_hls_player_over && meta->is_wait_by_ts && queue_left < meta->video_frame_min_count) {
			LOGI("video buffer start queue left :%d, video_frame_min_count :%d", queue_left, meta->video_frame_min_count);
			jboolean needDetach = JNI_FALSE;
			JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6, &needDetach);
			if (NULL != env) {
				if (NULL != g_handle && NULL != g_notifyid) {
					env->CallVoidMethod(g_handle, g_notifyid, CALL_PLAY_BUFFER,
							(jint) window, BUFFER_START, NULL);
				}
				if (JNI_TRUE == needDetach) {
					g_jvm->DetachCurrentThread();
				}
			}

			is_buffer_for_rtmp = true;
#ifdef DEBUG_TS
			LOGXX("<<<<<<<<<");
#endif /* DEBUG_TS */
		}

		// [Neo] keep starting with Frame I
		if (f->is_play_back != is_last_play_back) {
			need_jump = true;
			is_last_play_back = f->is_play_back;

			if (kVType04 == meta->video_type) {
				if (NULL != core->legacy_handle) {
					if (NULL != legacy_payload) {
						core->legacy_handle->p_payload = legacy_payload;
					}

					JVD04_DecodeClose(core->legacy_handle);
					legacy_payload = NULL;
				}

				JDEC_param_t param;
				memset(&param, 0, sizeof(JDEC_param_t));
				param.i_frame_width = meta->video_width;
				param.i_frame_height = meta->video_height;

				core->legacy_handle = (JDEC_param_t*) JVD04_DecodeOpen(&param);

				if (NULL != core->legacy_handle) {
					legacy_payload = core->legacy_handle->p_payload;
				} else {
					bad_status = BAD_STATUS_LEGACY;
					LOGE(
							"%s [%p]: window = %d, legacy open failed!!", LOCATE_PT, window);
				}
			} else {
				if (NULL != core->decoder_handle) {
					JVD05_DecodeCloseEx(core->decoder_handle);
					core->decoder_handle = NULL;
				}

				core->decoder_handle = JVD05_DecodeOpenEx(meta->video_type - 1);
				if (NULL == core->decoder_handle) {
					bad_status = BAD_STATUS_FFMPEG;
					LOGE(
							"%s [%p]: window = %d FF open Failed!!", LOCATE_PT, window);
				} else {
					last_video_type = meta->video_type;
				}

				reopenOmx(index);
			}

		}

		// [Neo] in case make Frame I go though
		if (f->is_play_back != player->is_playback_mode) {
			need_jump = true;
			f->is_i_frame = false;
		}

		// [Neo] auto jump or man did
		if (meta->video_max_frame_count < queue_left
				&& false == meta->is_wait_by_ts) {
			need_jump = true;
		} else if (player->try_fast_forward) {
			need_jump = true;
			is_new_show = true;
			player->try_fast_forward = false;
		}

		// [Neo] jump, when you are not me(I Frame)
		if ((need_jump && false == f->is_i_frame)
				|| NULL == core->opengl_window) {
			pthread_mutex_lock(&(stat->mutex));
			stat->video_jump_count++;
			pthread_mutex_unlock(&(stat->mutex));

			if (meta->is_wait_by_ts && false == need_jump) {
				if (timeStamp > 0) {
					need_delay = (int) (meta->delta_ts
							- (currentMillisSec() - f->ts));
					if (need_delay < 100) {
						msleep(need_delay);
					} else if (need_delay > 100) {
						meta->delta_ts = currentMillisSec() - f->ts;
					}
				} else {
					timeStamp = currentMillisSec();
				}
			}

		} else {

			if (need_jump && f->is_i_frame) {
				if (meta->is_wait_by_ts
						|| meta->video_frame_rate > queue_left) {
					need_jump = false;
				}
			}

#ifdef DEBUG_DECODER
			if (f->is_i_frame) {
				LOGW(
						"--------- %s [%p]: window = %d --------, delta = %llu, ts = %u", LOCATE_PT, window, meta->delta_ts, f->ts);
			}
#endif

			if (player->try_omx != is_last_try_omx) {
				// [Neo] auto reopen by try_omx
				reopenOmx(index);
				is_last_try_omx = player->try_omx;

				// [Neo] jump when not Frame I started, no need to decode
				if (false == f->is_i_frame) {
					need_jump = true;
					destroy(f);
					continue;
				}
			}

			// [Neo] start decode
			int result = -1;
			h264Pkt.i_payload = f->size;
			h264Pkt.p_payload = f->buf;

			delayed = currentMillisSec();
			if (kVType04 == meta->video_type) {
				if (NULL == core->legacy_handle) {
					JDEC_param_t param;
					memset(&param, 0, sizeof(JDEC_param_t));
					param.i_frame_width = meta->video_width;
					param.i_frame_height = meta->video_height;

					core->legacy_handle = (JDEC_param_t*) JVD04_DecodeOpen(
							&param);

					if (NULL != core->legacy_handle) {
						result = 0;
						need_jump = true;
						is_new_show = true;
						legacy_payload = core->legacy_handle->p_payload;
					}
				} else {
					core->legacy_handle->i_nal_type =
							(f->is_i_frame) ? JVS_TYPE_IDR : JVS_TYPE_P;
					result = decode(TYPE_LEGACY, core->legacy_handle, &h264Pkt,
							&Vo, NULL, NULL);
				}

			} else if (player->try_omx && NULL != core->hdec_handle) {
				result = decode(TYPE_OMX, core->hdec_handle, &h264Pkt, &Vo,
						NULL, NULL);
			} else if (NULL != core->decoder_handle) {
				result = decode(TYPE_FFMPEG, core->decoder_handle, &h264Pkt,
						&Vo, &delayFrame, NULL);
			} else {
				LOGE(
						"%s [%p]: window = %d, BBBBBBBBBBBBBBBBBBBBBBBad with no handle to dec", LOCATE_PT, window);

				core->decoder_handle = JVD05_DecodeOpenEx(meta->video_type - 1);

				if (NULL != core->decoder_handle) {
					last_video_type = meta->video_type;
				}

				reopenOmx(index);

				result = 0;
				need_jump = true;
			}
			delayed = currentMillisSec() - delayed;
			if (delayed > 500) {
				LOGE(
						"%s [%p]: window = %d, dec delayed %llu", LOCATE_PT, window, delayed);
			}

			// [Neo] very bad situation
			if (result < 0) {
				destroy(f);

				if (kVType04 != meta->video_type) {
					if (player->try_omx) {
						closeOmx(player);
						player->try_omx = false;
						need_jump = true;
						continue;
					} else {
						bad_status = BAD_STATUS_DECODE;
						LOGE(
								"%s [%p]: window = %d, FF Fatal error!!", LOCATE_PT, window);
						continue;
					}
				} else {
					bad_status = BAD_STATUS_LEGACY;
					LOGE(
							"%s [%p]: window = %d, legacy open failed!!", LOCATE_PT, window);
					continue;
				}

			} else if (0 == result) {

				if (f->is_i_frame) {
					is_no_picture_from_i = true;
				}

				if (false == is_no_picture_from_i) {
					need_jump = true;
				}

			} else {
				is_no_picture_from_i = false;
			}

			if (result > 0 && NULL != core->opengl_window
					&& NULL != core->opengl_handle) {

#ifdef DEBUG_DECODER
				LOGI(
						"after decode, mode: %d == %d, size: %dx%d == %dx%d", f->is_play_back, player->is_playback_mode, Vo.i_width, Vo.i_height, meta->video_width, meta->video_height);
#endif

				if (meta->video_width != Vo.i_width
						|| meta->video_height != Vo.i_height) {
					meta->video_width = Vo.i_width;
					meta->video_height = Vo.i_height;

					jboolean needDetach = JNI_FALSE;
					JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
							&needDetach);
					if (NULL != env) {
						if (NULL != g_handle && NULL != g_notifyid) {
							Value value;
							FastWriter writer;

							value["width"] = meta->video_width;
							value["height"] = meta->video_height;

							env->CallVoidMethod(g_handle, g_notifyid,
									CALL_PLAY_DOOMED, (jint) window,
									(jint) VIDEO_SIZE_CHANGED,
									env->NewStringUTF(
											writer.write(value).c_str()));
						}
						if (JNI_TRUE == needDetach) {
							g_jvm->DetachCurrentThread();
						}
					}
				}

				// [Neo] check OpenGL status
				if (glReady(player)) {
					// [Neo] if it's new show, callback
					if (is_new_show) {
						is_new_show = false;

						jboolean needDetach = JNI_FALSE;
						JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
								&needDetach);
						if (NULL != env) {
							if (NULL != g_handle && NULL != g_notifyid) {
								env->CallVoidMethod(g_handle, g_notifyid,
										CALL_NEW_PICTURE, (jint) window,
										(jint)(
												player->try_omx
														&& NULL
																!= core->hdec_handle) ?
												1 : 0, NULL);
							}
							if (JNI_TRUE == needDetach) {
								g_jvm->DetachCurrentThread();
							}
						}
					}

					if (timeStamp > 0) {
						if (meta->is_wait_by_ts) {
							need_delay = (int) (meta->delta_ts
									- (currentMillisSec() - f->ts));
#ifdef DEBUG_TS_DETAILS
							LOGX(
									"let check: %10d, %llu", f->ts, meta->delta_ts);
#endif
						} else {
							need_delay = meta->video_frame_period
									- (int) (currentMillisSec() - timeStamp);
						}

						if (need_delay < 100) {
							msleep(need_delay);
						} else if (need_delay > 100) {
#ifdef DEBUG_TS
							LOGXX(
									"overloaded %10d, %llu", f->ts, meta->delta_ts);
#endif
							meta->delta_ts = currentMillisSec() - f->ts;
						}
					}
					timeStamp = currentMillisSec();

#ifdef DEBUG_PLAY
					LOGW(
							"video: ts = %u, delay = %d, left = %d", f->ts, need_delay, queue_left);
#endif

					if (f->is_play_back) {
						Vo.i_chroma += 10;
					}

					// [Neo] ready to show up
					delayed2 = timeStamp;
					glRender(player, &Vo);
					delayed2 = currentMillisSec() - delayed2;
					if (delayed2 > 500) {
						LOGE(
								"%s [%p]: window = %d, render delayed %llu", LOCATE_PT, window, delayed2);
					}

					if (f->is_play_back == player->is_playback_mode) {
						if (NULL == core->yuv) {
							core->yuv = genYUV(Vo.p[0].i_pitch, Vo.p[1].i_pitch,
									Vo.i_width, Vo.i_height);
						} else if (core->yuv->i_width != Vo.i_width
								|| core->yuv->i_height != Vo.i_height) {
							deleteYUV(core->yuv);
							free(core->yuv);
							core->yuv = NULL;
    						core->yuv = genYUV(Vo.p[0].i_pitch, Vo.p[1].i_pitch,
									Vo.i_width, Vo.i_height);
						}

						dupYUV(core->yuv, &Vo);
					}
				}

			}

			pthread_mutex_lock(&(stat->mutex));
			stat->video_decoder_count++;
			stat->video_decoder_delayed += delayed;
			stat->video_render_delayed += delayed2;
			pthread_mutex_unlock(&(stat->mutex));

			delayed = 0l;
			delayed2 = 0l;
		}

		// [Neo] kill you
		if (queue_left > 0) {
			destroy(f);
		}

//		LOGI("while video----end -->bad_status:%d BAD_STATUS_NOOP:%d", bad_status, BAD_STATUS_NOOP);

	} // [Neo] end while

	offer_audio_frame(player, NULL, 0);
	glColor(player, 0, 0, 0, 0);
	glClose(player);
	glDetach(player);

	if (NULL != core->yuv) {
		if (NULL == core->yuv_thumb) {
			int target_height = (int) ((float) g_thumb_width
					* core->yuv->i_height / core->yuv->i_width);
			target_height = target_height >> 1 << 1;
			core->yuv_thumb = genYUV(g_thumb_width, g_thumb_width / 2,
					g_thumb_width, target_height);
		}
		yuv_convert(core->yuv_thumb, core->yuv);
		screenshot(index, core->yuv_thumb, true);
	}

	if (BAD_STATUS_NOOP != bad_status) {
		jboolean needDetach = JNI_FALSE;
		JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6, &needDetach);
		if (NULL != env) {
			if (NULL != g_handle && NULL != g_notifyid) {
				// [Neo] call back
				env->CallVoidMethod(g_handle, g_notifyid, CALL_PLAY_DOOMED,
						(jint) window, (jint) bad_status, NULL);
			}

			if (JNI_TRUE == needDetach) {
				g_jvm->DetachCurrentThread();
			}
		}
	}

	closeOmx(player);

	if (NULL != core->decoder_handle) {
		JVD05_DecodeCloseEx(core->decoder_handle);
		core->decoder_handle = NULL;
	}

	if (NULL != core->legacy_handle) {
		if (NULL != legacy_payload) {
			core->legacy_handle->p_payload = legacy_payload;
		}

		JVD04_DecodeClose(core->legacy_handle);
		legacy_payload = NULL;
		core->legacy_handle = NULL;
	}

	for (int i = 0; i < 10 && player->is_audio_working; i++) {
		offer_audio_frame(player, NULL, 0);
		LOGXX("%s [%p]: window = %d, wait audio", LOCATE_PT, window);
		msleep(500);
	}
//	while(true) {
//		offer_audio_frame(player, NULL, 0);
//		LOGXX("%s [%p]: window = %d, wait audio", LOCATE_PT, window);
//		msleep(500);
//		LOGI("wait audio stop---->");
//		if(!player->is_audio_working)
//			break;
//	}

	deletePlayer(index);

	jboolean needDetach = JNI_FALSE;
	JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6, &needDetach);
	if (NULL != env) {
		if (NULL != g_handle && NULL != g_notifyid) {
			// [Neo] call back
			env->CallVoidMethod(g_handle, g_notifyid, CALL_CONNECT_CHANGE,
					(jint) window, (jint)BAD_NOT_CONNECT, NULL);
		}

		if (JNI_TRUE == needDetach) {
			g_jvm->DetachCurrentThread();
		}
	}
	LOGI("on play vide end ----> index: %d", index);
	LOGX("%s [%p]: X, window = %d", LOCATE_PT, window);
	is_video_end = true;
	return NULL;
}

class EchoHandler: public utils::Handler {
public:
	EchoHandler() {
	}
	~EchoHandler() {
	}

	bool handle(int what, int arg1, int arg2, void *obj) {
		LOGI("echo: %d, %d, %d, %p", what, arg1, arg2, obj);
		return false;
	}

private:
	ONLY_EMPTY_CONSTRUCTION(EchoHandler);
};
#define FRAMESIZE 640

struct STU_AUDIO{
	nplayer::NPlayer* nPlayer;
	unsigned char* audio_data;
	size_t size;
};

void *append_by_data(void* stu_audio){
	struct STU_AUDIO *audio_data;
	audio_data = (struct STU_AUDIO *)stu_audio;
	LOGI("append by data");
	nplayer::NPlayer* nPlayer;
	unsigned char* data;
	size_t size;

	nPlayer = (*audio_data).nPlayer;
	data = (*audio_data).audio_data;
	size = (*audio_data).size;
	while (NULL != nPlayer
			&& false == nPlayer->append_audio_data(data,size)) {
		LOGI(" audio left %d",nPlayer->audio_left());
		msleep(100);
	}

	return NULL;
}
extern FILE *dummyFile ;
void* onPlayAudio(void* _index) {
	int index = (int) _index;
	LOGI("onPlayAudio start---> index: %d", index);
	int window = array2Window(index);
	LOGX("%s [%p]: E, window = %d", LOCATE_PT, window);

	jbyte* audio_out = NULL;

	long long delayed = 0l;
	long long delayed2 = 0l;

	int result = -1;
	int dec_type = -1;
	int last_dec_type = -1;
	int bad_status = BAD_STATUS_NOOP;

	bool can_decode = false;
	bool append_result = false;

	int need_delay = 0;

	player_suit* player = g_player[index];
	player->is_audio_working = true;

	stat_suit* stat = player->stat;
	player_core* core = player->core;
	video_meta* meta = player->vm_normal;

	if (player->is_playback_mode) {
		meta = player->vm_playback;
	} else {
		meta = player->vm_normal;
	}

	dec_type = meta->audio_type;
	last_dec_type = dec_type;

	if (NULL == core->audio_handle && AUDIO_PCM_RAW != dec_type) {
		core->audio_handle = JAD_DecodeOpenEx(dec_type);
		if (NULL == core->audio_handle) {
			bad_status = BAD_STATUS_AUDIO;
			LOGE( "%s [%p]: window = %d, JAD open Failed!!", LOCATE_PT, window);
		}
	}

//	dummyFile = fopen(DUMMY_FILE, "wb");

	while (player->is_connected && BAD_STATUS_NOOP == bad_status) {

//		LOGI("while audio------> bad_status:%d, BAD_STATUS_NOOP:%d",bad_status, BAD_STATUS_NOOP);
		// [Neo] fresh body
		frame* f = poll_audio_frame(player);

		// [Neo] bad boy
		if (NULL == f || NULL == f->buf
				|| (player->is_playback_mode != f->is_play_back)) {
			destroy(f);
			continue;
		}

		result = -1;
		can_decode = false;

		if (player->is_playback_mode) {
			meta = player->vm_playback;
		} else {
			meta = player->vm_normal;
		}

#ifdef _USE_OPENAL_
		if(NULL == player->alu) {
			player->alu = new OpenALUtils();
			if (8 == meta->audio_bit) {
				player->alu->start(kRateDefault, kChannelMono, kPCM8bit, rcb, true);
			} else {
				player->alu->start(kRateDefault, kChannelMono, kPCM16bit, rcb, true);
			}
		}
#else
		if (NULL == player->track) {
			player->track = new AudioTrack();
			if (8 == meta->audio_bit) {
				player->track->start(kRateDefault, kChannelMono, kPCM8bit, rcb,
						true);
			} else {
				player->track->start(kRateDefault, kChannelMono, kPCM16bit, rcb,
						true);
			}
		}
#endif

//		if(NULL == player->nplayer){
//			nplayer::NPlayer *new_nplayer = NULL;
//			nplayer::PlaySuit suit;
//			JAE_HANDLE audio_encoder = NULL;
//
//			EchoHandler* handler = new EchoHandler();
//
//			memset(&suit, 0, sizeof(nplayer::PlaySuit));
//			suit.window = 1;
//			suit.audio_sample_rate = 8000;
//			suit.audio_frame_block = FRAMESIZE;
//			suit.audio_channel_per_frame = 1;
//			suit.audio_bit_per_channel = 16;
//			suit.audio_type = nplayer::kATypeRawPCM;
//
//			suit.enable_denoise = true;
//			suit.enable_vad = false;
//			suit.noise_suppress = -24;
//
//			suit.enable_denoise = true;
//			new_nplayer = new nplayer::NPlayer(&suit, handler);
//			new_nplayer->resume();
//			new_nplayer->enable_audio(true);
//			new_nplayer->adjust_track_volume(adjust_volume);
//			LOGI("adjust_track_volume %f",adjust_volume);
//
//			if (NULL == audio_encoder) {
//				JAE_PARAM param = { 0 };
//				param.iCodecID = 2;
//				param.sample_rate = 8000;
//				param.channels = 1;
//				param.bits_per_sample = 16;
//				param.bytes_per_block = 640;
//
//				audio_encoder = JAE_EncodeOpenEx(&param);
//		//		adec = JAD_DecodeOpenEx(2);
//
//			}
//
//			player->nplayer = new_nplayer;
//		}

		if (f->is_chat_data) {
			dec_type = meta->audio_enc_type;
		} else {
			dec_type = meta->audio_type;
		}

		if (dec_type != last_dec_type) {
			last_dec_type = dec_type;

			if (NULL != core->audio_handle) {
				JAD_DecodeCloseEx(core->audio_handle);
				core->audio_handle = NULL;
			}

			if (AUDIO_PCM_RAW != last_dec_type) {
				core->audio_handle = JAD_DecodeOpenEx(last_dec_type);
				if (NULL == core->audio_handle) {
					bad_status = BAD_STATUS_AUDIO;
					continue;
				}
			}
		}
//		LOGI("while audio run here--->1");
		// [Neo] precheck
		switch (last_dec_type) {
		case AUDIO_PCM_RAW:
			audio_out = (jbyte*) f->buf;
			result = f->size;
			break;

		case JAD_CODEC_SAMR:
			if (FRAME_AMR_SIZE == f->size && NULL != f->buf) {
				can_decode = true;
			}
			break;

		case JAD_CODEC_ALAW:
		case JAD_CODEC_ULAW:
			if (FRAME_G711_SIZE == f->size && NULL != f->buf) {
				can_decode = true;
			}
			break;

		case JAD_CODEC_G729:
			if (FRAME_G729_SIZE == f->size && NULL != f->buf) {
				can_decode = true;
			}
			break;

		}
//		LOGI("while audio run here--->2");
		delayed = currentMillisSec();
//		LOGI("while audio run here--->3 delayed:%d", delayed);
		if (NULL != player->core->audio_handle) {
			if (can_decode) {
				result = JAD_DecodeOneFrameEx(player->core->audio_handle,
						f->buf, (unsigned char**) &audio_out);
			} else {
				LOGE(
						"%s [%p]: window = %d, cannot decode type = %d, %p (%d)!!", LOCATE_PT, window, last_dec_type, f->buf, f->size);
			}
		} else if (AUDIO_PCM_RAW != meta->audio_type) {
			// [Neo] TODO invalid handle when decode, quit
			bad_status = BAD_STATUS_AUDIO;
			LOGE( "%s [%p]: window = %d, invalid handle", LOCATE_PT, window);
			continue;
		}
		delayed = currentMillisSec() - delayed;
//		LOGI("while audio run here--->4 delayed:%d", delayed);
		if (delayed > 500) {
			LOGE(
					"%s [%p]: window = %d, dec delayed %llu", LOCATE_PT, window, delayed);
		}
//		LOGI("while audio run here--->5 result:%d", result);
		if (result > 0) {

			if (meta->is_wait_by_ts && meta->delta_ts > 0) {
				need_delay = (int) (meta->delta_ts
						- (currentMillisSec() - f->ts));
#ifdef DEBUG_AUDIO
				LOGV(
						"audio: delta = %llu, ts = %u, delay = %d, size = %d, is_chat = %d", meta->delta_ts, f->ts, need_delay, f->size, f->is_chat_data);
#endif
//				LOGI("while audio run here--->6 need_delay:%d", need_delay);
				if(need_delay < 3000)
					msleep(need_delay);
				else{
					while(true){
						if(!player->is_connected)
							break;
						if(need_delay > 1000){
							need_delay = need_delay -1000;
							msleep(1000);
						}else if(0 < need_delay < 1000){
							msleep(need_delay);
							break;
						}else if(need_delay<=0){
							break;
						}
					}
				}
			}

//			static int aaa = 0;
//			static FILE* fout = NULL;
//			if (aaa == 0) {
//				fout = fopen("/sdcard/recv.pcm", "wb");
//				LOGE("start write");
//			}
//
//			if (fout) {
//				if (aaa < MAX_FRAME_COUNT) {
//					fwrite(audio_out, result, 1, fout);
//					aaa++;
//				} else if (MAX_FRAME_COUNT == aaa) {
//					fclose(fout);
//					LOGE("write done");
//					aaa++;
//					fout = NULL;
//				}
//			}

#ifdef _USE_OPENAL_
			player->alu->append((unsigned char*) audio_out, result);
#else
			{
//				LOGE("audio_out 0X%x  result %d",audio_out,result);
//				append_result = player->track->append((unsigned char*) audio_out,
//					result);

				if(NULL == audio_out)
					LOGE("audio_out == NULL");
				else{
					if(NULL == player->track){
						LOGE("player->track == null");
					}else{
						if(player->is_connecting || player->is_connected)
							append_result = player->track->append((unsigned char*) audio_out,
											result);
					}
				}
			}

//			player->nplayer->append_audio_data((unsigned char*) audio_out,result);


//		    if(NULL != dummyFile)
//		    	fwrite((unsigned char*) audio_out,result,1,dummyFile);

//			while(player->is_connected && BAD_STATUS_NOOP == bad_status
//					&& false == player->nplayer->append_audio_data((unsigned char*) audio_out,result)) {
////				LOGI("nplayer %p audio left %d data %p size %d ",player->nplayer,player->nplayer->audio_left() ,audio_out,result);
//				msleep(40);
//			}

			//			struct STU_AUDIO stu_audio;
			//			stu_audio.audio_data = (unsigned char*) audio_out ;
			//			stu_audio.size = result;
			//			stu_audio.nPlayer = player->nplayer;
			//			pthread_t pid;
			//			pthread_create(&pid, NULL, append_by_data, &stu_audio);

#ifdef DEBUG_AUDIO
			LOGXX("append audio: %d, size = %d", append_result, f->size);
#endif

			if(NULL != append_result){
				if (false == append_result) {
					pthread_mutex_lock(&(stat->mutex));
					stat->audio_jump_count++;
					pthread_mutex_unlock(&(stat->mutex));
				}
			}

#endif
		}

		if(player->is_connected&& BAD_STATUS_NOOP == bad_status){
			pthread_mutex_lock(&(stat->mutex));
			stat->audio_decoder_count++;
			stat->audio_decoder_delayed += delayed;
			stat->audio_play_delayed += delayed2;
			pthread_mutex_unlock(&(stat->mutex));
		}
		destroy(f);
//		LOGI("while audio end------> bad_status:%d, BAD_STATUS_NOOP:%d",bad_status, BAD_STATUS_NOOP);
	}
//	LOGI("跳出while audio---->");
	if (NULL != core->audio_handle) {
		JAD_DecodeCloseEx(core->audio_handle);
		core->audio_handle = NULL;
	}

//	LOGI("跳出while audio---->222");
	pthread_mutex_lock(&(stat->mutex));
#ifdef _USE_OPENAL_
//	delete player->alu;
//	player->alu = NULL;
#else
//	LOGI("跳出while audio---->3333");
	if (NULL != player->track) {
		player->track->stop();
		delete player->track;
		player->track = NULL;
	}
#endif

//	if (NULL != player->nplayer){
//		player->nplayer->stop_record_audio();
//		player->nplayer->enable_audio(false);
//
//		msleep(150);
//
//		delete player->nplayer;
//		player->nplayer = NULL;
////		LOGI("nplayer[%d] is null",index);
//	}

	pthread_mutex_unlock(&(stat->mutex));

	if (BAD_STATUS_NOOP != bad_status) {
		jboolean needDetach = JNI_FALSE;
		JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6, &needDetach);
		if (NULL != env) {
			if (NULL != g_handle && NULL != g_notifyid) {
				// [Neo] call back
				env->CallVoidMethod(g_handle, g_notifyid, CALL_PLAY_DOOMED,
						(jint) window, (jint) bad_status, NULL);
			}

			if (JNI_TRUE == needDetach) {
				g_jvm->DetachCurrentThread();
			}
		}
	}
//	LOGI("跳出while audio---->444");
	LOGX("%s [%p]: X, window = %d", LOCATE_PT, window);
	player->is_audio_working = false;

	is_audio_end = true;
	return NULL;
}

void* onBar(void* msg) {
	return NULL;
}

void* onFoo(void* msg) {
	return NULL;
}
