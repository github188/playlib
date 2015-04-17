#ifndef _JDEC05_H
#define _JDEC05_H

/*****************************************************************************
Copyright (C), 2012-2014, Jovision Tech. co., Ltd.
File Name:      JDEC05.h
Author:         wxy(walker)				
Description:
decode an standard h.264 frame to a yv12(yuv420p) frame.	
				
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
	
#ifdef WIN32
#define JDEC05_API __declspec(dllexport)
#else
#define JDEC05_API
#endif

#define JVDEC_TYPE_H264 0
#define JVDEC_TYPE_HEVC 1

// decode input h264, hold by caller
typedef struct _H264_PACKET
{
    unsigned int        i_payload; // h264 data size
    unsigned char *     p_payload; // h264 data pointer
}H264_PACKET, *PH264_PACKET;

// decode output yv12(yuv420p), hold by decoder
typedef struct _YUV_PACKET
{
    unsigned int        i_frame_width;
    unsigned int        i_frame_height;
    unsigned int        i_y_pitch;
    unsigned int        i_u_pitch;
    unsigned int        i_v_pitch;
    unsigned char *     pY;
    unsigned char *     pU;
    unsigned char *     pV;
    // Zero if no frame could be decompressed, otherwise, it is > 0
    int                 i_got_picture;
} YUV_PACKET, *PYUV_PACKET;


typedef void * JDEC05_HANDLE; // h264 decoder handle

/*****************************************************************************
 *JVD05_InitSDK:
 *the first call and called only once
 *In:
 *Return: return 1 if successful, or 0 if an error occurred
*****************************************************************************/ 
JDEC05_API int JVD05_InitSDK();


/*****************************************************************************
 *JVD05_ReleaseSDK:
 *must be the last call
 *In:
 *Return:
*****************************************************************************/ 
JDEC05_API void JVD05_ReleaseSDK();


/*****************************************************************************
 *JVD05_DecodeOpen:
 *create a h264 decoder instance
 *In:
 *Return: return a handle to the newly-created instance, 
 *        or NULL if an error occurred 
*****************************************************************************/
//JDEC05_API JDEC05_HANDLE JVD05_DecodeOpen();


/*****************************************************************************
 *JVD05_DecodeClose:
 *destroy a h264 decoder instance
 *In:     JDEC05_HANDLE h
 *Return: 
*****************************************************************************/
//JDEC05_API void JVD05_DecodeClose(JDEC05_HANDLE h);


/*****************************************************************************
 *JVD05_DecodeOneFrame
 *decode an standard h.264 frame to a yv12(yuv420p) frame.
 *In:     JDEC05_HANDLE h
 *In:     PH264_PACKET  pH264Pkt    // input h264
 *Out:    PYUV_PACKET * ppYuvPkt    // output yv12(yuv420p)
 *In:     int *         bDelayFrame // When using the decoding end
 *                                  // bDelayFrame = 1, else bDelayFrame = 0
 *Return: On error a negative value is returned, otherwise the number of
 *        bytes used or zero if no frame could be decompressed.
*****************************************************************************/
//JDEC05_API int JVD05_DecodeOneFrame(JDEC05_HANDLE h, PH264_PACKET pH264Pkt,
//                                     PYUV_PACKET *ppYuvPkt, int *bDelayFrame);

/**********************************************************************************
Function:		JVD05_DecodeOpen
Description:	�򿪽�������JVD05_DecodeClose�ɶ�ʹ��
Param:				
				int iCodec 
				#define JVDEC_TYPE_H264 0
				#define JVDEC_TYPE_HEVC 1

Return:			���ؽ��������,	NULL,��ʾʧ��,�����ʾ�ɹ�
**********************************************************************************/
JDEC05_API	JDEC05_HANDLE JVD05_DecodeOpenEx	(int iCodec);



/**********************************************************************************
Function:		JVD05_DecodeClose
Description:	�رս�������JVD05_DecodeOpen�ɶ�ʹ��
Param:				
				JDEC05_HANDLE		h		// ���������

Return:			��
**********************************************************************************/
JDEC05_API	void JVD05_DecodeCloseEx	(JDEC05_HANDLE h);



/**********************************************************************************
Function:		JVD05_DecodeOneFrame
Description:	����һ֡��׼h264����Ϊyv12��ʽ
Param:				
				JDEC05_HANDLE		h			// ���������
				PH264_PACKET		pH264Pkt	// ��������h264/h265�ṹ��ָ��
				PYUV_PACKET *		ppYuvPkt	// �������YUV�ṹ��ָ��
				int *				bDelayFrame // TRUE, FALSE,�Ƿ�����ӳ�֡,
												// �������ʱʹ��,����������
												// DelayFrameʱ,�᷵��FALSE

Return:			���ؽ����ֽ���, ������ʧ��,����-1
**********************************************************************************/
JDEC05_API	int	JVD05_DecodeOneFrameEx(JDEC05_HANDLE h, PH264_PACKET pH264Pkt, PYUV_PACKET *ppYuvPkt, int *bDelayFrame);


#ifdef __cplusplus
}
#endif

#endif	// _JDEC05_H
