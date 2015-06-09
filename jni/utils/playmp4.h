#ifndef _PLAYMP4_H_
#define _PLAYMP4_H_

#include <jni.h>
#include <signal.h>
#include "../defines.h"
#include "commons.h"
#include <iostream>
using namespace std;
class PlayMP4{
public:
    PlayMP4();
    void setURI(string uri);
    int prepare(JNIEnv* env);
    int start(JNIEnv *env, jobject surface);
    int pause();
    int resume();
    int stop(int stop_seconds);
    int destroy();

    int get_opengl_status()
    {
        return opengl_status;
    };
    int get_audio_dectype()
    {
        return dec_type;
    };
    bool GetQuitFlag() {
        return is_produce_quit_;
    };

    pthread_mutex_t *GetMutex() {
        return &mutex;
    };
    pthread_cond_t *GetCond() {
        return &cond;
    };
    bool GetSuspendFlag() {
        return isuspend;
    };
    void SetRunFlag(int flag) {
        is_produce_run_ = flag;
    };

    int GetPlayTotalTime() {
        return total_seconds;
    };
    void SetPlayTotalTime(int time) {
        total_seconds = time;
    };

    int GetStopSeconds() {
        return stop_seconds_;
    };
    void SetStopSeconds(int seconds) {
        stop_seconds_ = seconds;
    };

    JDEC05_HANDLE decoder_handle;
    JADEC_HANDLE audio_handle;
    MP4_INFO mp4Info;
    MP4_UPK_HANDLE	upkHandle;
    AudioTrack* track;

    int opengl_attach(JNIEnv *env, jobject surface);
    int opengl_detach();
    int opengl_open();
    int opengl_close();
    int opengl_render(PVO_IN_YUV pVo);
    int decode(int type, void* handler, H264_PACKET* in, PVO_IN_YUV out, int* arg1, int* arg2);

private:

    string _uri;
	int opengl_status;
	int dec_type;
	int total_seconds;
	int stop_seconds_;
	int video_width;
	int video_height;
	ANativeWindow* opengl_window;
	JVO_HANDLE opengl_handle;

	VO_IN_YUV* yuv;
	VO_IN_YUV* yuv_thumb;

	MP4_CHECK		Mp4Check;

	pthread_t play_id_;

    pthread_mutex_t mutex;
    pthread_cond_t cond;
    /*produce thread run interval*/
    volatile sig_atomic_t is_produce_quit_;
    volatile sig_atomic_t is_produce_run_;
    /*volatile isuspend;*/
    volatile bool isuspend;
};
#endif // _PLAYMP4_H_
