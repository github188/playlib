/*
 * DVBCrc.h
 *
 *  Created on: 2015��6��9��
 *      Author: LiuFengxiang
 *		 Email: lfx@jovision.com
 */

#ifndef DVBCRC_H_
#define DVBCRC_H_
namespace jhls
{

unsigned int DVBCrc32Calculate(const unsigned char  *buffer, unsigned int size);

}

#endif /* DVBCRC_H_ */
