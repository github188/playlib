/*////////////////////////////////////////////////////////////////////////////////
// JDEC04.h
// ��ά04���������̬���ӿ�ĵ���ͷ�ļ�
// ������ά���ͿƼ����޹�˾
// ��SDK����C���룬����JVSDKPʹ�ã������⴫
// 2009-11-18�մ���
// 2009-11-18��������
//////////////////////////////////////////////////////////////////////////////////*/



#ifndef _JDEC04_H
#define _JDEC04_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
	#define JDEC04_API __declspec(dllexport)
#else
	#define JDEC04_API
#endif


//��ά��˾�Զ���֡����
#define JVS_TYPE_P			0x00			//ǰ��Ԥ��֡
#define JVS_TYPE_IDR		0x01			//�ؼ�֡
#define JVS_TYPE_AUD		0x02			//��Ƶ֡
#define JVS_TYPE_SKIP		0x03			//SKIP֡���ɺ���
#define JVS_TYPE_D			0x04			//�ɶ���֡
#define JVS_TYPE_B			JVS_TYPE_D		//˫��Ԥ��֡

typedef struct JDEC_param_t
{
    unsigned int	i_frame_width;			//֡��
    unsigned int	i_frame_height;			//֡��
    unsigned int	i_y_pitch;				//Y���ݵ�pitchֵ��һ��Ϊ֡��+64
    unsigned int	i_uv_pitch;				//UV���ݵ�pitchֵ

    unsigned int    i_nal_type;				//֡����
    unsigned int    i_bitstream;			//payload���������ܳ���

    unsigned int	i_payload;				//ʵ�ʱ������ݳ���
    unsigned char *	p_payload;				//ָ��������ݻ�����

    unsigned char * pY;						//ָ��Y���ݻ�����
    unsigned char * pU;						//ָ��U���ݻ�����
    unsigned char * pV;						//ָ��V���ݻ�����

} JDEC_param_t;

// ��ʼ��SDK��Դ�����뱻��һ������
JDEC04_API	void	JVD04_InitSDK();

// �ͷ�SDK��Դ��������󱻵���
JDEC04_API	void	JVD04_ReleaseSDK();

//��һ·������
JDEC04_API	void *	JVD04_DecodeOpen(JDEC_param_t * param);

//�ر�һ·������
JDEC04_API	void	JVD04_DecodeClose(void * h);

//����һ֡����
JDEC04_API	int		JVD04_DecodeOneFrame(void * h);

#ifdef __cplusplus
}
#endif

#endif
