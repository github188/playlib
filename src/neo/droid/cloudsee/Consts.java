package neo.droid.cloudsee;

import java.io.File;

import android.os.Environment;

public class Consts {

	public static final String SD_CARD_PATH = Environment
			.getExternalStorageDirectory().getPath();

	public static final String LOG_PATH = SD_CARD_PATH + File.separator + "CSL";

	public static final String TAG_APP = "CS_APP";
	public static final String TAG_UI = "CS_UI";
	public static final String TAG_PLAY = "CS_PLAY";
	public static final String TAG_XX = "CS_XX";

	public static final int CHANNEL_JY = 5555;

	public static final int TYPE_GET_PARAM = 0x02;
	public static final int TYPE_SET_PARAM = 0x03;

	public static final int TYPE_EX_UPDATE = 0x01;
	public static final int TYPE_EX_SENSOR = 0x02;
	public static final int TYPE_EX_STORAGE_SWITCH = 0x07;
	public static final int TYPE_EX_SET_DHCP = 0x09;

	public static final int COUNT_EX_UPDATE = 0x01;
	public static final int COUNT_EX_NETWORK = 0x02;
	public static final int COUNT_EX_STORAGE = 0x03;
	public static final int COUNT_EX_SENSOR = 0x08;

	public static final String FORMATTER_FLASH_SWITCH = "FlashMode=%d;";
	public static final String FORMATTER_MOTION_DETECTION_SWITCH = "bMDEnable=%d;";
	public static final String FORMATTER_PN_SWITCH = "PNMode=%d";
	public static final String FORMATTER_AP_SWITCH = "apModeOn=%d";

	public static final String FORMATTER_STORAGE_MODE = "storageMode=%d";
	public static final String FORMATTER_TALK_SWITCH = "talkSwitch=%d";
	public static final String FORMATTER_SET_WIFI = "ACTIVED=%d;WIFI_ID=%s;WIFI_PW=%s;";
	public static final String FORMATTER_SAVE_WIFI = "ACTIVED=%d;WIFI_ID=%s;WIFI_PW=%s;WIFI_AUTH=%s;WIFI_ENC=%s;";
	public static final String FORMATTER_SET_DHCP = "ACTIVED=%d;bDHCP=%d;nlIP=%d;nlNM=%d;nlGW=%d;nlDNS=%d;";
	public static final String FORMATTER_SET_BPS_FPS = "[CH%d];width=%d;height=%d;nMBPH=%d;framerate=%d;rcMode=%d;";

	public static final int CALL_CONNECT_CHANGE = 0xA1;
	public static final int CALL_NORMAL_DATA = 0xA2;
	public static final int CALL_CHECK_RESULT = 0xA3;
	public static final int CALL_CHAT_DATA = 0xA4;
	public static final int CALL_TEXT_DATA = 0xA5;
	public static final int CALL_DOWNLOAD = 0xA6;
	public static final int CALL_PLAY_DATA = 0xA7;
	public static final int CALL_LAN_SEARCH = 0xA8;
	public static final int CALL_NEW_PICTURE = 0xA9;
	public static final int CALL_STAT_REPORT = 0xAA;
	public static final int CALL_GOT_SCREENSHOT = 0xAB;
	public static final int CALL_PLAY_DOOMED = 0xAC;
	public static final int CALL_PLAY_AUDIO = 0xAD;
	public static final int CALL_QUERY_DEVICE = 0xAE;
	public static final int CALL_HDEC_TYPE = 0xAF;
	public static final int CALL_LIB_UNLOAD = 0xB0;
	public static final int CALL_GEN_VOICE = 0xB1;
	public static final int CALL_PLAY_BUFFER = 0xB2;

	public static final int BUFFER_START = -1;
	public static final int BUFFER_FINISH = -2;

	public static final int RTMP_CONN_SCCUESS = (0x01 | 0xA0);
	public static final int RTMP_CONN_FAILED = (0x02 | 0xA0);
	public static final int RTMP_DISCONNECTED = (0x03 | 0xA0);
	public static final int RTMP_EDISCONNECT = (0x04 | 0xA0);

	public static final int DEVICE_TYPE_UNKOWN = -1;
	public static final int DEVICE_TYPE_DVR = 0x01;
	public static final int DEVICE_TYPE_950 = 0x02;
	public static final int DEVICE_TYPE_951 = 0x03;
	public static final int DEVICE_TYPE_IPC = 0x04;
	public static final int DEVICE_TYPE_NVR = 0x05;

