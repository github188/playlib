#ifndef _JVS_AENC_H
#define _JVS_AENC_H

/*****************************************************************************
Copyright (C), 2012-2014, Jovision Tech. co., Ltd.
File Name:      JAENC.h
Author:         wxy(walker)				
Description:
v1.0.0.4 2014-07-23 (EX)
1. add g711 encoder
2. support android

fun:    encode a pcm frame to a g.711 frame
input:  pcm 8k/16bit/1channle 640 bytes
output: amr/alaw/ulaw 8k/16bit/1channel amr(42 bytes) alaw/ulaw(320 bytes)


2012-11-21 
fun:    encode a pcm frame to amr frame
input:  pcm 8k/8/16bit/1channle 320 bytes
output: amr 8k/16bit/1channel amr(21 bytes)
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define JAENC_API __declspec(dllexport)
#else
#define JAENC_API
#endif			


typedef struct JAENC_PARAM
{
	unsigned int		iBitsPerSample;	 // 8/16 bits
    // amr mode
	// iMode no MR mode	  bytes	kbit/s
    // Mode 0 - AMR 4.75 - 13 - 4.75
    // Mode 1 - AMR 5.15 - 14 - 5.15
	// Mode 2 - AMR 5.9  - 16 - 5.9
	// Mode 3 - AMR 6.7  - 18 - 6.7
	// Mode 4 - AMR 7.4  - 20 - 7.4
	// Mode 5 - AMR 7.95 - 21 - 7.95
	// Mode 6 - AMR 10.2 - 27 - 10.2
	// Mode 7 - AMR 12.2 - 32 - 12.2 
	unsigned int		iMode;
}JAENC_PARAM;

// a handle to an amr encoder instance
typedef struct JAENC_ENCODER *	JAENC_HANDLE;

/*****************************************************************************
 *JAE_EncodeOpen:
 *Create an amr encoder instance.
 *In:     JAENC_PARAM *  param
 *return: return a handle to the newly-created instance, or NULL if an error 	  
*****************************************************************************/ 
JAENC_API void * JAE_EncodeOpen(JAENC_PARAM * param);


/*****************************************************************************
 *JAE_EncodeClose:
 *Destroy an amr encoder instance.
 *In:     JAENC_HANDLE h  // a handle to an amr encoder instance
 *Return:   	  
*****************************************************************************/
JAENC_API	void   JAE_EncodeClose(JAENC_HANDLE h);


/*****************************************************************************
 *JAE_EncodeOneFrame:
 *encoder a pcm frame to an amr frame
 *In:     JAENC_HANDLE    h         // a handle to an amr encoder instance
 *In:     unsigned char * pPcmBuf   // pointer to a buffer containing an pcm 
 *                                     frame to encode
 *Out:    unsigned char * pAmrBuf	// pointer to an frame buffer that will 
 *                                     receive the amr frame. hold by caller
 *Return: the size of the amr data				
 *notice: pcm (1) 8k/8 bit/1channel 160Bytes (2) 8k/16bit/1channel 320Bytes   	  
*****************************************************************************/
JAENC_API	int    JAE_EncodeOneFrame(JAENC_HANDLE h,  
							unsigned char * pPcmBuf, unsigned char * pAmrBuf);



#define JAE_ENCODER_SAMR	0
#define JAE_ENCODER_ALAW	1
#define JAE_ENCODER_ULAW	2
#define JAE_ENCODER_G729	3

// a handle to an amr/alaw/ulaw encoder instance
typedef void *	JAE_HANDLE;

typedef struct
{
	int    iCodecID;            // amr/alaw/ulaw
	unsigned int sample_rate; 	/* Should be one of 8000, 16000*/
	int    bits_per_sample;     // Should be 8 or 16
	int    channels;  			/* Should be 1 or 2  					*/
	int    bytes_per_block;     /* size of unit we will read/write in 	*/
}JAE_PARAM;

/*****************************************************************************
 *JAE_EncodeOpenEx:
 *Create an amr/alaw/ulaw encoder instance.
 *In:     JAE_PARAM *  param
 *return: return a handle to the newly-created instance, or NULL if an error 	  
*****************************************************************************/ 
JAENC_API	JAE_HANDLE  JAE_EncodeOpenEx(JAE_PARAM * param);


/*****************************************************************************
 *JAE_EncodeCloseEx:
 *Destroy an amr/alaw/ulaw encoder instance.
 *In:     JAE_HANDLE h  // a handle to an amr/alaw/ulaw encoder instance
 *Return:   	  
*****************************************************************************/
JAENC_API void JAE_EncodeCloseEx(JAE_HANDLE h);


/*****************************************************************************
 *JAE_EncodeOneFrameEx:
 *encoder a pcm frame to an amr/alaw/ulaw frame
 *In:     JAE_HANDLE      h  // a handle to an amr/alaw/ulaw encoder instance
 *In:     unsigned char * pszPcmBuf // pointer to a buffer containing an pcm 
 *                                     frame to encode
 *Out:    unsigned char ** ppszEncBuf//	address of a pointer to an frame 
 *                                      buffer that will receive the
 *                                      amr/alaw/ulaw frame
 *Return: the size of the amr/ulaw/alaw data				
 *notice: pcm 8k/16bit/1channel 640 bytes
 *        amr 8k/16bit/1channel 42 bytes alaw/ulaw 320 bytes 
*****************************************************************************/
JAENC_API int  JAE_EncodeOneFrameEx(JAE_HANDLE h, 
                      unsigned char * pszPcmBuf, unsigned char ** ppszEncBuf);


#ifdef __cplusplus
	}
#endif

#endif	// _JVS_AENC_H

