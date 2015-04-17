#ifndef _JVS_AUDIO_DEC_H
#define _JVS_AUDIO_DEC_H

/*****************************************************************************
 Copyright (C), 2012-2014, Jovision Tech. co., Ltd.
 File Name:     JADEC.h
 Author:        wxy(walker)
 Date:          2012-11-21 - 2014-08-08
 Description:
 V1.0.0.4 2014-08-01
 added the _EX function and interface
 support amr and g.711 decode

 2014-07-23
 supported android

 2013-5-16
 fun:    decode a g.711 frame to a pcm frame
 input:  amr 8k/16bit/1channel alaw/ulaw size
 output: pcm 8k/16bit/1channle 320 bytes

 2012-11-21
 fun:    decode a amr frame to a pcm frame
 input:  amr 8k/16bit/1channel amr size hold mode 5
 output: pcm 8k/16bit/1channle 320 bytes

*****************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define JAD_API __declspec(dllexport)
#else
#define JAD_API
#endif

// a handle to an amr/alaw/ulaw decoder instance
typedef void *  JADEC_HANDLE;

// decode type: samr, alaw, ulaw
#define	JAD_CODEC_SAMR		0
#define	JAD_CODEC_ALAW		1
#define	JAD_CODEC_ULAW		2
#define	JAD_CODEC_G729		3


/*****************************************************************************
 *JAD_DecodeOpen:
 *create an amr/alaw/ulaw decoder instance			
 *In:     int iACodec; JAD_CODEC_(SAMR/ALAW/ULAW)
 *Return: return a handle to the newly-created instance, 
 *        or NULL if an error occurred 
*****************************************************************************/
JAD_API	JADEC_HANDLE JAD_DecodeOpen		(int iACodec);


/*****************************************************************************
 *JAD_DecodeClose:
 *destroy a amr/alaw/ulaw decoder instance.
 *In:     JADEC_HANDLE       h        // decoder handle
 *Return:			
*****************************************************************************/
JAD_API	void JAD_DecodeClose		(JADEC_HANDLE h);



/*****************************************************************************
 *JAD_DecodeOneFrame:
 *decode an amr/alaw/ulaw frame to a pcm frame.
 *        amr/alaw/ulaw 8k/16bit/1channel amr(21bytes) g.711(len)
 *        pcm           8k/16bit/1channel 320byts
 *In:     JADEC_HANDLE       h        // decoder handle
 *In:     unsigned char *    pszIn    // the amr/alaw/ulaw data pointer
 *                                       hold the data by the caller
 *In:     int                inLen	  // the length of the amr/alaw/ulaw data
 *Out:    unsigned char **   ppszOut  // hold the pcm data by the decoder
 *Return: return the size of pcm data if successful,
          or -1 if an error occurred
 *notice: if amr, need call two 
*****************************************************************************/
JAD_API	int JAD_DecodeOneFrame(JADEC_HANDLE h, unsigned char *pszIn,
                                          int inLen, unsigned char **ppszOut);


// version: EX
typedef void *    JAD_HANDLE;    // decoder handle

/*****************************************************************************
 *JAD_DecodeOpenEx:
 *create an amr/alaw/ulaw decoder instance
 *In:     int iACodec; JAD_CODEC_(SAMR/ALAW/ULAW)
 *Return: return a handle to the newly-created instance,
 *        or NULL if an error occurred 
*****************************************************************************/
JAD_API	JAD_HANDLE JAD_DecodeOpenEx(int iACodec);



/*****************************************************************************
 *JAD_DecodeCloseEx:
 *destroy a amr/alaw/ulaw decoder instance.
 *In:     JADEC_HANDLE       h        // decoder handle
 *Return:			
*****************************************************************************/
JAD_API void JAD_DecodeCloseEx(JAD_HANDLE h);


/*****************************************************************************
 *JAD_DecodeOneFrameEX:
 *decode an amr/alaw/ulaw frame to a pcm frame.
 *        amr/alaw/ulaw 8k/16bit/1channel amr(21bytes) g.711(len)
 *        pcm 8k/16bit/1channel 320byts
 *In:     JADEC_HANDLE       h        // decoder handle
 *In:     unsigned char *    pszIn	  // the amr/alaw/ulaw data pointer
 *                                       hold the data by the caller
 *                                       len: amr (42)  alaw/ulaw (320)
 *Out:    unsigned char **	 ppszOut  // hold the pcm data by the decoder
 *                                       len: pcm (640)												
 *Return: return the size of pcm data if successful, 
 *        or < 0 if an error occurred
*****************************************************************************/
JAD_API int JAD_DecodeOneFrameEx(JAD_HANDLE h, unsigned char * pszIn, 
                                                    unsigned char ** ppszOut);


#ifdef __cplusplus
}
#endif

#endif // _JVS_AUDIO_DEC_H