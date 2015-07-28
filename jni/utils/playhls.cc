#include <json.h>
#include <time.h>
#include "playhls.h"
#include "JHLSClient.h"
#include "jhlstype.h"
#include "M3U8Parser.h"
#include "TSParser.h"
#include <sstream>
#include "defines.h"
#include "utils/commons.h"
#include "utils/threads.h"
#include "utils/callbacks.h"
#include "utils/char_conv.h"
#include "auth_base64/oss_auth.h"
#include <json.h>
#include <net_defs.h>
#include <sys/select.h>
#include <fcntl.h>

#define HLS_QUEUE_LEFT_FLAG 15

struct downObj
{
    FILE *fp;
    char *psign;
};

#define file_json_len_key  "file_len"

using namespace jhls;
using namespace Json;

//char* access_host = "oss-cn-hangzhou.aliyuncs.com";
//char* access_id = "4fZazqCFmQTbbmcw";
//char* access_bucket = "jovetech";
//char* access_key = "sBT6DgBMdCHnnaYdvK0o6O3zaN06sW";

char access_host[1024] = {0};
char access_id[1024] = {0};
char access_bucket[1024] = {0};
char access_key[1024] = {0};

char g_headPathStr[1024] = {0};
char g_headUriStr[1024] = {0};
char g_fileName[1024] = {0};
char g_url[1024] = {0};

bool is_decoder_init = false;
bool is_hls_palying_over = false;
bool is_current_client_parsing_over = false;

bool downloadFlag = false;
int M3U8Flag = 0;
int downloadRet = 0;

/**
 * 毫秒级的睡觉
 *
 */
void hls_msleep(int millis) {
	if (millis > 0) {
		struct timeval tt;
		tt.tv_sec = millis / 1000;
		tt.tv_usec = (millis % 1000) * 1000;
		select(0, NULL, NULL, NULL, &tt);
	}
}


void callBackToJava(int what, int window, int type, string json)
{
	jboolean needDetach = JNI_FALSE;
	JNIEnv* env = genAttachedEnv(g_jvm, JNI_VERSION_1_6, &needDetach);
	if (NULL != env && NULL != g_handle && NULL != g_notifyid) {

		jstring jmsg = NULL;
		if (json.length() > 0) {
			jmsg = env->NewStringUTF(json.c_str());
		}

		env->CallVoidMethod(g_handle, g_notifyid, (jint)what,
				(jint) window, (jint) type, jmsg);

		if (JNI_TRUE == needDetach) {
			g_jvm->DetachCurrentThread();
		}
	}
}

void hlsPlayerInit(JHLSVideoStreamType_e vtype, JHLSAudioStreamType_e atype)
{
	int window = array2Window(0);
	LOGW( "%s [%p]: E, window = %d", LOCATE_PT, window);
	if (window >= 0) {
		player_suit* player = g_player[0];

		if (NULL != player) {
			offer_video_frame(player, NULL, 0, -1);
			player->thumb_name = NULL;
			//player->is_play_audio = true;
			video_meta* meta = player->vm_normal;
			meta->video_type =  (vtype == JHLS_VIDEO_STREAMTYPE_H264?kVTypeH264:kVTypeH265);
			meta->is_wait_by_ts = true;
			meta->is_hls_player_over = false;
			meta->video_width = 1280;
			meta->video_height = 720;
//			meta->video_frame_min_count=10;
//			meta->video_frame_rate = 10;
//			meta->video_frame_period = 100;
			meta->video_max_frame_count = meta->video_frame_rate * MAX_DELAY_TIMES;
			if(atype == JHLS_AUDIO_STREAMTYPE_G711A)
				meta->audio_type = JAD_CODEC_ALAW;
			if(atype == JHLS_AUDIO_STREAMTYPE_G711U)
				meta->audio_type = JAD_CODEC_ULAW;
			meta->audio_bit = 16;
			meta->audio_channel = 1;
			meta->audio_sample_rate = 8000;
			meta->audio_enc_type = meta->audio_type;
			player->is_connected = true;
			player->is_audio_working = false;
			if (player->is_connected && false == player->is_audio_working) {
				pthread_t pt;
				pthread_create(&pt, NULL, onPlayAudio, (void*) 0);
			}
			is_decoder_init = true;
			//传入O帧
			BYTE* o = (BYTE*) malloc(DUMMY_FRAME_SIZE);
			memset(o, 0, DUMMY_FRAME_SIZE);
			o[0] = DUMMY_FRAME_0_O;

			offer_video_frame(player, o, DUMMY_FRAME_SIZE, 0X08);
			int window = array2Window(0);
			callBackToJava(CALL_NORMAL_DATA, window, 0, "{\"msg\":\"O Frame Coming\"}");
		}
	}
}

