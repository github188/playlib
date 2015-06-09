#include <json.h>
#include "playmp4.h"

using namespace Json;

static void rcb_pri(ReportWhat what, ReportArg1 arg1, ReportArg2 arg2, const void* data) {
	LOGV("report: %d, %d, %d, %p", what, arg1, arg2, data);
}

static int current_sec = 0;
static int last_sec = 0;
/*thread func*/
static void * Play(void *args)
{
    int result = -1;
    int audio_result = -1;
    PlayMP4 *player = (PlayMP4 *)args;

    unsigned int sampleId = 0;
    int can_decode = 0;
    jbyte* audio_out = NULL;
    int delayFrame = 0;
    AV_UNPKT AvUnpkt;
    AV_UNPKT AvUnpktAudio;
    H264_PACKET  h264Pkt;
    VO_IN_YUV Vo = { { { 0 } }, 0 };
    current_sec = 0;
    last_sec = 0;
    player->SetRunFlag(true);
    player->opengl_open();
    string errmsg = "unknown error";

    if(player->get_opengl_status() == OPENGL_OPENGED)
    {
        int last_timep = 0;

        do{

            //此处实现播放界面home掉后，重新进来，继续从上次播放时间继续播放
            int restart_seconds = player->GetStopSeconds();
            int restart_sampleid = restart_seconds*player->mp4Info.dFrameRate;
            if(restart_sampleid == 0)
            {
                //首次进来，从1开始
                sampleId = 1;
            }
            else{
                //先查找附近的Izhen
                int iIFrame = JP_UnpkgKeyFrame(player->upkHandle, restart_sampleid, FALSE);
                if(iIFrame < 0){
                    LOGE("JP_UnpkgKeyFrame failed：%d, sampleId:%d",iIFrame,restart_sampleid);
                }
                else{
                    LOGE("JP_UnpkgKeyFrame success, ret:%d, sampleId:%d",iIFrame,restart_sampleid);
                    sampleId = iIFrame;
                }

            }
            if(player->GetQuitFlag())
            {
                result = 1;
                break;
            }
            LOGE("play mp4 start from sampleId:%d", sampleId);

            for (; sampleId <= player->mp4Info.iNumVideoSamples; sampleId++)
            {

                if(player->GetQuitFlag())
                {
                    result = 1;
                    break;
                }
                pthread_mutex_lock(player->GetMutex());
                /*judge whether suspend*/
                while (player->GetSuspendFlag()) {
                    pthread_cond_wait(player->GetCond(), player->GetMutex());
                }
                pthread_mutex_unlock(player->GetMutex());

                AvUnpkt.iSampleId	=  sampleId; //iKeyFrameNo;
                AvUnpkt.iType		= JVS_UPKT_VIDEO;

                long long  unpkg_start = currentMillisSec();
                BOOL bRet = JP_UnpkgOneFrame(player->upkHandle, &AvUnpkt);

                if(!bRet)
                {
                    errmsg = "JP_UnpkgOneFrame video failed!";
                    LOGE("%s",errmsg.c_str());
                    break;

                }
                if(player->mp4Info.iNumAudioSamples > 0)
                {
                    AvUnpktAudio.iSampleId	=  sampleId; //iKeyFrameNo;
                    AvUnpktAudio.iType = JVS_UPKT_AUDIO;

                    BOOL bRet = JP_UnpkgOneFrame(player->upkHandle, &AvUnpktAudio);

                    if(!bRet)
                    {
                        errmsg = "JP_UnpkgOneFrame audio failed!";
                        LOGE("%s",errmsg.c_str());
                        break;

                    }
                    switch (player->get_audio_dectype()) {
                    case AUDIO_PCM_RAW:
                        audio_out = (jbyte*) AvUnpktAudio.pData;
                        audio_result = AvUnpktAudio.iSize;
                        break;

                    case JAD_CODEC_SAMR:
                        if (FRAME_AMR_SIZE == AvUnpktAudio.iSize && NULL != AvUnpktAudio.pData) {
                            can_decode = true;
                        }
                        break;

                    case JAD_CODEC_ALAW:
                    case JAD_CODEC_ULAW:
                        if (FRAME_G711_SIZE == AvUnpktAudio.iSize && NULL != AvUnpktAudio.pData) {
                            can_decode = true;
                        }
                        break;

                    case JAD_CODEC_G729:
                        if (FRAME_G729_SIZE == AvUnpktAudio.iSize && NULL != AvUnpktAudio.pData) {
                            can_decode = true;
                        }
                        break;

                    }
                    if (NULL == player->track) {
                        player->track = new AudioTrack();

                        player->track->start(kRateDefault, kChannelMono, kPCM16bit, rcb_pri,
                                    true);
                    }
                    if (NULL != player->audio_handle) {
                        if (can_decode) {
                            audio_result = JAD_DecodeOneFrameEx(player->audio_handle,
                                    AvUnpktAudio.pData, (unsigned char**) &audio_out);
                        } else {
                            errmsg = "cannot decode audio type";
                            LOGE(
                                    "%s [%p]: cannot decode type = %d (%d)!!", LOCATE_PT, player->get_audio_dectype(), AvUnpktAudio.iSize);
                        }
                    }
                }
                h264Pkt.i_payload = AvUnpkt.iSize;
                h264Pkt.p_payload = AvUnpkt.pData;

                result = player->decode(TYPE_FFMPEG, player->decoder_handle, &h264Pkt,
                        &Vo, &delayFrame, NULL);

                long long unpkg_end = currentMillisSec();
                int delay_decode  = (int)(unpkg_end - unpkg_start);
                int delay = AvUnpkt.iSampleTime - last_timep - delay_decode;

                if(delay > 0)
                {
                    msleep(delay-3);//减3是吧opengl渲染的时间减去
                }

                if(result > 0)
                {
                    //更新进度条
                    current_sec = sampleId/(player->mp4Info.dFrameRate);
                    if(current_sec > last_sec)
                    {
                        last_sec = current_sec;
                        jboolean needDetach = JNI_FALSE;
                        JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
                            &needDetach);
                         if (NULL != env && NULL != g_handle && NULL != g_notifyid) {
                            env->CallVoidMethod(g_handle, g_notifyid, CALL_MP4_TIME_INFO,
                                    (jint)current_sec , (jint) player->GetPlayTotalTime(), NULL);
                        }
                        if (JNI_TRUE == needDetach) {
                            g_jvm->DetachCurrentThread();
                        }
                        //LOGE("play second:%d s", current_sec);
                    }

                    //LOGE("decode delay:%d, new delay:%d", delay_decode, delay);
                    //LOGE("reday to play...");

                    player->opengl_render(&Vo);
                    if(can_decode)
                    {
                        int append_result = player->track->append((unsigned char*) audio_out,
                        audio_result);
                        //LOGE("append audio: %d, size = %d", append_result, AvUnpktAudio.iSize);
                    }

                }
                else
                {
                    errmsg = "decode video failed";
                    LOGE("failed to play...:%d", result);
                }

                last_timep = AvUnpkt.iSampleTime;
            }
        }while(0);
    }
    if(player->GetStopSeconds() == 0 || (player->GetStopSeconds()==player->GetPlayTotalTime()))
    {
        //这属于用户自行推出，不是home隐藏
        int call_back_ret = CALL_MP4_PLAY_FIN;//结束
        if(result > 0)
        {
            errmsg = "success";
            call_back_ret = CALL_MP4_PLAY_FIN;//结束
        }
        else
        {
            call_back_ret = CALL_MP4_PLAY_ERROR;//失败
        }

        jboolean needDetach = JNI_FALSE;
        JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
            &needDetach);
        jstring jmsg = env->NewStringUTF(
                errmsg.c_str());
         if (NULL != env && NULL != g_handle && NULL != g_notifyid) {
            env->CallVoidMethod(g_handle, g_notifyid, call_back_ret,
                    (jint)current_sec , (jint) player->GetPlayTotalTime(), jmsg);
        }
        env->DeleteLocalRef(jmsg);
        if (JNI_TRUE == needDetach) {
            g_jvm->DetachCurrentThread();
        }
    }
    else{
        if(result <= 0){
            jboolean needDetach = JNI_FALSE;
            JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
                &needDetach);
            jstring jmsg = env->NewStringUTF(
                    errmsg.c_str());
             if (NULL != env && NULL != g_handle && NULL != g_notifyid) {
                env->CallVoidMethod(g_handle, g_notifyid, CALL_MP4_PLAY_ERROR,
                        (jint)current_sec , (jint) player->GetPlayTotalTime(), jmsg);
            }
            env->DeleteLocalRef(jmsg);
            if (JNI_TRUE == needDetach) {
                g_jvm->DetachCurrentThread();
            }
        }
        else{
            if(current_sec == player->GetPlayTotalTime())
            {
                jboolean needDetach = JNI_FALSE;
                JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
                    &needDetach);
                jstring jmsg = env->NewStringUTF(
                        "OK");
                 if (NULL != env && NULL != g_handle && NULL != g_notifyid) {
                    env->CallVoidMethod(g_handle, g_notifyid, CALL_MP4_PLAY_FIN,
                            (jint)current_sec , (jint) player->GetPlayTotalTime(), jmsg);
                }
                env->DeleteLocalRef(jmsg);
                if (JNI_TRUE == needDetach) {
                    g_jvm->DetachCurrentThread();
                }
            }
        }
    }

    player->destroy();
    player->SetRunFlag(false);
    LOGD("the play thread[%lu] is finished...\n", pthread_self());
    return NULL;
}

