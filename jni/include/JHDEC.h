#ifndef _JVS_HDECODER_H
#define	_JVS_HDECODER_H
/*****************************************************************************
Copyright (C), 2012-2014, Jovision Tech. co., Ltd.
File Name:      JHDEC.h
Author:         wxy(walker)				
Description:
decode an standard h.264 frame to a yv12(yuv420p)(?) frame.	in android
				
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

typedef void * JHD_ANDROID_HANDLE; // hdec android handle

// not used yet!
typedef enum _HDEC_ANDROID_YUV
{
    JDEC_YV12,
    JDEC_NV12,
    JDEC_QOMX_COLOR_FormatYUV420PackedSemiPlanar64x32Tile2m8ka,
}HDEC_ANDROID_YUV;

// hdec input h264, hold by caller
typedef struct _HDEC_ANDROID_H264_PKT {
    unsigned int    i_payload;
    unsigned char * p_payload;
}HDEC_ANDROID_H264_PKT, *PHDEC_ANDROID_H264_PKT;


// yuv plane
typedef struct _HDEC_ANDROID_PLANE
{
    unsigned char *	 p_pixels; // y/u/v data pointer
    int              i_pitch;  // y/u/v pitch
}HDEC_ANDROID_PLANE, *PHDEC_ANDROID_PLANE;

// hdec ouput yuv, hold by decoder
typedef struct _HDEC_ANDROID_YUV_PKT
{
    HDEC_ANDROID_PLANE p[4];  // y/u/v
    unsigned int    i_chroma; // not used yet!
    unsigned int    i_width;  // y width
    unsigned int    i_height; // u width
}HDEC_ANDROID_YUV_PKT, *PHDEC_ANDROID_YUV_PKT;

/*****************************************************************************
 *JHD05_InitSDK:
 *the first call and called only once
 *In: void * JavaVM  // need input
 *Return: return 1 if successful, or 0 if an error occurred
*****************************************************************************/ 
int JHD05_InitSDK(void * JavaVM);

/*****************************************************************************
 *JHD05_ReleaseSDK:
 *must be the last call
 *In:
 *Return:
*****************************************************************************/ 
void JHD05_ReleaseSDK();

/*****************************************************************************
 *JHD05_OpenDecoder:
 *create a h264 hdecoder instance
 *In:
 *Return: return a handle to the newly-created instance, 
 *        or NULL if an error occurred 
*****************************************************************************/
JHD_ANDROID_HANDLE 	JHD05_OpenDecoder(int width, int height);

/*****************************************************************************
 *JHD05_CloseDecoder:
 *destroy a h264 hdecoder instance
 *In:     JHD_ANDROID_HANDLE h
 *Return: 
*****************************************************************************/
void JHD05_CloseDecoder(JHD_ANDROID_HANDLE h);

/*****************************************************************************
 *JHD05_DecodeOneFrame
 *hdecode an standard h.264 frame to a yv12(yuv420p)(?) frame.
 *In:     JHD_ANDROID_HANDLE     h
 *In:     PHDEC_ANDROID_H264_PKT pH264Pkt    // input h264
 *Out:    PHDEC_ANDROID_YUV_PKT *ppYuvPkt    // output yv12(yuv420p)(?)
 *In:     int * got_picture            // Zero if no frame could be
 *                                     // decompressed, otherwise, it is > 0
 *Return: On error a negative value is returned, otherwise the number of
 *        bytes used or zero if no frame could be decompressed.
*****************************************************************************/
int JHD05_DecodeOneFrame(JHD_ANDROID_HANDLE h,
                         PHDEC_ANDROID_H264_PKT pH264Pkt, 
                         PHDEC_ANDROID_YUV_PKT *ppYuvPkt, 
                         int * got_picture);

/*
 2014-10-14
*/

typedef struct _OMX_INFO
{
      char sz[120];
}OMX_INFO;

typedef int (*OMX_INFO_CALLBACK)(OMX_INFO * info);

/*****************************************************************************
 *JHD_SetCallBack_OmxInfo
 *OMX_INFO_CALLBACK omxInfoCallBack
 *Return: return 1 if successful, or 0 if an error occurred
*****************************************************************************/
int JHD_SetCallBack_OmxInfo(OMX_INFO_CALLBACK omxInfoCallBack);

#ifdef __cplusplus
}
#endif

#endif // _JVS_HDECODER_H