void offerHlsPlayOver()
{
	//传入结束帧
	player_suit* player = g_player[0];
	if(NULL != player){
		video_meta* meta = player->vm_normal;
		meta->is_hls_player_over = true;
		BYTE* o = (BYTE*) malloc(DUMMY_FRAME_SIZE);
		memset(o, 0, DUMMY_FRAME_SIZE);
		o[0] = DUMMY_FRAME_HLS_END;
		int i = 0;
		for(i;i<8;i++){
			offer_video_frame(player, o, DUMMY_FRAME_SIZE, 0x08);
		}
	}
}
int authurl(char *psign, const char *fname)
{
	char now[64] = {0};
	time_t		nowt;

	time(&nowt);
	nowt += 36000;
	sprintf(now,"%ld",nowt);
	gen_authorization(1, psign, access_host, access_id, access_key, access_bucket, "GET", now, fname);
	return 0;
}

int get_file_size(const char *filename)
{
	int size;
	FILE* fp = fopen( filename, "rb" );
	if(fp==NULL)
	{
		printf("ERROR: Open file %s failed.\n", filename);
		return 0;
	}
	fseek( fp, SEEK_SET, SEEK_END );
	size=ftell(fp);
	fclose(fp);
	LOGI("file size:%d", size);
	return size;
}

//下载线程
void* downloadThread(void *param)
{
	struct downObj *down_obj=(struct downObj*)param;
	downloadRet = downloadFile(down_obj->fp, down_obj->psign);
	downloadFlag = true;
	pthread_exit(NULL);
}

class MyDownload : public CDownload
{
public:
	MyDownload(){m_fp = NULL;}
	~MyDownload(){}

	virtual int open(const char *fname)
	{
		//下载文件
		LOGI("open fname:%s", fname);
		if(is_hls_palying_over)
			return -1;
		char path[1024] = {0};
		char headerUri[1024] = {0};
		memset(path,0,1024);
		memset(headerUri,0, 1024);
		strcpy(path, g_headPathStr);
		strcpy(headerUri, g_headUriStr);
		const char *realUri;
		if(strstr(fname,".ts")){
			//ts文件
			LOGE("是ts文件");
			strcat(path, fname);
			strcat(headerUri, fname);
			realUri = headerUri;
		}else if(strstr(fname,".m3u8")){
			LOGE("是m3u8文件");
			M3U8Flag++;
			strcat(path, "1.m3u8");
			realUri = fname;
		}
		//如果是ts文件并且存在，则不下载
		if(strstr(fname,".ts") &&(access(path,F_OK))!=-1){
			LOGI("文件存在，不下载，直接打开");
		}else{
			//第二次M3U8需要等播放线程队列剩余15帧后。
			if(M3U8Flag == 2){
				player_suit* player = g_player[0];
				int queue_left = 0;
				while(true){
					if(is_hls_palying_over)
						return -1;
					queue_left = get_video_left(player);
					LOGI("queue_left = %d", queue_left);
					if(queue_left > HLS_QUEUE_LEFT_FLAG){
						LOGI("queue left 大于30 sleep 500");
						hls_msleep(200);
					}else
						break;
				}
			}
			char sign[1024] = {0};
			authurl(sign, realUri);
			LOGI("sign :%s", sign);
			FILE *fp = fopen(path, "wb");
			if (fp == NULL) {
				LOGE("could not open %s\n", path);
				return -1;
			}
			downloadFlag = false;
			struct downObj obj;
			obj.fp=fp;
			obj.psign=sign;
			int ret = 0;
			pthread_t	thread_id;
			ret = pthread_create(&thread_id, NULL, downloadThread, (void *)&obj);
			//pthread_join(thread_id, NULL);
			while(!is_hls_palying_over){
				if(downloadFlag)
					break;
				hls_msleep(500);
			}
			fclose(fp);

			if(downloadRet != 200){
				LOGE("下载失败\n");
				remove(path);
				char json[1024] = {0};
				sprintf(json, "{\"download_msg\":%d}", ret);
				int window = array2Window(0);
				callBackToJava(CALL_CONNECT_CHANGE, window, 0x04, json);
				return -1;
			}else{
				LOGE("下载成功\n");
				int size = get_file_size(path);
				char json[1024] = {0};
				sprintf(json, "{\"file_len\":%d}", size);
				LOGE("json :%s", json);
				int window = array2Window(0);
				callBackToJava(CALL_HLS_DOWNLOAD_CALLBACK, window, 0x00, json);
			}
		}

		if(is_hls_palying_over){
			remove(path);
			return -1;
		}

		m_fp = fopen(path, "rb");
		if (!m_fp)
		{
			LOGE("Failed fopen file: %s\n", fname);
			return -1;
		}else{
			LOGI("打开文件成功 path:%s", path);
		}
		return 0;
	}
	virtual int read(unsigned char *data, int len)
	{
		if(is_hls_palying_over)
			return -1;
		return fread(data, 1, len, m_fp);;
	}
	virtual int close()
	{
		fclose(m_fp);
		return 0;
	}