PlayMP4::PlayMP4()
{
    opengl_status = OPENGL_UNATTACHED;
    _uri = "";
	upkHandle = NULL;
	opengl_handle = NULL;
	audio_handle = NULL;
	track = NULL;
    dec_type = -1;
    total_seconds = 0;
    stop_seconds_ = 0;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    isuspend = false;
    is_produce_quit_ = false;
    is_produce_run_ = false;
}

void PlayMP4::setURI(string uri)
{
    _uri = uri;
}


int PlayMP4::opengl_detach()
{
    if (NULL != opengl_window) {
        ANativeWindow_release(opengl_window);
        opengl_window = NULL;
        opengl_status = OPENGL_UNATTACHED;
    }
    return 0;
}

int PlayMP4::opengl_open()
{
    if(NULL != opengl_window && NULL == opengl_handle)
    {
        if (OPENGL_ATTACHED == opengl_status
					|| OPENGL_TRY_OPEN == opengl_status) {
            opengl_handle = JVO_Open(opengl_window);
            if (NULL !=opengl_handle) {
                opengl_status = OPENGL_OPENGED;
            }
            else{
                LOGE("open failed, opengl_handle is NULL");
            }
            return 0;
        }
        else
        {
            LOGE("open failed, with bad status = %d!!", opengl_status);
            return -2;
        }
    }
    else{
        LOGE("open failed, opengl_window is NULL or opengl_handle is not null");
        return -1;
    }
}

