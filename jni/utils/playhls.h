#ifndef _PLAYHLS_H_
#define _PLAYHLS_H_

#include <jni.h>
#include <signal.h>
#include "../defines.h"
#include "commons.h"
#include <iostream>

//class PlayHLS{
//public:
//	PlayHLS();
	/**
	 * aythJson 是账号从服务器获取的信息
	 */
    void playerInit(char *m3u8Path, char* url, char* filename,char* authJson);
    void playerClose(int index);
//private:

//};

#endif
