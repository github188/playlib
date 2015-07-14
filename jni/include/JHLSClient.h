/*
 * JHLSClient.h
 *
 *  Created on: 2015��6��12��
 *      Author: LiuFengxiang
 *		 Email: lfx@jovision.com
 */

#ifndef JHLSCLIENT_H_
#define JHLSCLIENT_H_

#include "M3U8Parser.h"
#include "TSParser.h"
#include "jhlstype.h"

namespace jhls
{

class CDownload
{
public:
	CDownload(){}
	virtual ~CDownload(){}

	virtual int open(const char *fname)=0;
	virtual int read(unsigned char *data, int len)=0;
	virtual int close()=0;

	virtual int seek(int percent)=0;//percent ȡֵ(0,100)���뱣֤ƫ�ƴ�С��188�ֽ�Ϊ��λ
	virtual int getPos()=0; // �����Ѷ��ļ�ռ���ļ���С�İٷֱȡ�ȡֵ [0,100]

	virtual void OnFrameReceived(JHLSFrameType_e type, long long timeStamp, const unsigned char *frame, int len)=0;
	virtual void OnStreamType(JHLSVideoStreamType_e vtype, JHLSAudioStreamType_e atype)=0;
};

class CJHLSClient : public CTSParser
{
public:
	/**
	 *@param download function to download data.
	 *@param baseName just like this: [baseName].m3u8, baseName-1.ts, baseName-2.ts...
	 */
	CJHLSClient(CDownload &download, const char *baseName, int maxFrameLen);
	virtual ~CJHLSClient();

public:
	/**
	 *@brief parsing
	 *
	 *@return false if finished.
	 */
	bool parsing();

	/**
	 *@brief get position in second
	 */
	int getPos();

	/**
	 *@brief get the hls length with second
	 */
	int getLength();

	/**
	 *@brief seek with second in this hls file
	 */
	void seek(int second);

protected:
	virtual void OnStreamType(JHLSVideoStreamType_e vtype, JHLSAudioStreamType_e atype);

	virtual void OnVideoData(long long timeStamp, const unsigned char *frame, int len);
	virtual void OnAudioData(long long timeStamp, const unsigned char *frame, int len);


private:
	void __getM3u8();

private:
	CDownload &m_download;
	char m_baseName[MAX_M3U8_LINE_LEN];

	CM3U8Parser m_mparser;
	int m_errno;
	bool m_tsOpened;
	M3U8Node_t m_curNode;
};

} /* namespace jhls */

#endif /* JHLSCLIENT_H_ */