int PlayMP4::opengl_close()
{
    int ret = 0;

    if (NULL != opengl_handle) {
        if (OPENGL_OPENGED == opengl_status
                || OPENGL_TRY_CLOSE == opengl_status) {
            JVO_Close(opengl_handle);
            opengl_handle = NULL;
            opengl_status = OPENGL_ATTACHED;
        } else {
            ret = -1;
            LOGE("opengl_close failed, with bad status = %d!!", opengl_status);
        }
    }
    return ret;
}

int PlayMP4::opengl_render(PVO_IN_YUV pVo)
{
	int result = -1;

	if (NULL != pVo) {
		if (NULL != opengl_window
				&& NULL != opengl_handle) {
			if (OPENGL_OPENGED == opengl_status) {
#ifdef DEBUG_DECODER
				LOGE(
						"render.chroma: %d, %dx%d", pVo->i_chroma, pVo->i_width, pVo->i_height);
#endif
				result = JVO_Render(opengl_handle, pVo);
			} else {
				LOGW(
						"render failed, with bad status = %d", opengl_status);
			}
		}
	}

	return result;
}

int PlayMP4::decode(int type, void *handler, H264_PACKET* in, PVO_IN_YUV out, int* arg1, int* arg2)
{
	// [Neo] < 0 fatal, reopen or quit
	// [Neo] = 0 no picture
	// [Neo] > 0 show up
	int got = -1;
	int result = -1;

	PYUV_PACKET packet = NULL;
	PHDEC_ANDROID_YUV_PKT packet_hd = NULL;

	out->i_width = 0;
	out->i_height = 0;
	out->i_chroma = (out->i_chroma + 1) % 10;

    result = JVD05_DecodeOneFrameEx((JDEC05_HANDLE) handler, in, &packet, arg1);
    got = packet->i_got_picture;

    if (result > 0 && NULL != packet && got > 0) {
        out->p[0].i_pitch = packet->i_y_pitch;
        out->p[2].i_pitch = packet->i_u_pitch;
        out->p[1].i_pitch = packet->i_v_pitch;

        out->p[0].p_pixels = packet->pY;
        out->p[2].p_pixels = packet->pU;
        out->p[1].p_pixels = packet->pV;

        out->i_width = packet->i_frame_width;
        out->i_height = packet->i_frame_height;
    }

#ifdef DEBUG_DECODER
	LOGI(
			"deocde: type = %d, size = %d, got/result = %d/%d, render = %p", type, in->i_payload, got, result, out);
#endif

	if (result > 0 && 0 == got) {
		result = 0;
	}

	return result;
}