	virtual int seek(int percent)
	{
		return 0;
	}
	virtual int getPos()
	{
		return 0;// 返回已读文件占总文件大小的百分比。取值 [0,100]
	}

	virtual void OnFrameReceived(JHLSFrameType_e type, long long timeStamp, const unsigned char *frame, int len)
	{
		if(!is_hls_palying_over){
			//LOGI("data received type: %d, len: %d, timeStamp: %lld\n", type, len, timeStamp);
			player_suit* player = g_player[0];
			if(type == JHLS_FRAME_TYPE_AUDIO){
				if(player->is_play_audio){
					if(NULL != frame)
						offer_audio_frame(player, (BYTE*)frame, len, false, false, timeStamp);
				}
			}
			else if(type == JHLS_FRAME_TYPE_VIDEO_I){
				offer_video_frame(player, (BYTE*)frame, len, JVN_DATA_I, false, timeStamp);

			}else if(type == JHLS_FRAME_TYPE_VIDEO_P || type == JHLS_FRAME_TYPE_VIDEO_B){
				offer_video_frame(player, (BYTE*)frame, len, JVN_DATA_P, false, timeStamp);

			}
		}
	}

	virtual void OnStreamType(JHLSVideoStreamType_e vtype, JHLSAudioStreamType_e atype)
	{
		LOGE("frame type: %d, %d\n", vtype, atype);
		//初始化播放器的一些列参数
		if(!is_hls_palying_over && !is_decoder_init){
			hlsPlayerInit(vtype, atype);
		}
	}

private:
	FILE *m_fp;
};

MyDownload download;
CJHLSClient *client = NULL;

//解析线程
void* clientParingThread(void *param){

	if(client == NULL)
		return NULL;

	while(true){
		is_current_client_parsing_over = false;
		bool result = client->parsing() ;
		is_current_client_parsing_over = true;
		if(result==false){
			break;
		}
		if(is_hls_palying_over)
		{
			break;
		}
	}
	LOGI("parsing out---->");
	offerHlsPlayOver();
	return	 NULL;
}

void playerInit(char* m3u8Path, char*url, char* filename, char* authJson)
{
	Reader reader;
	Value root;
	//string s(authJson);
	LOGI("hls player init authJson:%s", authJson);
	int access_cstype = -1;
	if (reader.parse(authJson, root)) {
		string access_host_s = root["cshost"].asString();
		string access_id_s = root["csid"].asString();
		string access_key_s = root["cskey"].asString();
		string access_bucket_s = root["csspace"].asString();

		memset(access_host,0,1024);
		memset(access_id,0,1024);
		memset(access_key,0,1024);
		memset(access_bucket,0,1024);

		snprintf(access_host, sizeof(access_host), "%s", access_host_s.c_str());
		snprintf(access_id, sizeof(access_id), "%s", access_id_s.c_str());
		snprintf(access_key, sizeof(access_key), "%s", access_key_s.c_str());
		snprintf(access_bucket, sizeof(access_bucket), "%s",access_bucket_s.c_str());
		LOGI("access_host1:%s", access_host);
		LOGI("access_id1:%s", access_id);
		LOGI("access_key1:%s", access_key);
		LOGI("access_bucket1:%s", access_bucket);
	 }
	else{
		LOGE("eader.parse reqjson failed");
		return;
	}
	LOGI("m3u8Path:%s, url:%s, filename :%s",m3u8Path,url, filename);

	is_hls_palying_over = false;
	is_decoder_init = false;
	M3U8Flag = 0;
	memset(g_headPathStr,0,1024);
	memset(g_headUriStr,0,1024);
	memset(g_fileName,0,1024);
	memset(g_url,0,1024);

	snprintf(g_headPathStr, sizeof(g_headPathStr), "%s/", m3u8Path);
	snprintf(g_headUriStr, sizeof(g_headUriStr), "%s/", url);
	snprintf(g_fileName, sizeof(g_fileName), "%s", filename);

	snprintf(g_url,sizeof(g_url), "%s/%s", url, filename);
	LOGI("g_url :%s", g_url);
	//下载m3u8
	client = new CJHLSClient(download, g_url, 500*1024);
	pthread_t pt;
	pthread_create(&pt, NULL, clientParingThread, NULL);
}



void  playerClose(int index)
{
	LOGI("播放结束");
	//播放结束
	is_hls_palying_over = true;
	player_suit* player = g_player[index];
	if(NULL != player){
		video_meta* meta = player->vm_normal;
		meta->is_hls_player_over = true;
		player->is_connecting = false;
		player->is_connected = false;

	}
	offer_video_frame(player, NULL, 0, -1);
	while(true){
		if(!is_decoder_init && is_current_client_parsing_over && is_video_end){
			LOGE("decoder 没有初始化");
			break;
		}
		if(is_current_client_parsing_over && is_audio_end && is_video_end){
			break;
		}
		hls_msleep(1000);
	}
	if(NULL != client){
		delete client;
		client = NULL;
	}
	return;
}


