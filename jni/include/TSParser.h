/******************************************************************************

  Copyright (c) 2015-2015 Jovision Technology Co., Ltd. All rights reserved.

  File Name     : CTSDemux.h
  Version       :
  Author        : Simon Liu <lfx@jovision.com>
  Created       : 2015-6-5
  Description   : TS Demuxer for HLS (http living stream)
  History       :
  1.Date        : 2015-6-5
    Author      : Simon Liu <lfx@jovision.com>
    Modification: Created file
******************************************************************************/

#ifndef CTSPARSER_H_
#define CTSPARSER_H_

#include "jhlstype.h"

namespace jhls
{

class CTSParser
{
public:
	CTSParser(int videoBufLen);
	virtual ~CTSParser();

	void inputData(const unsigned char *ts);

protected:
	virtual void OnStreamType(JHLSVideoStreamType_e vtype, JHLSAudioStreamType_e atype);
	virtual void OnAudioInfo(JHLSAudioInfo_t &info);

	virtual void OnVideoData(long long timeStamp, const unsigned char *frame, int len);
	virtual void OnAudioData(long long timeStamp, const unsigned char *frame, int len);


private:
	void __parsePAT(const unsigned char *ts);

	//note, for HLS, I assert PMT len < 188-header bytes.
	void __parsePMT(const unsigned char *ts);
	void __parseVideo(const unsigned char *ts);
	void __parseAudio(const unsigned char *ts);

private:
	int m_videoBufLen;
	unsigned short m_pmtPid;
	unsigned short m_pcrPid;
	unsigned short m_videoPid;
	unsigned short m_audioPid;
	JHLSVideoStreamType_e m_vtype;
	JHLSAudioStreamType_e m_atype;

	unsigned char *m_videoBuf;
	int m_curVideoLen;
	unsigned char m_audioBuf[1024];
	int m_curAudioLen;


	long long m_pcrStamp;
	long long m_curVideoStamp;
	long long m_curAudioStamp;
};

} /* namespace jhls */

#endif /* CTSDEMUX_H_ */
