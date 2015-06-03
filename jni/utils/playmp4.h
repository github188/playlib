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
    PlayMP4(JNIEnv* env, jobject surface);
    int setURI(string uri);
    int prepare();
    int start();
    int pause();
    int resume();
    int stop();
    int destroy();

    int get_opengl_status()
    {
        return opengl_status;
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

    JDEC05_HANDLE decoder_handle;
    MP4_INFO mp4Info;
    MP4_UPK_HANDLE	upkHandle;

    int opengl_attach();
    int opengl_detach();
    int opengl_open();
    int opengl_close();
    int opengl_render(PVO_IN_YUV pVo);
    int decode(int type, void* handler, H264_PACKET* in, PVO_IN_YUV out, int* arg1, int* arg2);

private:
    JNIEnv *_env;
    jobject _surface;
    string _uri;
	int opengl_status;
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