int PlayMP4::prepare(JNIEnv *env)
{
    int ret = -1;
    int iDecVCodec = 0;
    int total_s = 0;

    video_width = 0;
    video_height = 0;
    do{
        if(strlen(_uri.c_str()) > 0){

            upkHandle = JP_OpenUnpkg((char *)_uri.c_str(), &mp4Info, 0);
            if(NULL == upkHandle){
                LOGE("JP_OpenUnpkg failed, the return handle is NULL");
                return -4;
            }
            LOGE("Video:\nWidth:\t%d\nHeight:\t%d\nEncode:\t%s\nfps:\t%.2f\nFrame:\t%d\n",
                        mp4Info.iFrameWidth, mp4Info.iFrameHeight, mp4Info.szVideoMediaDataName,
                        mp4Info.dFrameRate, mp4Info.iNumVideoSamples);

            LOGE("Audio:\nEncode:\t%s\nFrame:\t%d\n", mp4Info.szAudioMediaDataName, mp4Info.iNumAudioSamples);

            total_s = (int)(mp4Info.iNumVideoSamples/mp4Info.dFrameRate);

            if (strcmp(mp4Info.szVideoMediaDataName, "avc1") == 0)
            {
                iDecVCodec = JVDEC_TYPE_H264;
            }
            else if ( (strcmp(mp4Info.szVideoMediaDataName, "hev1") == 0)
                 || (strcmp(mp4Info.szVideoMediaDataName, "hvc1") == 0))
            {
                iDecVCodec = JVDEC_TYPE_HEVC;
            }
            else{
                ret = -5;
                break;
            }
            decoder_handle = JVD05_DecodeOpenEx(iDecVCodec);
            if(NULL == decoder_handle){
                 ret = -6;
                 break;
            }
            else{
                ret = 0;
            }
            if(strcmp("samr", mp4Info.szAudioMediaDataName) == 0)
            {
                dec_type = JAD_CODEC_SAMR;

            }else if(strcmp("alaw", mp4Info.szAudioMediaDataName) == 0)
            {
                dec_type = JAD_CODEC_ALAW;

            }else if(strcmp("ulaw", mp4Info.szAudioMediaDataName) == 0)
            {
                dec_type = JAD_CODEC_ULAW;

            }else{
                LOGE("unknown audio dec type:%s", mp4Info.szAudioMediaDataName);
            }
            if(mp4Info.iNumAudioSamples > 0 && dec_type!=-1)
            {
                audio_handle = JAD_DecodeOpenEx(dec_type);
                if (NULL == audio_handle) {
                    //bad_status = BAD_STATUS_AUDIO;
                    LOGE( "%s [%p]: JAD open Failed!!", LOCATE_PT);
                }
            }

        }
        else{
            ret = -3;//uri is null
            return ret;
        }

    }while(0);

    if(ret != 0){
        if (NULL != decoder_handle) {
            JVD05_DecodeCloseEx(decoder_handle);
            decoder_handle = NULL;
        }
        if(NULL != audio_handle)
        {
            JAD_DecodeCloseEx(audio_handle);
            audio_handle = NULL;
        }
        if(NULL != upkHandle){
            JP_CloseUnpkg(upkHandle);//则个函数貌似崩溃，需要联系网修养
            upkHandle = NULL;
        }

    }
    else
    {
        Value res_root;
        FastWriter writer;
        SetPlayTotalTime(total_s);
        res_root["width"] = mp4Info.iFrameWidth;
        res_root["height"] = mp4Info.iFrameHeight;
        res_root["length"] = total_s;//总时间长度S
        res_root["video_type"] = mp4Info.szVideoMediaDataName;
        res_root["audio_type"] = mp4Info.szAudioMediaDataName;

        string str_jsonres = writer.write(res_root);
        jboolean needDetach = JNI_FALSE;
        JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6,
                &needDetach);

        if (NULL != env && NULL != g_handle && NULL != g_notifyid) {
            jstring jmsg = env->NewStringUTF(
                    str_jsonres.c_str());

            env->CallVoidMethod(g_handle, g_notifyid, CALL_MP4_PRE_INFO,
                    (jint) 0, (jint) total_s, jmsg);

            env->DeleteLocalRef(jmsg);
        }
        if (JNI_TRUE == needDetach) {
            g_jvm->DetachCurrentThread();
        }
    }
    return ret;
}

