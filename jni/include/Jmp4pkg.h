#ifndef _JVS_PKG_H
#define _JVS_PKG_H

/************************************************************
Copyright (C), 2012-2015, Jovision Tech. co., Ltd.
File Name:		Jmp4pkg.h
Author:			������
Version:		Ver 1.2.1.3
Description:
			    2015-05-12
				����h265֧��
				��װ����:
				1. ʵ�ֱ�׼h264����/amr����/g711������װ
				��mp4��ʽ�ļ�, һ��ֻ��װһ֡

				2.�����װmp4ʱ,��֡��Ϣ����Ϊ��ʱ�ļ���
				���ڽ�����ڷ�װmp4����(��¼�߷�)

				���װ����:
				1. ʵ�ֱ�׼h264������amr\g.711������
				mp4�ļ����װ, һ��ֻ���һ������

				2. ����ʱ�ļ��ж�ȡ֡��mp4�ļ��е���Ϣ��
				���ڽ�����ڷ�װ��mp4�ļ�


************************************************************/

#ifdef WIN32

	typedef __int64					int64_t;
	typedef unsigned __int64		uint64_t;


	#ifdef __cplusplus
	#define JVS_API extern "C" __declspec(dllexport)
	#else
	#define JVS_API __declspec(dllexport)
	#endif

#else

	typedef long long				int64_t;
	typedef unsigned long long		uint64_t;


#ifndef TRUE
	typedef int BOOL;
	#define TRUE  1
	#define FALSE 0
#endif

	#ifdef __cplusplus
	#define JVS_API extern "C"
	#else
	#define JVS_API
	#endif

#endif

	// Ҫ��װ����Ƶ������ (Ĭ��Ϊh264)
#define JVS_VCODEC_H264 0x100
#define JVS_VCODEC_HEVC 0x200

// Ҫ��װ����Ƶ������
#define JVS_ACODEC_SAMR		0
#define JVS_ACODEC_ALAW		1
#define JVS_ACODEC_ULAW		2

// ��JP_PackageOneFrame ��Ƚ�avPkt.type
#define JVS_PKG_VIDEO		1			// ��װ��Ƶ����
#define JVS_PKG_AUDIO		2			// ��װ��Ƶ����

// mp4 sps\pps, ��h264�򿪱���������,
// 1. ����ʱsps��ppsҪȥ��ǰ��� 00 00 01/ 00 00 00 01
// 2. Ҳ���Բ����� �� iSpsSize = 0; pPps = NULL(Ч�ʽ���?)
typedef struct _MP4_AVCC
{
	int					iSpsSize;			// h264 sps��С
	int					iPpsSize;			// h264 pps��С
	unsigned char *		pSps;				// h264 sps
	unsigned char *		pPps;				// h264 pps

}MP4_AVCC;

// ��װ��ʼʱ������Ƶ����
typedef struct _PKG_VIDEO_PARAM
{

	int					iFrameWidth;		// ֡���
	int					iFrameHeight;		// ֡�߶�
	float				fFrameRate;			// ֡��
	// ���������sps\pps, ���뽫pSps = NULL, iSpsSize = 0;
	MP4_AVCC			avcC;

}PKG_VIDEO_PARAM, * PPKG_VIDEO_PARAM;


// ��װ����������(��Ƶ����Ƶ)
typedef struct _AV_PACKET
{
	unsigned int		iType;				// ��װ֡����(��Ƶ����Ƶ)
	unsigned char *		pData;				// ��װ����ָ��
	unsigned int		iSize;				// ��װ���ݴ�С
	int64_t				iPts;				// �Ժ���Ϊ��λ������ʱ�����
	int64_t				iDts;				// δʹ��
}AV_PACKET, * PAV_PACKET;


typedef struct _MP4_PKG *	MP4_PKG_HANDLE;	//��װ���, ��Open����

/**********************************************************************************
Function:		JP_OpenPackage
Description:	�򿪷�װ����JP_ClosePackage�ɶ�ʹ��
Param:			���в������ɵ����߶�����
				PPKG_VIDEO_PARAM pVideoParam	// ��װ��Ƶ������� wdith, sps..
				BOOL			 bWriteVideo	// �Ƿ�д����Ƶ
				BOOL			 bWriteAudio	// �Ƿ�д����Ƶ
				char *			 pszmp4file     // ����Ϊmp4�ļ���
				char *			 pszIdxFile		// ���������ļ�,����֡��mp4�ļ��е�������Ϣ,
												// ������ڷ�װ���̵�mp4�ļ�
												// (��չ��.jdx, �ļ�����mp4��ͬ)
				int				 iAcodec		// Ҫ��װ����Ƶ������
				int				 iChannel		// linux����Ҫ�����ͨ����


Return:			���ط�װ�����, NULL,��ʾʧ��,�����ʾ�ɹ�
**********************************************************************************/
JVS_API	MP4_PKG_HANDLE	JP_OpenPackage		(PPKG_VIDEO_PARAM	pVideoParam,
											 BOOL				bWriteVideo,
											 BOOL				bWriteAudio,
											 char *				pszmp4file,
											 char *				pszIdxFile,
											 int				iAcodec,
											 int				iChannel);



/**********************************************************************************
Function:		JP_ClosePackage
Description:	�رշ�װ����JP_OpenPackage�ɶ�ʹ��
Param:			MP4_PKG_HANDLE	h		// ��װ�����
Return:			��
**********************************************************************************/
JVS_API	void			JP_ClosePackage		(MP4_PKG_HANDLE h);



/**********************************************************************************
Function:		JP_PackageOneFrame
Description:	��װһ֡h264��amr����
Param:			MP4_PKG_HANDLE	h		// ��װ�����
				PAV_PACKET pAVPkt		// h264��amr֡���ݽṹ��

Return:			���� TURE, ��ʾ�ɹ�������FALSE,��ʾʧ��
**********************************************************************************/
JVS_API	BOOL			JP_PackageOneFrame	(MP4_PKG_HANDLE h, PAV_PACKET pAVPkt);



#ifndef	WIN32
/**********************************************************************************
Function:		JP_InitSDK
Description:	ÿ·����iSize��С�Ļ���, linux���ȵ�����ֻ����һ��
Param:			int			iSize		// ÿ·������Ĵ�С
				int			iCount		// �����·��

Return:			���� TURE, ��ʾ�ɹ�������FALSE,��ʾʧ��
**********************************************************************************/
JVS_API	BOOL			JP_InitSDK(int iSize, int iCount);


/**********************************************************************************
Function:		JP_ReleaseSDK
Description:	�ͷ�JP_InitSDK����Ļ���
Param:			int			iSize		// ÿ·������Ĵ�С
				int			iCount		// �����·��

Return:			���� TURE, ��ʾ�ɹ�������FALSE,��ʾʧ��
**********************************************************************************/
JVS_API	void			JP_ReleaseSDK();
#endif






// ��JP_UnpkgOneFrame ��Ƚ�avUnpkt.type
#define  JVS_UPKT_VIDEO		1			// �����Ƶ����
#define  JVS_UPKT_AUDIO		2			// ������Ƶ����

// �򿪽��������mp4�ļ���Ϣ,�ɵ�����ά�ִ˽ṹ��
typedef struct _MP4_INFO
{
	char				szVideoMediaDataName[8];	// ��Ƶ�������� "avc1"/"hev1"/"hvc1"
	unsigned int		iFrameWidth;				// ��Ƶ���
	unsigned int		iFrameHeight;				// ��Ƶ�߶�
	unsigned int		iNumVideoSamples;			// VideoSample����
	double				dFrameRate;					// ֡��

	char				szAudioMediaDataName[8];	// ��Ƶ�������� "samr" "alaw" "ulaw"
	unsigned int		iNumAudioSamples;			// AudioSample����


}MP4_INFO, *PMP4_INFO;

// ������������(��Ƶ����Ƶ),�ɵ�����ά�ִ˽ṹ��
typedef struct _AV_UNPKT
{
	unsigned int		iType;			// �������	(����)
	unsigned int		iSampleId;		// ���Sample ID (����) (1 -- iNumVideoSamples)
	unsigned char *		pData;			// ����������ָ�� �����ֻά��һ�ε��õ�����
	unsigned int		iSize;			// ���������ݴ�С
	uint64_t			iSampleTime;	// ������ʱ��� (�Ժ���Ϊ��λ) 0 40 80...
	BOOL				bKeyFrame;		// �Ƿ�Ϊ�ؼ�֡(���)
}AV_UNPKT, *PAV_UNPKT;

typedef struct _MP4_UPK *		MP4_UPK_HANDLE;	// ����� ��open����


