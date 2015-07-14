/******************************************************************************

  Copyright (c) 2015-2015 Jovision Technology Co., Ltd. All rights reserved.

  File Name     : CTSDemux.h
  Version       :
  Author        : Simon Liu <lfx@jovision.com>
  Created       : 2015-6-8
  Description   : TS Demuxer for HLS (http living stream)
  History       :
  1.Date        : 2015-6-8
    Author      : Simon Liu <lfx@jovision.com>
    Modification: Created file
******************************************************************************/

#ifndef M3U8PARSER_H_
#define M3U8PARSER_H_

#include <vector>

#include "jhlstype.h"

namespace jhls
{

class CM3U8Parser
{
public:
	CM3U8Parser();
	virtual ~CM3U8Parser();

	/**
	 *@brief change http://xxx/abc.def to http://xxx/[fname]
	 *
	 *@param url maybe url of m3u8, http://xxx/abc.m3u8, ftp://xxx/xxx/abc.def
	 *@param fname name to change.
	 *
	 *@return url if success.
	 */
	static char *urlChangeFileName(char *url, char *fname);


	int renewFile(const char *fname);
	int renewStr(const char *buffer);

	M3U8Node_t *getNextNode();

	bool isBEndList() const
	{
		return m_bEndList;
	}

	int getMediaSequence() const
	{
		return m_mediaSequence;
	}

	int getTargetDuration() const
	{
		return m_targetDuration;
	}

	int getNodeCnt();
	M3U8Node_t *getNode(int index);
	int getCurNodeIndex();


	/**
	 *@brief get the sum of all ts second
	 */
	int getLength();

	/**
	 *@brief return the second of current
	 */
	int getPos();

	/**
	 *@brief set current node with second
	 *@param second the target second to play
	 *@return index of current node.
	 */
	int seek(int second);

private:
	void __parseLine(const char *line);
	void __setCurNode(const char *nodeName);


private:
	int m_mediaSequence;
	int m_targetDuration;
	bool m_bEndList;
	std::vector<M3U8Node_t> m_nodeList;
	unsigned int m_curNodeIndex;
	M3U8Node_t *m_lastGettedNode;
};

} /* namespace jhls */

#endif /* M3U8_H_ */
