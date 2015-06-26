/******************************************************************************

  Copyright (c) 2013 . All rights reserved.

  ******************************************************************************
  File Name     : oss_auth.h
  Version       : Daomeng Han <itangwang@126.com>
  Author        : Daomeng Han <itangwang@126.com>
  Created       : 2014-12-02 14:05:32
  Description   : 
  History       : 
  1.Date        : 2014-12-02 14:05:33
    Author      : Daomeng Han <itangwang@126.com>
	Version     : 0.1
	Modification: 
******************************************************************************/

#ifndef _OSS_AUTH_H_
#define _OSS_AUTH_H_

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/* *
 * 生成签名值，传入参数分别为：
 * @authurl:			 返回URL字符串
 * @access_host：		 用户host
 * @access_id： 		 用户ID
 * @access_key：    	 用户的key
 * @access_bucket:	 用户根目录
 * @method：          请求方法
 * @date:				 时间，1970年1月1日0时0分0秒起的总秒数+过期秒数
 * @resource：        请求的资源
 * @//yun_type  0 中维云存储 1阿里云存储
 * 该函数返回两个值，第一个返回值为签名结果，由函数直接返回，
 * 第二个返回值为 sign_len，由最后一个参数 sign_len 返回。
 * */
int gen_authorization(int yun_type, char *authurl, 
	const char* access_host, 
	const char* access_id,
	const char* access_key, 
	const char* access_bucket, 
	const char* method, 
	const char *date, 
	const char* resource);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif // GENERATE_AUTHENTICATION_H