/**********************************************************************************
Function:		JP_OpenUnpkg
Description:	�򿪽������JP_CloseUnpkg�ɶ�ʹ��
Param:
				char *			pszmp4file		// Ҫ�����ļ���
				PMP4_INFO		pMp4Info		// �����AudioSample��AudioSample����
				unsigned int 	iBufSize		// setvbuf����
												// linux dvr��ʹ��(��Ϊ0, 64kB)

Return:			���ؽ�������, NULL, ��ʾʧ��,�����ʾ�ɹ�
**********************************************************************************/
JVS_API MP4_UPK_HANDLE  JP_OpenUnpkg	(char *pszmp4file, PMP4_INFO pMp4Info, unsigned int iBufSize);



/**********************************************************************************
Function:		JP_CloseUnpkg
Description:	�رս������JP_OpenUnpkg�ɶ�ʹ��
Param:
				MP4_PKG_HANDLE	h				// ��������
Return:			��
**********************************************************************************/
JVS_API void			JP_CloseUnpkg	(MP4_UPK_HANDLE h);



/**********************************************************************************
Function:		JP_UnpkgOneFrame
Description:	���һ֡h264/amr/g711����
Param:			MP4_UPK_HANDLE	h				// ��������
				PAV_UNPKT pAvUnpkt				// ���h264��amr/g711֡���ݽṹ��
				// if (pAvUnpkt->iSampleId <= 0 || pAvUnpkt->iSampleId > iNumVideoSamples)
				//	  return FALSE;

Return:			���� TURE,��ʾ�ɹ�������FALSE,��ʾʧ��
**********************************************************************************/
JVS_API BOOL			JP_UnpkgOneFrame(MP4_UPK_HANDLE h, PAV_UNPKT pAvUnpkt);



/**********************************************************************************
Function:		JP_UnpkgKeyFrame
Description:	���Ҹ���֡�Ÿ����Ĺؼ�֡
Param:			MP4_UPK_HANDLE	h				// ��������
				unsigned int	iVFrameNo		// �������Ƶ֡��
				BOOL			bForward		// ��ʾ���ҷ���,
												// TRUE��ǰ(����), FALSE, ���(����)
												// ����������

Return:			���عؼ�֡֡��	-2,��ʾ����,   -1, ��ʾû���ҵ�
**********************************************************************************/
JVS_API int				JP_UnpkgKeyFrame(MP4_UPK_HANDLE h, unsigned int iVFrameNo, BOOL bForward);


/**********************************************************************************
Function:		JP_PkgGetAudioSampleId
Description:	��ȡ��Ƶ֡��, ����Ƶ֡��pts�ǲ�������Ƶpts
Param:			MP4_UPK_HANDLE	h				// ��������
				unsigned int	iVFrameNo		// �������Ƶ֡��
				uint64_t *      VPts			// ������Ƶpts, ���Դ�NULL
				uint64_t *		APts			// ������Ƶpts, ���Դ�NULL

Return:			������Ƶ֡��	-2,��ʾ����,   -1, ��ʾû���ҵ�
**********************************************************************************/
JVS_API int JP_PkgGetAudioSampleId(MP4_UPK_HANDLE h, unsigned int iVFrameNo, uint64_t * piVPts, uint64_t * piAPts);



typedef struct _MP4_FILE *	MP4_FILE_HANDLE;		// ������ڷ�װ��mp4�ļ����

// �ɵ�����ά�ִ˽ṹ��
typedef struct _MP4_CHECK
{
	BOOL			bNormal;						// TRUE, ��ʾ����ɷ�װ,FALSE, ��װ���ڽ�����
	unsigned int	iDataStart;						// ���mp4�ļ����ݿ�ʼλ��
}MP4_CHECK, *PMP4_CHECK;

// ��mp4�ļ�������Ƶ��Ϣ,�ɵ�����ά�ִ˽ṹ��
typedef struct _JP_MP4_INFO
{
	BOOL				bHasVideo;					// �Ƿ�����Ƶ
	BOOL				bHasAudio;					// �Ƿ�����Ƶ

	char				szVideoMediaDataName[8];	// ��Ƶ�������� "avc1"
	unsigned int		iFrameWidth;				// ��Ƶ���
	unsigned int		iFrameHeight;				// ��Ƶ�߶�
	double				dFrameRate;					// ֡��
	unsigned int		iNumVideoSamples;			// VideoSample����, ��mp4�ļ�ʱ���ص�ǰ��֡��

	char				szAudioMediaDataName[8];	// ��Ƶ�������� "samr" "alaw" "ulaw"
	unsigned int		iNumAudioSamples;			// AudioSample����, ��mp4�ļ�ʱ���ص�ǰ��֡��
}JP_MP4_INFO, *PJP_MP4_INFO;

