#include "playmp4.h"


/*thread func*/
static void * Play(void *args)
{
    int result = -1;

    PlayMP4 *player = (PlayMP4 *)args;

    unsigned int sampleId = 0;
    int delayFrame = 0;
    AV_UNPKT AvUnpkt;
    H264_PACKET  h264Pkt;
    VO_IN_YUV Vo = { { { 0 } }, 0 };

    player->SetRunFlag(true);
    if(player->get_opengl_status() == OPENGL_OPENGED)
    {
        int last_timep = 0;
        do{
            for (sampleId = 1; sampleId <= player->mp4Info.iNumVideoSamples; sampleId++)
            {

                if(player->GetQuitFlag())
                {
                    break;
                }
                pthread_mutex_lock(player->GetMutex());
                /*judge whether suspend*/
                while (player->GetSuspendFlag()) {
                    pthread_cond_wait(player->GetCond(), player->GetMutex());
                }
                pthread_mutex_unlock(player->GetMutex());

                player->opengl_open();

                AvUnpkt.iSampleId	=  sampleId; //iKeyFrameNo;
                AvUnpkt.iType		= JVS_UPKT_VIDEO;

                BOOL bRet = JP_UnpkgOneFrame(player->upkHandle, &AvUnpkt);

                if(!bRet)
                {
                    LOGE("JP_UnpkgOneFrame failed!");
                    break;

                }

                h264Pkt.i_payload = AvUnpkt.iSize;
                h264Pkt.p_payload = AvUnpkt.pData;

                result = player->decode(TYPE_FFMPEG, player->decoder_handle, &h264Pkt,
                        &Vo, &delayFrame, NULL);

                int delay = AvUnpkt.iSampleTime - last_timep - 5;
                if(delay > 0)
                {
                    msleep(delay);
                }

                if(result > 0)
                {
                    LOGE("reday to play...");
                    player->opengl_render(&Vo);
                }
                else
                {
                    LOGE("failed to play...:%d", result);
                }

                last_timep = AvUnpkt.iSampleTime;
            }
        }while(0);
    }
    player->SetRunFlag(false);
    player->destroy();
    LOGD("the play thread[%lu] is finished...\n", pthread_self());
}

PlayMP4::PlayMP4(JNIEnv *env, jobject surface)
{
    _env = env;
    _surface  = surface;
    opengl_status = OPENGL_UNATTACHED;
    _uri = "";
	upkHandle = NULL;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    isuspend = false;
    is_produce_quit_ = false;
    is_produce_run_ = false;
}

int PlayMP4::setURI(string uri)
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

int PlayMP4::opengl_attach()
{

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
        }
        else{
				LOGE("open failed, with bad status = %d!!", opengl_status);
        }
    }
    else{
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


int PlayMP4::prepare()
{
    int ret = -1;
    int iDecVCodec = 0;
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
        }
        else{
            ret = -3;//uri is null
            return ret;
        }


        if (NULL != _env && NULL != _surface) {
            opengl_window = ANativeWindow_fromSurface(_env,
                        _surface);
            if(NULL != opengl_window){
                opengl_status = OPENGL_ATTACHED;
                ret = 0;
                break;
            }
            else{
                return -2;
            }
        }
        else
        {
            LOGE("the env or surface is NULL");
            return -1;
        }
    }while(0);

    if(ret != 0){
        if (NULL != decoder_handle) {
            JVD05_DecodeCloseEx(decoder_handle);
            decoder_handle = NULL;
        }
        if(NULL != upkHandle){
            JP_CloseUnpkg(upkHandle);
        }
        opengl_detach();
    }

}

int PlayMP4::start()
{
    int ret = 0;
    ret = pthread_create(&play_id_, NULL, Play, this);

    if (ret != 0) {
        LOGE("PlayMP4 pthread_create error : %d\n", ret);
    }

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
}
int PlayMP4::destroy()
{
    opengl_close();
    opengl_detach();
    if (NULL != decoder_handle) {
        JVD05_DecodeCloseEx(decoder_handle);
        decoder_handle = NULL;
    }
    if(NULL != upkHandle){
        JP_CloseUnpkg(upkHandle);
        upkHandle = NULL;
    }
}

int PlayMP4::stop()
{
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
}
