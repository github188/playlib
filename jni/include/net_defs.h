#ifndef _JVNSDKDEF_H
#define _JVNSDKDEF_H

#include <sys/stat.h>//
//#include <sys/vfs.h>//
#include <sys/wait.h>//
#ifdef  __cplusplus
extern "C" {
#endif

#define jvs_min(a,b) (a<b?a:b)
#define jvs_max(a,b) (a>b?a:b)
#define MAX_PATH 256
#define TRUE  1
#define FALSE 0
#define closesocket close
#define SD_BOTH SHUT_RDWR

#include <stdio.h>
#include <stdlib.h>

typedef struct _JVRTMP_Metadata_t {
	int nVideoWidth;
	int nVideoHeight;
	int nVideoFrameRateNum;
	int nVideoFrameRateDen;
	int nAudioDataType;
	int nAudioSampleRate;
	int nAudioSampleBits;
	int nAudioChannels;
} JVRTMP_Metadata_t;

typedef long HRESULT;
typedef struct _GUID {
	unsigned long Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
} GUID;

typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef unsigned char BYTE;

//typedef bool BOOL;

typedef struct {
	char chGroup[4];
	int nYSTNO;
	int nCardType;
	int nChannelCount;
	char chClientIP[16];
	int nClientPort;
	int nVariety;
	char chDeviceName[100];
#ifndef WIN32
	int bTimoOut;
#else
	BOOL bTimoOut;
#endif

	int nNetMod; //例如 是否具有wifi功能: nNetMod & NET_MOD_WIFI
	int nCurMod; //例如 当前使用的(wifi或有线)：nCurMod(NET_MOD_WIFI 或 NET_MOD_WIRED)

	int nPrivateSize; //自定义数据实际长度
	char chPrivateInfo[500]; //自定义数据内容
} STLANSRESULT; //局域网设备搜索结果

typedef struct STLANTOOLINFO {
	BYTE uchType; //消息类型，1来自工具的广播；2来自工具的配置；3设备回应；

	/*工具信息*/
	char chPName[256]; //用户名，用于提高IPC安全性，防止恶意配置
	char chPWord[256]; //密码，用于提高IPC安全性，防止恶意配置
	int nYSTNUM; //云视通号码，用于工具向设备发送配置
	char chCurTime[20]; //系统时间，用于工具向设备发送配置 xxxx-xx-xx xx:xx:xx
	char *pchData; //配置内容，用于工具向设备发送配置
	int nDLen; //配置内容长度，用于工具向设备发送配置

	/*应答信息*/
	int nCardType; //设备类型，用于设备应答时的附加信息
	int nDate; //出厂日期 形如 20091011
	int nSerial; //出厂序列号
	GUID guid; //唯一GUID
	char chGroup[4]; //设备便组号，用于设备应答时的附加信息

	char chIP[16];
	int nPort;

	STLANTOOLINFO() {
		uchType = 0;
		memset(chPName, 0, 256);
		memset(chPWord, 0, 256);
		nYSTNUM = 0;
		memset(chCurTime, 0, 20);
		pchData = NULL;
		nDLen = 0;

		nCardType = 0;
		memset(chGroup, 0, 4);
		nDate = 0;
		nSerial = 0;
		memset(&guid, 0, sizeof(GUID));

		memset(chIP, 0, 16);
		nPort = 0;
	}
} STLANTOOLINFO; //局域网生产工具消息内容

#define RC_DATA_SIZE	192*800
#define RC_LOADDLG 		0x05
#define RC_EX_NETWORK	0x02
#define RC_EXTEND       0x06
#define RC_EX_SENSOR    0x08
#define EX_WIFI_AP		0x03	//获取AP消息
#define EX_TRANSPARENT	0x27	// [Neo] 玩具透传协议
#define EX_NW_SUBMIT	0x09
typedef struct tagPAC {
	unsigned int nPacketType :5; //包的类型
	unsigned int nPacketCount :8; //包总数
	unsigned int nPacketID :8; //包序号
	unsigned int nPacketLen :11; //包的长度
	char acData[RC_DATA_SIZE];
} PAC, *PPAC;
typedef struct tagEXTEND {
	int nType;
	int nParam1; //存放的包的个数
	int nParam2;
	int nParam3;
	char acData[RC_DATA_SIZE - 16];
} EXTEND, *PEXTEND;

typedef struct {
	char name[32]; //
	char passwd[16]; //历史纪录的密码
	int quality; //信号强度，满值一百
	int keystat; //是否需要加密 －1不需要
	char iestat[8]; //加密的方式 支持wpa/..
} wifiap_t;

typedef struct {
	char wifiSsid[32];
	char wifiPwd[64];
	int wifiAuth;
	int wifiEncryp;
	unsigned char wifiIndex;
	unsigned char wifiChannel;
	unsigned char wifiRate;
} WIFI_INFO;

#define EX_STORAGE_REFRESH 0x01
#define EX_STORAGE_REC_ON 0x02
#define EX_STORAGE_REC_OFF 0x03
#define EX_STORAGE_OK 0x04
#define EX_STORAGE_ERR 0x05
#define EX_STORAGE_FORMAT 0x06
#define EX_STORAGE_SWITCH		0x07

#define RC_EX_FlashJpeg	    0x0a
#define RC_GETPARAM		0x02
#define RC_SETPARAM		0x03

#define EX_MD_REFRESH			0x01
#define EX_MD_SUBMIT			0x02
#define EX_MD_UPDATE			0x03
#define RC_EX_STORAGE		0x03
#define RC_EX_MD		    0x06
#define EX_NW_REFRESH		0x08
#define EX_WIFI_AP_CONFIG 0x0B	//针对新AP配置方式，获取到手机端配置的AP信息，便立即返回
#define RC_EX_FIRMUP      0x01  //检查到设备更新，更新设备命令
#define EX_UPLOAD_START   0x01  //检查到设备更新，更新设备命令
//typedef int (*FUNC_DEVICE_CALLBACK)(int nType, char* pGroup, int nYST,
//		char* pIP, int nPort);
typedef int (*FUNC_DEVICE_CALLBACK)(STLANSRESULT* result);

typedef void (*FUNC_CCONNECT_CALLBACK)(int nLocalChannel, unsigned char uchType,
		char *pMsg, int nPWData);
typedef void (*FUNC_CNORMALDATA_CALLBACK)(int nLocalChannel,
		unsigned char uchType, unsigned char *pBuffer, int nSize, int nWidth,
		int nHeight);
typedef void (*FUNC_CCHECKRESULT_CALLBACK)(int nLocalChannel,
		unsigned char *pBuffer, int nSize);
typedef void (*FUNC_CCHATDATA_CALLBACK)(int nLocalChannel,
		unsigned char uchType, unsigned char *pBuffer, int nSize);
typedef void (*FUNC_CTEXTDATA_CALLBACK)(int nLocalChannel,
		unsigned char uchType, unsigned char *pBuffer, int nSize);
typedef void (*FUNC_CDOWNLOAD_CALLBACK)(int nLocalChannel,
		unsigned char uchType, unsigned char *pBuffer, int nSize, int nFileLen);
typedef void (*FUNC_CPLAYDATA_CALLBACK)(int nLocalChannel,
		unsigned char uchType, unsigned char *pBuffer, int nSize, int nWidth,
		int nHeight, int nTotalFrame);
typedef void (*FUNC_CBUFRATE_CALLBACK)(int nLocalChannel, unsigned char uchType,
		unsigned char *pBuffer, int nSize, int nRate);
typedef void (*FUNC_CLANSDATA_CALLBACK)(STLANSRESULT stLSResult);
typedef void (*FUNC_CBCDATA_CALLBACK)(int nBCID, unsigned char *pBuffer,
		int nSize, char chIP[16], bool bTimeOut);
typedef int (*FUNC_CLANTDATA_CALLBACK)(STLANTOOLINFO *pLANTData);

//手机端从内存卡中获取数据回调
typedef void (*FUNC_GETDATA_CALLBACK)(unsigned char *chGroup,
		unsigned char *pBuffer, int *nSize);
typedef void (*FUNC_WRITE_CALLBACK)(unsigned char *chGroup,
		unsigned char *pBuffer, int nSize);
typedef void (*FUNC_CRTMP_CONNECT_CALLBACK)(int nLocalChannel,
		unsigned char uchType, char *pMsg, int nPWData); //1 成功 2 失败 3 断开 4 异常断开
typedef void (*FUNC_CRTMP_NORMALDATA_CALLBACK)(int nLocalChannel,
		unsigned char uchType, unsigned char *pBuffer, int nSize,
		int nTimestamp);
typedef void (*FUNC_CBCSELFDATA_CALLBACK)(unsigned char *pBuffer, int nSize, char chIP[16], int nPort);

//////////////////////////////////////////////////////////////////////////
/*实时监控数据类型*/
#define JVN_DATA_I           0x01//视频I帧
#define JVN_DATA_B           0x02//视频B帧
#define JVN_DATA_P           0x03//视频P帧
#define JVN_DATA_A           0x04//音频
#define JVN_DATA_S           0x05//帧尺寸
#define JVN_DATA_OK          0x06//视频帧收到确认
#define JVN_DATA_DANDP       0x07//下载或回放收到确认
#define JVN_DATA_O           0x08//其他自定义数据
#define JVN_DATA_SKIP        0x09//视频S帧
#define JVN_DATA_SPEED		 0x64//主控码率
/*请求类型*/
#define JVN_REQ_CHECK        0x10//请求录像检索
#define JVN_REQ_DOWNLOAD     0x20//请求录像下载
#define JVN_REQ_PLAY         0x30//请求远程回放
#define JVN_REQ_CHAT         0x40//请求语音聊天
#define JVN_REQ_TEXT         0x50//请求文本聊天
#define JVN_REQ_CHECKPASS    0x71//请求身份验证
#define JVN_REQ_RECHECK      0x13//预验证
#define JVN_REQ_RATE		 0x63//分控请求码率
/*请求返回结果类型*/
#define JVN_RSP_CHECKDATA    0x11//检索结果
#define JVN_RSP_CHECKOVER    0x12//检索完成
#define JVN_RSP_DOWNLOADDATA 0x21//下载数据
#define JVN_RSP_DOWNLOADOVER 0x22//下载数据完成
#define JVN_RSP_DOWNLOADE    0x23//下载数据失败
#define JVN_RSP_PLAYDATA     0x31//回放数据
#define JVN_RSP_PLAYOVER     0x32//回放完成
#define JVN_RSP_PLAYE        0x39//回放失败
#define JVN_RSP_CHATDATA     0x41//语音数据
#define JVN_RSP_CHATACCEPT   0x42//同意语音请求
#define JVN_RSP_TEXTDATA     0x51//文本数据
#define JVN_RSP_TEXTACCEPT   0x52//同意文本请求
#define JVN_RSP_CHECKPASS    0x72//身份验证
#define JVN_RSP_CHECKPASST   0x72//身份验证成功 为TCP保留
#define JVN_RSP_CHECKPASSF   0x73//身份验证失败 为TCP保留
#define JVN_RSP_NOSERVER     0x74//无该通道服务
#define JVN_RSP_INVALIDTYPE  0x7A//连接类型无效
#define JVN_RSP_OVERLIMIT    0x7B//连接超过主控允许的最大数目
#define JVN_RSP_DLTIMEOUT    0x76//下载超时
#define JVN_RSP_PLTIMEOUT    0x77//回放超时
#define JVN_RSP_RECHECK      0x14//预验证
#define JVN_RSP_OLD          0x15//旧版主控回复
/*命令类型*/
#define JVN_CMD_DOWNLOADSTOP 0x24//停止下载数据
#define JVN_CMD_PLAYUP       0x33//快进
#define JVN_CMD_PLAYDOWN     0x34//慢放
#define JVN_CMD_PLAYDEF      0x35//原速播放
#define JVN_CMD_PLAYSTOP     0x36//停止播放
#define JVN_CMD_PLAYPAUSE    0x37//暂停播放
#define JVN_CMD_PLAYGOON     0x38//继续播放
#define JVN_CMD_CHATSTOP     0x43//停止语音聊天
#define JVN_CMD_PLAYSEEK     0x44//播放定位		按帧定位 需要参数 帧数(1~最大帧)
#define JVN_CMD_TEXTSTOP     0x53//停止文本聊天
#define JVN_CMD_YTCTRL       0x60//云台控制
#define JVN_CMD_VIDEO        0x70//实时监控
#define JVN_CMD_VIDEOPAUSE   0x75//暂停实时监控
#define JVN_CMD_TRYTOUCH     0x78//打洞包
#define JVN_CMD_FRAMETIME    0x79//帧发送时间间隔(单位ms)
#define JVN_CMD_DISCONN      0x80//断开连接
#define JVN_CMD_MOTYPE		 0x72//UDP手机类型 注：此值与下面一类型定义值相同，本应避免，暂保持这样
#define JVN_CMD_ONLYI        0x61//该通道只发关键帧
#define JVN_CMD_FULL         0x62//该通道恢复满帧
#define JVN_CMD_ALLAUDIO	 0x65//音频全转发
/*与云视通服务器的交互消息*/
#define JVN_CMD_TOUCH        0x81//探测包
#define JVN_REQ_ACTIVEYSTNO  0x82//主控请求激活YST号码
#define JVN_RSP_YSTNO        0x82//服务器返回YST号码
#define JVN_REQ_ONLINE       0x83//主控请求上线
#define JVN_RSP_ONLINE       0x84//服务器返回上线令牌
#define JVN_CMD_ONLINE       0x84//主控地址更新
#define JVN_CMD_OFFLINE      0x85//主控下线
#define JVN_CMD_KEEP         0x86//主控保活
#define JVN_REQ_CONNA        0x87//分控请求主控地址 udp时停用
#define JVN_RSP_CONNA        0x87//服务器向分控返回主控地址
#define JVN_CMD_CONNB        0x87//服务器命令主控向分控穿透
#define JVN_RSP_CONNAF       0x88//服务器向分控返回 主控未上线
#define JVN_CMD_RELOGIN		 0x89//通知主控重新登陆
#define JVN_CMD_CLEAR		 0x8A//通知主控下线并清除网络信息包括云视通号码
#define JVN_CMD_REGCARD		 0x8B//主控注册板卡信息到服务器
#define JVN_CMD_CONNB2				0xB0        //分控请求连接主控 带参数
#define JVN_CMD_ONLINES2     0x8C//服务器命令主控向转发服务器上线/主控向转发服务器上线(停用)
#define JVN_CMD_CONNS2       0x8D//服务器命令分控向转发服务器发起连接
#define JVN_REQ_S2           0x8E//分控向服务器请求转发
#define JVN_TDATA_CONN       0x8F//分控向转发服务器发起连接(停用)
#define JVN_TDATA_NORMAL     0x90//分控/主控向转发服务器发送普通数据
#define JVN_TDATA_AOL        0x8E//主控向转发服务器上线(新)
#define JVN_TDATA_BCON       0x8D//分控向转发服务器发起连接(新)
#define JVN_CMD_CARDCHECK    0x91//板卡验证
#define JVN_CMD_ONLINEEX     0x92//主控地址更新扩展
#define JVN_CMD_TCPONLINES2  0x93//服务器命令主控TCP向转发服务器上线
#define JVN_CMD_CHANNELCOUNT 0x97//分控查询主控所具有的通道数目
#define JVN_CMD_ONLINE1EX    0x9C//主控UDP1上线扩展(新上线)
#define JVN_REQ_MOS2		 0x9D//3G手机向服务器请求转发
#define YST_A_NEW_ADDRESS    0x100//分控查询NAT使用 主控返回服务器新的NAT
//---------------------------------------v2.0.0.1
#define JVN_CMD_BM           0x94//BM广播消息 A->B
#define JVN_CMD_TCP          0x95//节点间TCP连接 B->B
#define JVN_CMD_KEEPLIVE     0x96//分控和主控间的心跳数据
#define JVN_CMD_PLIST        0x98//组成员列表       A->B B->A
#define JVN_RSP_BMDBUSY      0x99//回复某数据片现在忙碌 B->B A->B
#define JVN_CMD_CMD          0x9A//主控要求分控执行特殊操作 A->B
#define JVN_CMD_ADDR         0x9B//分控节点内外网地址 A->B
#define JVN_REQ_BMD          0x9D//请求某数据片 B->A B->B
#define JVN_RSP_BMD          0x9E//回复某数据片 A->B B->B
#define JVN_CMD_LADDR        0x9F//分控上传自己的内网地址
#define JVN_RSP_BMDNULL      0xA0//回复某数据片失败 A->B B->B
#define JVN_CMD_TRY          0xA1//A命令B 向伙伴打洞
#define JVN_DATA_RATE        0xA2//多播经小助手时的缓冲进度
//---------------------------------------v2.0.0.1

/*后续扩展*/
#define JVN_CMD_YSTCHECK     0xAC//查询及返回某号码是否在线以及号码主控SDK版本
#define JVN_REQ_EXCONNA      0xAD//分控请求主控地址
#define JVN_CMD_KEEPEX       0xAE//主控心跳扩展(带编组+编号+时间戳)
#define JVN_CMD_OLCOUNT      0xAF//获取当前服务器总在线数目
/*客户版流媒体服务器相关*/
#define JVN_REQ_CONNSTREAM_SVR    0xD0//请求连接流媒体服务器主控
#define JVN_REQ_CONNSTREAM_CLT    0xD1//请求连接流媒体服务器分控
#define JVN_RSP_CONNSTREAM_SVR    0xD2//回复
#define JVN_RSP_CONNSTREAM_CLT    0xD3
#define JVN_NOTIFY_ID			  0xD4
#define JVN_RSP_ID				  0xD5
#define JVN_CMD_CONNSSERVER		  0xD6
#define JVN_RSP_NEWCLIENT         0xD9

/*收费工具相关*/
#define TOOL_USER_LOGIN		    0xD1//收费工具登录
#define TOOL_USER_CHANGE		0xD2//收费工具修改密码
#define TOOL_VIP_SERCH		    0xD3//收费工具查询VIP
#define TOOL_VIP_SET			0xD4//收费工具设置VIP
#define TOOL_VIP_RESET		    0xD5//收费工具重新设置，对于设置不成功时使用
#define A_VIP_README    	    0xE0//主控查询如何开通VIP
#define A_VIP_SET			    0xE1//主控申请开通vip试用
#define A_VIP_SERCH  			0xE2//主控查询vip
/*局域网设备搜索*/
#define JVN_REQ_LANSERCH  0x01//局域网设备搜索命令
#define JVN_CMD_LANSALL   1//局域网搜索所有中维设备
#define JVN_CMD_LANSYST   2//局域网搜索指定云视通号码的设备
#define JVN_CMD_LANSTYPE  3//局域网搜索指定卡系的设备
#define JVN_CMD_LANSNAME  4//局域网搜索指定别名的设备
#define JVN_RSP_LANSERCH  0x02//局域网设备搜索响应命令
#define JVN_DEVICENAMELEN  100//设备别名长度限制
/*局域网广播*/
#define JVN_REQ_BC  0x03//局域网广播命令
#define JVN_RSP_BC  0x04//局域网广播响应命令
/*局域网管理工具*/
#define JVN_REQ_TOOL 0x05//工具消息
#define JVN_RSP_TOOL 0x06//设备响应
/*主分控端设置*/
#define JVN_MAXREQ        500     //分控请求队列的最大容量
#define JVN_MAXREQRUN     20      //允许同时处理的分控请求数
#define JVNC_DATABUFLEN   512000//800K//分控接收数据缓冲大小150*1024
#define JVNS_DATABUFLEN   150*1024//主控接收数据缓冲大小
#define JVN_RESENDFRAMEB  1       //恢复发送B帧的条件
#define JVN_NOTSENDFRAMEB ((float)1/(float)2)  //不发送B帧的条件
#define JVN_ASPACKDEFLEN  1024    //与服务器间数据包默认最大长度
#define JVN_BAPACKDEFLEN  30*1024 //分控向主控发送数据包默认最大长度
#define JVN_ABCHECKPLEN   14      //与分控间文件检索参数数据包长度
#define JVNC_PTINFO_LEN   102400

#define JVN_ABFRAMERET    25      //帧序列中每个多少帧一个回复
#define JVNC_ABFRAMERET   15      //帧序列中每个多少帧一个回复
#define JVN_ABFRAMERET_MO 35      //帧序列中每个多少帧一个回复
#define JVN_RELOGINLIMIT  30   //掉线后重新上线失败次数上限
#define JVN_RUNEVENTLEN   2048 //日志文本长度
#define JVN_RUNFILELEN    4000*1024//日志文件大小上限
#define LINUX_THREAD_STACK_SIZE 512*1024 //linux环境下的线程堆栈上限
#define JVN_WEBSITE1      "www.jovetech.com"//服务网站1
#define JVN_WEBSITE2      "www.afdvr.com"//服务网站2
#define JVN_WEBFOLDER     "/down/YST/"//"/down/ser703/oem800"//服务器列表文件夹
//#define JVN_WEBFILE       "/yst.txt"//"/ipsecu.txt\n"//服务器列表文件
//#define JVN_NEWWEBFILE    "/ystnew.txt"//服务器列表文件
#define JVN_AGENTINFO     "User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)\r\n"

#define JVN_WEBSITE_POS       "www.afdvr.com"//地区定位服务网站
#define JVN_WEB_POSREQ         "/GetPos/"
#define JVN_POS_STR            "/yst_%s.txt"
#define JVN_YSTLIST_ALL        "/yst_all.txt"      //所有服务器清单
#define JVN_YSTLIST_HOME       "/yst_home.txt"      //默认服务器清单(国内所有用户+有需要的国外用户)
//地区清单：
#define JVN_YSTLIST_USA        "/yst_usa.txt"       //美国服务器(美国用户) ***********有效
#define JVN_YSTLIST_INDIA      "/yst_india.txt"     //印度服务器(印度用户)
#define JVN_YSTLIST_SINGAPORE  "/yst_singapore.txt" //新加坡服务器(新加坡周边用户)
#define JVN_YSTLIST_SOUTHCHINA "/yst_southchina.txt"//中国南部服务器(中国南方用户)
#define JVN_YSTLIST_WESTCHINA  "/yst_westchina.txt" //中国西部服务器(中国西部用户)
#define JVN_YSTLIST_MIDDLEEAST "/yst_middleeast.txt"//中东服务器(中东用户)
#define JVN_YSTLIST_AP         "/yst_ap.txt"        //亚太地区服务器(亚太用户)
#define JVN_ALLSERVER     0//所有服务
#define JVN_ONLYNET       1//只局域网服务
#define JVN_ONLYYST       2//只云视通号码服务
#define JVN_NOTURN        0//云视通方式时禁用转发
#define JVN_TRYTURN       1//云视通方式时启用转发
#define JVN_ONLYTURN      2//云视通方式时仅用转发
#define JVN_CONNTYPE_LOCAL  1//局域网连接
#define JVN_CONNTYPE_P2P    2//P2P穿透连接
#define JVN_CONNTYPE_TURN   3//转发
#define JVN_LANGUAGE_ENGLISH  1
#define JVN_LANGUAGE_CHINESE  2

#define JVN_TRANS_ONLYI   1//关键帧转发
#define JVN_TRANS_ALL     2//完整转发/完整传输
#define TYPE_PC_UDP      1//连接类型 UDP 支持UDP收发完整数据
#define TYPE_PC_TCP      2//连接类型 TCP 支持TCP收发完整数据
#define TYPE_MO_TCP      3//连接类型 TCP 支持TCP收发简单数据,普通视频帧等不再发送，只能采用专用接口收发数据(适用于手机监控)
#define TYPE_MO_UDP      4//连接类型 UDP
#define TYPE_3GMO_UDP    5//连接类型 3GUDP
#define PARTNER_ADDR     1//需要对方地址
#define PARTNER_NATTRY   2//需要对方打洞
#define OLD_RSP_IMOLD     1//告知分控我是旧协议版本
/*特殊命令类型*/
#define CMD_TYPE_CLEARBUFFER    1//主控和分控清空缓存，重新进行缓存
/*云台控制类型*/
#define JVN_YTCTRL_U      1//上
#define JVN_YTCTRL_D      2//下
#define JVN_YTCTRL_L      3//左
#define JVN_YTCTRL_R      4//右
#define JVN_YTCTRL_A      5//自动
#define JVN_YTCTRL_GQD    6//光圈大
#define JVN_YTCTRL_GQX    7//光圈小
#define JVN_YTCTRL_BJD    8//变焦大
#define JVN_YTCTRL_BJX    9//变焦小
#define JVN_YTCTRL_BBD    10//变倍大
#define JVN_YTCTRL_BBX    11//变倍小
#define JVN_YTCTRL_UT     21//上停止
#define JVN_YTCTRL_DT     22//下停止
#define JVN_YTCTRL_LT     23//左停止
#define JVN_YTCTRL_RT     24//右停止
#define JVN_YTCTRL_AT     25//自动停止
#define JVN_YTCTRL_GQDT   26//光圈大停止
#define JVN_YTCTRL_GQXT   27//光圈小停止
#define JVN_YTCTRL_BJDT   28//变焦大停止
#define JVN_YTCTRL_BJXT   29//变焦小停止
#define JVN_YTCTRL_BBDT   30//变倍大停止
#define JVN_YTCTRL_BBXT   31//变倍小停止
#define JVN_YTCTRL_FZ1    32//辅助1
#define JVN_YTCTRL_FZ1T   33//辅助1停止
#define JVN_YTCTRL_FZ2    34//辅助2
#define JVN_YTCTRL_FZ2T   35//辅助2停止
#define JVN_YTCTRL_FZ3    36//辅助3
#define JVN_YTCTRL_FZ3T   37//辅助3停止
#define JVN_YTCTRL_FZ4    38//辅助4
#define JVN_YTCTRL_FZ4T   39//辅助4停止
#define JVN_YTCTRL_RECSTART  41//远程录像开始
#define JVN_YTCTRL_RECSTOP	 42//远程录像开始
/*远程控制指令(主分控应用层约定)*/
#define RC_DISCOVER		0x01
#define RC_GETPARAM		0x02
#define RC_SETPARAM		0x03
#define RC_VERITY		0x04
#define RC_SNAPSLIST	0x05
#define RC_GETFILE		0x06
#define RC_USERLIST		0x07
#define RC_PRODUCTREG	0X08
#define RC_GETSYSTIME	0x09
#define RC_SETSYSTIME	0x0a
#define RC_DEVRESTORE	0x0b
#define RC_SETPARAMOK	0x0c
#define RC_DVRBUSY		0X0d
#define RC_GETDEVLOG	0x0e
#define RC_DISCOVER2	0x0f	//zwq20111206,csst云视通号码直接登录，局域网广播搜索
#define RC_GPIN_ADD     0x10
#define RC_GPIN_SET     0x11
#define RC_GPIN_SELECT	0x12
#define RC_GPIN_DEL		0x13

#define JVN_VC_BrightUp			0x10 //视频调节
#define JVN_VC_BrightDown		0x11
#define JVN_VC_ContrastUp		0x12
#define JVN_VC_ContrastDown		0x13
#define JVN_VC_SaturationUp		0x14
#define JVN_VC_SaturationDown	0x15
#define JVN_VC_HueUp			0x16
#define JVN_VC_HueDown			0x17
#define JVN_VC_SharpnessUp		0x18
#define JVN_VC_SharpnessDown	0x19
#define JVN_VC_PRESENT          0x20 //预置位调用
#define JVN_DSC_CARD  		0x0453564A// 板卡解码类型04版解码器
#define JVN_DSC_9800CARD 	0x0953564A// 9800板卡解码类型04版解码器
#define JVN_DSC_960CARD  	0x0A53564A// 新版标准解码器05版解码器
#define JVN_DSC_DVR  		0x0553563A// DVR的解码器类型
#define JVN_DSC_952CARD  	0x0153564A// 952
#define JVN_DVR_8004CARD  	0x0253564A// 宝宝在线
#define DVR8004_STARTCODE  	0x0553564A//
#define JVSC950_STARTCODE  	0x0653564A
#define JVSC951_STARTCODE  	0x0753564A//
#define IPC3507_STARTCODE  	0x1053564A//
#define IPC_DEC_STARTCODE  	0x1153564A//
#define JVN_NVR_STARTCODE  	0x2053564A//nvr
/************主控回调函数参数类型***************/
/*FUNC_SCONNECT_CALLBACK*/
#define JVN_SCONNECTTYPE_PCCONNOK  0X01//有PC分控连接成功
#define JVN_SCONNECTTYPE_DISCONNOK 0X02//有分控断开连接成功
#define JVN_SCONNECTTYPE_DISCONNE  0X03//连接异常断开
#define JVN_SCONNECTTYPE_MOCONNOK  0X04//有移动设备分控连接成功
/*FUNC_SONLINE_CALLBACK*/
#define JVN_SONLINETYPE_ONLINE     0x01//上线
#define JVN_SONLINETYPE_OFFLINE    0x02//下线
#define JVN_SONLINETYPE_CLEAR      0x03//YST号码无效，应清空重新申请
/************分控回调函数参数类型***************/
/*FUNC_CCONNECT_CALLBACK*/
#define JVN_CCONNECTTYPE_CONNOK    0X01//连接成功
#define JVN_CCONNECTTYPE_DISOK     0X02//断开连接成功
#define JVN_CCONNECTTYPE_RECONN    0X03//不必重复连接
#define JVN_CCONNECTTYPE_CONNERR   0X04//连接失败
#define JVN_CCONNECTTYPE_NOCONN    0X05//没连接
#define JVN_CCONNECTTYPE_DISCONNE  0X06//连接异常断开
#define JVN_CCONNECTTYPE_SSTOP     0X07//服务停止，连接断开
#define JVN_CCONNECTTYPE_DISF      0x08//断开连接失败
#define JVN_CCONNECTTYPE_IOK      0x10//deng dai i zhen
/*返回值*/
#define JVN_RETURNOK    0//成功
#define JVN_PARAERROR   1//参数错误
#define JVN_RETURNERROR 2//失败
#define JVN_NOMEMERROR  3//内存或硬盘空间不足
//增加IP段，搜索局域网设备时候用。
typedef struct {
	char startip[16]; //起始IP
	char endip[16]; //终止IP
} IPSECTION;

typedef struct _JVS_FILE_HEADER_EX {
	//老文件头，为兼容以前版本分控，保证其能正常预览
	uint8_t ucOldHeader[32]; //JVS_FILE_HEADER	oldHeader; //此处定义不可直接定义为JVS_FILE_HEADER类型，否则会有结构体成员对齐问题

	//结构体信息
	uint8_t ucHeader[3]; //结构体识别码，设置为‘J','F','H'
	uint8_t ucVersion; //结构体版本号，当前版本为1

	//设备相关
	uint16_t wDeviceType; //设备类型

	//视频部分
	uint16_t wVideoCodecID; //视频编码类型
	uint16_t wVideoDataType; //数据类型
	uint16_t wVideoFormat; //视频模式
	uint16_t wVideoWidth; //视频宽
	uint16_t wVideoHeight; //视频高
	uint16_t wFrameRateNum; //帧率分子
	uint16_t wFrameRateDen; //帧率分母

	//音频部分
	uint16_t wAudioCodecID; //音频编码格式
	uint16_t wAudioSampleRate; //音频采样率
	uint16_t wAudioChannels; //音频声道数
	uint16_t wAudioBits; //音频采样位数

	//录像相关
	uint32_t dwRecFileTotalFrames; //录像总帧数
	uint16_t wRecFileType; //录像类型

	//保留位
	uint8_t ucReserved[30]; //请全部置0

} JVS_FILE_HEADER_EX, *PJVS_FILE_HEADER_EX;

//云视通号码基本信息，用于初始化小助手的虚连接
typedef struct STBASEYSTNO {
	char chGroup[4];
	int nYSTNO;
	int nChannel;
	char chPName[MAX_PATH];
	char chPWord[MAX_PATH];
	//连接状态 设置时=0，查询时表示状态 0 未连接 1 内网 2 转发 3外网
	int nConnectStatus;
} STBASEYSTNO;

#ifdef  __cplusplus
}
#endif

#endif
