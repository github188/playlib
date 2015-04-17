#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "voiceenc.h"

//100毫秒一个信号
#define UPDOWN_CNT 40
#define SIN_LOW (-0x2000)//0x5000
#define SIN_HIGH 0x2000

//注意，在8K采样率的情况下，实测只能支持 [1600,2200]范围内的频率
#define FREQ_BASE 1600
#define FREQ_GAP 150

static unsigned int __code2Freq(unsigned char code) {
#if 1
	static unsigned int value[] = { 1600, 1777, //1777, //4.5
			2000, //2000, //4
			2285, //2285, //3.5
			2666, //2666, //3//2500//2550
			//2909,//2909, //2.8//2800//2870
			3200 //3200, //2.5//3300
			};
#else
	static unsigned int value[] = {
		FREQ_BASE + FREQ_GAP*0,
		FREQ_BASE + FREQ_GAP*1,
		FREQ_BASE + FREQ_GAP*2,
		FREQ_BASE + FREQ_GAP*3,
		FREQ_BASE + FREQ_GAP*4,
	};
#endif
	return value[code] * 3;
}

/**
 *@brief 将编码变成声音的原始PCM数据
 *
 *@param samplerate 采样率，至少为16000. 建议使用该值
 *@param signalCnt 信号的数量。该值越大，信号越长，声音越长。建议值为  40
 *@param code 要发送的编码数据
 *@param buffer 编好的音频数据
 *@param maxLen 音频数据Buffer的最大长度，其长度至少为：#samplerate * signalCnt / 1000
 *			其中，1000为采样频率的最低值
 *
 *@return 实际的数据长度
 */
int voiceenc_code2pcm_16K16Bit(int samplerate, int signalCnt,
		unsigned char code, unsigned char *buffer, int maxLen) {
	int i;
	int freq = __code2Freq(code);
	unsigned short *audio = (unsigned short *) buffer;

	double cntrate = ((double) samplerate) / freq;
	int cnt = 0;

	int upDown = 0;
	//4个升降为一组
	int tminus[4];
	tminus[0] = cntrate * 1;
	tminus[1] = (int) (cntrate * 2) - (int) (cntrate * 1);
	tminus[2] = (int) (cntrate * 3) - (int) (cntrate * 2);
	tminus[3] = (int) (cntrate * 4) - (int) (cntrate * 3);
	int target = 0;
	for (i = 0; i < maxLen - 20;) {
		unsigned short low = SIN_LOW;
//		unsigned short high = SIN_HIGH;
		int c;
		target += tminus[cnt++];
		if (cnt == 4)
			cnt = 0;
		int minus = target - i;

		int upCnt = minus / 2;
		int downCnt = minus - upCnt;
		int upStep = (SIN_HIGH - SIN_LOW) / upCnt;
		int downStep = (SIN_HIGH - SIN_LOW) / downCnt;
		for (c = 0; c < upCnt; c++) {
			audio[i + c] = low;
			low += upStep;
		}

		for (; c < target; c++) {
			audio[i + c] = low;
			low -= downStep;
		}
		i = target;
		upDown++;
		if (upDown >= signalCnt)
			break;
	}

	return i * 2;
}

// 8bit CRC (X(8) + X(2) + X(1) + 1)
#define AL2_FCS_COEF ((1 << 7) + (1 << 6) + (1 << 5))

unsigned char crc_8(unsigned char * data, int length) {
	unsigned char cFcs = 0;
	int i, j;

	for (i = 0; i < length; i++) {
		cFcs ^= data[i];
		for (j = 0; j < 8; j++) {
			if (cFcs & 1) {
				cFcs = (unsigned char) ((cFcs >> 1) ^ AL2_FCS_COEF);
			} else {
				cFcs >>= 1;
			}
		}
	}

	return cFcs;
}

unsigned int crc_16(unsigned char *buf, unsigned int length) {
	unsigned int i;
	unsigned int j;
	unsigned int c;
	unsigned int crc = 0xFFFF;
	for (i = 0; i < length; i++) {
		c = *(buf + i) & 0x00FF;
		crc ^= c;
		for (j = 0; j < 8; j++) {
			if (crc & 0x0001) {
				crc >>= 1;
				crc ^= 0xA001;
			} else {
				crc >>= 1;
			}
		}
	}
	crc = (crc >> 8) + (crc << 8);
	return (crc);
}

/**
 *@brief 编码，将要发送的数据，编码成适合发送的数据
 *
 *@param data 要编码的数据
 *@param len 要编码的长度
 *@param code 编码后的数据
 *@param maxLen 编码后数据的最大长度。所需长度在 len*4到len*10之间，建议len*10
 *
 *@return 编码后数据的实际长度
 */
int voiceenc_data2code(const unsigned char *data, int len, unsigned char *code,
		int maxLen) {
#define BYTE_SEP 5
#define BIT_SEP 4
	int i;
	unsigned char *p = malloc(len * 2);
	memcpy(p, data, len);
	unsigned short checksum = crc_16(p, len);

	p[len++] = (checksum & 0xFF00) >> 8;
	p[len++] = (checksum & 0xFF) >> 0;
	printf("checksum: %x\n", checksum);

	int cl = 0;

	//开头多加字节头，以确保接收到字节头
	code[cl++] = BYTE_SEP;
	code[cl++] = BYTE_SEP;
	for (i = 0; i < len; i++) {
		code[cl++] = BYTE_SEP;
		int j;
		for (j = 0; j < 4; j++) {
			unsigned char val;
			val = (p[i] >> (6 - (j * 2))) & 0x3;
			if (cl > 0 && code[cl - 1] == val) {
				code[cl++] = BIT_SEP;
			}
			code[cl++] = val;

		}
	}
	//结尾重复发
	code[cl] = code[cl - 1];
	cl++;
	code[cl] = code[cl - 1];
	cl++;
	free(p);
	return cl;
}
