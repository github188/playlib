/*////////////////////////////////////////////////////////////////////////////////
// JDEC04.h
// 中维04版解码器动态连接库的导出头文件
// 济南中维世纪科技有限公司
// 该SDK采用C编译，仅供JVSDKP使用，请勿外传
// 2009-11-18日创建
// 2009-11-18日最后更新
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


//中维公司自定义帧类型
#define JVS_TYPE_P			0x00			//前向预测帧
#define JVS_TYPE_IDR		0x01			//关键帧
#define JVS_TYPE_AUD		0x02			//音频帧
#define JVS_TYPE_SKIP		0x03			//SKIP帧，可忽略
#define JVS_TYPE_D			0x04			//可丢弃帧
#define JVS_TYPE_B			JVS_TYPE_D		//双向预测帧

typedef struct JDEC_param_t
{
    unsigned int	i_frame_width;			//帧宽
    unsigned int	i_frame_height;			//帧高
    unsigned int	i_y_pitch;				//Y数据的pitch值，一般为帧宽+64
    unsigned int	i_uv_pitch;				//UV数据的pitch值

    unsigned int    i_nal_type;				//帧类型
    unsigned int    i_bitstream;			//payload缓冲区的总长度

    unsigned int	i_payload;				//实际编码数据长度
    unsigned char *	p_payload;				//指向编码数据缓冲区

    unsigned char * pY;						//指向Y数据缓冲区
    unsigned char * pU;						//指向U数据缓冲区
    unsigned char * pV;						//指向V数据缓冲区

} JDEC_param_t;

// 初始化SDK资源，必须被第一个调用
JDEC04_API	void	JVD04_InitSDK();

// 释放SDK资源，必须最后被调用
JDEC04_API	void	JVD04_ReleaseSDK();

//打开一路解码器
JDEC04_API	void *	JVD04_DecodeOpen(JDEC_param_t * param);

//关闭一路解码器
JDEC04_API	void	JVD04_DecodeClose(void * h);

//解码一帧数据
JDEC04_API	int		JVD04_DecodeOneFrame(void * h);

#ifdef __cplusplus
}
#endif

#endif