/**********************************************************************************
Function:		JP_CheckFile
Description:	���mp4�Ƿ�Ϊ���ڷ�װ���ļ������Ƿ�Ϊ�������ļ�
				Ҫ���mp4�ļ�,�����ȵ�������ӿڣ��Ծ���������һ��API,
				��� pMp4Check.bNormal = TRUE, Ҫ���������API,
				������������API
Param:
				char *			pszmp4file			// Ҫ���mp4�ļ���
				PMP4_CHECK		pMp4Check			// mp4�ļ��ļ����

Return:			TRUE,���ɹ�, FALSE, ���ʧ��
**********************************************************************************/
JVS_API BOOL			JP_CheckFile	(char *pszmp4file, PMP4_CHECK pMp4Check);




/**********************************************************************************
Function:		JP_OpenFile
Description:	��mp4�ļ���JP_CloseFile�ɶ�ʹ��
Param:
				char *			pszmp4file	// Ҫ�����ļ���
				unsigned int	iDataStart	// ���mp4�ļ����ݿ�ʼλ��
				char *			pszIdxFile	// ���ڸ��������ļ�,���ļ��ɷ�װ������
				unsigned int 	iBufSize	// setvbuf����
											// linux dvr��ʹ��(��Ϊ0, 64kB)

Return:			���ؽ�������, NULL,��ʾʧ��,�����ʾ�ɹ�
**********************************************************************************/
JVS_API MP4_FILE_HANDLE	JP_OpenFile		(char *			pszmp4file,
										 unsigned int	iDataStart,
										 char *			pszIdxFile,
										 PJP_MP4_INFO	pMp4Info,
										 unsigned int	iBufSize);



/**********************************************************************************
Function:		JP_ReadFile
Description:	���һ֡h264��amr����
Param:			MP4_FILE_HANDLE	h			// ��������
				PAV_UNPKT		pAvUnpkt	// ���h264��amr֡���ݽṹ��

				// if (pAvUnpkt->iSampleId <= 0 || pAvUnpkt->iSampleId > iNumVideoSamples)
				//	  return 0;

Return:			0, ��ʾ�����ݿɶ�,����������
**********************************************************************************/
JVS_API int				JP_ReadFile		(MP4_FILE_HANDLE mp4Handle, PAV_UNPKT pAvUnpkt);




/**********************************************************************************
Function:		JP_CloseFile
Description:	�رս������JP_OpenFile�ɶ�ʹ��
Param:
				MP4_FILE_HANDLE	h			// ��������

Return:			��
**********************************************************************************/
JVS_API void			JP_CloseFile	(MP4_FILE_HANDLE mp4Handle);




/**********************************************************************************
Function:		JP_ReadKeyFrame
Description:	���Ҹ���֡�Ÿ����Ĺؼ�֡
Param:			MP4_FILE_HANDLE	h					// ��������
				unsigned int	iVFrameNo			// �������Ƶ֡��
				BOOL			bForward			// ��ʾ���ҷ���,
													// TRUE��ǰ(����), FALSE, ���(����)
													// ����������
Return:			���عؼ�֡֡��	-2,��ʾ����,   -1, ��ʾû���ҵ�
**********************************************************************************/
JVS_API int				JP_ReadKeyFrame (MP4_FILE_HANDLE h, unsigned int iVFrameNo, BOOL bForward);


/**********************************************************************************
Function:		JP_JdxGetAudioSampleId
Description:	��ȡ��Ƶ֡��, ����Ƶ֡��pts�ǲ�������Ƶpts
Param:			MP4_FILE_HANDLE	h				// ��������
				unsigned int	iVFrameNo		// �������Ƶ֡��
				uint64_t *      VPts			// ������Ƶpts, ���Դ�NULL
				uint64_t *		APts			// ������Ƶpts, ���Դ�NULL

Return:			������Ƶ֡��	-2,��ʾ����,   -1, ��ʾû���ҵ�
**********************************************************************************/
JVS_API int JP_JdxGetAudioSampleId(MP4_FILE_HANDLE h, unsigned int iVFrameNo, uint64_t * piVPts, uint64_t * piAPts);


#endif	// _JVS_PKG_H
