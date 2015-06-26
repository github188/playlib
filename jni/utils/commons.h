// Generated by Neo

#ifndef _JNI_COMMON_H
#define _JNI_COMMON_H

#include <jni.h>

#include "defines.h"

#include <JVideoOut.h>
#include <net_defs.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void msleep(int millis);
long long currentMillisSec();
ENCODING detectEncoding(char* str);

int getValidArrayIndex(int window);
int window2Array(int window);
int array2Window(int index);
void invalidArray(int index);

bool checkHeader(BYTE* buf, int size);
bool skipHeader(BYTE* buf);
bool checkStartCode(int startCode);
bool isDisconnected(int uchType);
int getUsefulAudioType(int type);

BYTE* convertAudioData(jbyte* data);

VO_IN_YUV* genYUV(int y_pitch, int uv_pitch, int width, int height);
void dupYUV(VO_IN_YUV* dst, VO_IN_YUV* src);
void deleteYUV(VO_IN_YUV* yuv);

player_suit* genPlayer(int index);
void deletePlayer(int index);

bool openOmx(player_suit* player);
bool closeOmx(player_suit* player);

bool reopenOmx(int window);

bool glResume(JNIEnv* env, player_suit* player, jobject surface);
bool glPause(player_suit* player);
bool glDetach(player_suit* player);
bool glAttach(JNIEnv* env, player_suit* player, jobject surface);
bool glOpen(player_suit* player);
bool glClose(player_suit* player);
int glRender(player_suit* player, PVO_IN_YUV packet);
int glColor(player_suit* player, float red, float green, float blue,
		float alpha);
bool glReady(player_suit* player);

int screenshot(int index, PVO_IN_YUV pVo, bool is_thumb = false);

frame* poll_video_frame(player_suit* player);
frame* poll_audio_frame(player_suit* player);

void offer_video_frame(player_suit* player, BYTE* buf, int size, int type,
		bool is_play_back = false, unsigned int ts = 0);
void offer_audio_frame(player_suit* player, BYTE* buf, int size,
		bool is_chat_data = false, bool is_play_back = false, unsigned int ts =
				0);

int get_video_left(player_suit* player);
int get_audio_left(player_suit* player);

bool clean_all_queue(player_suit* player);
bool destroy(frame* f);

int decode(int type, void* handle, H264_PACKET* in, PVO_IN_YUV out, int* arg1,
		int* arg2);

void yuv_convert(VO_IN_YUV* dst, VO_IN_YUV* src);
void yuv_translate(unsigned char * pDstY, int iDstYPitch, unsigned char * pDstU,
		unsigned char * pDstV, int iDstUVPitch, int iDstWidth, int iDstHeight,
		unsigned char * pSrcY, int iSrcYPitch, unsigned char * pSrcU,
		unsigned char * pSrcV, int iSrcUVPitch, int iSrcWidth, int iSrcHeight);

char* byte2echo(BYTE* buf, int offset, int length);
char* byte2char(BYTE* buf, int offset, int length);

JNIEnv* genAttachedEnv(JavaVM* jvm, jint version, jboolean* needDetach);

jbyte* getNativeByte(JNIEnv* env, jbyteArray jarray);
jbyte* getNativeByteByLength(JNIEnv* env, jbyteArray jarray, jint offset,
		jint length);
jbyteArray genByteArray(JNIEnv* env, jbyte* buf, jint offset, jint length);

char* getNativeChar(JNIEnv* env, jstring jstr);
jstring genString(JNIEnv* env, const char* buf, int offset, int length);

jobjectArray genJObjectArray(JNIEnv* env, jint length);

/**
 * download file method
 * in:file
 */
int downloadFile(FILE *fp);
#ifdef __cplusplus
}
#endif

#endif /* _JNI_COMMON_H */