int PlayMP4::opengl_attach(JNIEnv *env, jobject surface)
{
    int ret = -1;
    do{
        if (NULL != env && NULL != surface) {
            opengl_window = ANativeWindow_fromSurface(env,
                        surface);
            if(NULL != opengl_window){
                opengl_status = OPENGL_ATTACHED;
                ret = 0;
                break;
            }
            else{
                ret = -2;
                break;
            }
        }
        else
        {
            LOGE("the env or surface is NULL");
            ret = -1;
            break;
        }
    }while(0);

    return ret;
}

int PlayMP4::start(JNIEnv *env, jobject surface)
{
    int ret = 0;
    is_produce_quit_ = false;
    ret = opengl_attach(env, surface);

    if(ret == 0)
    {
        ret = pthread_create(&play_id_, NULL, Play, this);

        if (ret != 0) {
            LOGE("PlayMP4 pthread_create error : %d\n", ret);
        }
        else{
             LOGE("PlayMP4 pthread_create success(%lu)\n", play_id_);
        }
    }
    if(ret != 0)
    {
        opengl_detach();
    }
    return ret;
}

int PlayMP4::pause() {
    pthread_mutex_lock(&mutex);
    if(!isuspend) isuspend = true;
    pthread_mutex_unlock(&mutex);
    return 0;
}

int PlayMP4::resume() {

    pthread_mutex_lock(&mutex);
    do
    {
        if(isuspend){
            isuspend = false;
            pthread_cond_signal(&cond);
        }
        else{
            break;
        }
    }while(0);
    pthread_mutex_unlock(&mutex);

    return 0;
}
int PlayMP4::destroy()
{
	if (NULL != track)
    {
		track->stop();
		delete track;
		track = NULL;
	}
    int ret = opengl_close();
    if(ret != 0)
    {
        return ret;
    }
    ret = opengl_detach();
    if(ret != 0)
    {
        return ret;
    }
    if (NULL != decoder_handle) {
        JVD05_DecodeCloseEx(decoder_handle);
        decoder_handle = NULL;
    }
    if(NULL != audio_handle)
    {
        JAD_DecodeCloseEx(audio_handle);
        audio_handle = NULL;
    }
    if(NULL != upkHandle){
        JP_CloseUnpkg(upkHandle);
        upkHandle = NULL;
    }

    return 0;
}

int PlayMP4::stop(int stop_seconds)
{
    stop_seconds_ = stop_seconds;
    if(is_produce_quit_) {
        LOGD("long long ago,the play thread is already finished...\n");
        return 0;
    }
    /*if the thread is suspended, then need resume first*/
    if (isuspend) resume();
    LOGD("the play thread is stopping...\n");
    is_produce_quit_ = true;
    if(!is_produce_run_){
        return 0;
    }

    return 0;
}