	public static final int JAE_ENCODER_SAMR = 0x00;
	public static final int JAE_ENCODER_ALAW = 0x01;
	public static final int JAE_ENCODER_ULAW = 0x02;
	public static final int JAE_ENCODER_G729 = 0x03;

	// [Neo] TODO
	public static final int ENC_8BIT_PCM_SIZE = 320;
	public static final int ENC_16BIT_PCM_SIZE = 320;
	public static final int ENC_AMR_SIZE = 640;
	public static final int ENC_G711_SIZE = 640;
	public static final int ENC_G729_SIZE = 960;

	public static final int TEXT_REMOTE_CONFIG = 0x01;
	public static final int TEXT_AP = 0x02;
	public static final int TEXT_GET_STREAM = 0x03;

	public static final int FLAG_WIFI_CONFIG = 0x01;
	public static final int FLAG_WIFI_AP = 0x02;
	public static final int FLAG_BPS_CONFIG = 0x03;
	public static final int FLAG_CONFIG_SCCUESS = 0x04;
	public static final int FLAG_CONFIG_FAILED = 0x05;
	public static final int FLAG_CONFIG_ING = 0x06;
	public static final int FLAG_SET_PARAM = 0x07;
	public static final int FLAG_GPIN_ADD = 0x10;
	public static final int FLAG_GPIN_SET = 0x11;
	public static final int FLAG_GPIN_SELECT = 0x12;
	public static final int FLAG_GPIN_DEL = 0x13;

	public static final int FLAG_CAPTURE_FLASH = 0x10;
	public static final int FLAG_GET_MD_STATE = 0x11;
	public static final int FLAG_SET_MD_STATE = 0x12;

	public static final int FLAG_GET_PWD = 0x14;

	public static final int FLAG_GET_PARAM = 0x50;
	public static final int FLAG_SET_PARAM_OK = 0x55;

	public static final int EX_WIFI_CONFIG = 0x0A;

	public static final int RESULT_SUCCESS = 0x01;
	public static final int RESULT_NO_FILENAME = 0x10;
	public static final int RESULT_OPEN_FAILED = 0x11;

	public static final int ARG1_PLAY_BAD = 0x01;

	public static final int DOWNLOAD_REQUEST = 0x20;
	public static final int DOWNLOAD_START = 0x21;
	public static final int DOWNLOAD_FINISHED = 0x22;
	public static final int DOWNLOAD_ERROR = 0x23;
	public static final int DOWNLOAD_STOP = 0x24;
	public static final int DOWNLOAD_TIMEOUT = 0x76;

	public static final int BAD_STATUS_NOOP = 0x00;
	public static final int BAD_STATUS_DECODE = 0x01;
	public static final int BAD_STATUS_OPENGL = 0x02;
	public static final int BAD_STATUS_AUDIO = 0x03;
	public static final int BAD_STATUS_OMX = 0x04;
	public static final int BAD_STATUS_FFMPEG = 0x05;
	public static final int BAD_STATUS_LEGACY = 0x06;
	public static final int PLAYBACK_DONE = 0x10;
	public static final int VIDEO_SIZE_CHANGED = 0x11;

	public static final int BAD_SCREENSHOT_NOOP = 0x00;
	public static final int BAD_SCREENSHOT_INIT = 0x01;
	public static final int BAD_SCREENSHOT_CONV = 0x02;
	public static final int BAD_SCREENSHOT_OPEN = 0x03;

	public static final int BAD_HAS_CONNECTED = -1;
	public static final int BAD_CONN_OVERFLOW = -2;
	public static final int BAD_NOT_CONNECT = -3;
	public static final int BAD_ARRAY_OVERFLOW = -4;
	public static final int BAD_CONN_UNKOWN = -5;

	public static final int ARG2_REMOTE_PLAY_OVER = 0x32;
	public static final int ARG2_REMOTE_PLAY_ERROR = 0x39;
	public static final int ARG2_REMOTE_PLAY_TIMEOUT = 0x77;

	public static final String IPC_DEFAULT_USER = "jwifiApuser";
	public static final String IPC_DEFAULT_PWD = "^!^@#&1a**U";
	public static final String IPC_DEFAULT_IP = "10.10.0.1";
	public static final int IPC_DEFAULT_PORT = 9101;

}
