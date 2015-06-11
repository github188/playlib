#include "char_conv.h"

#include <string.h>
#include <stdlib.h>

#include <iconv.h>

CharConvert::CharConvert() {
	size = 0;
	content = NULL;
}

CharConvert::~CharConvert() {
	delete[] content;
	content = NULL;
	size = 0;
}

char* CharConvert::foo(const char* src, const char* from, const char* to) {
	if (NULL == src || strlen(src) == 0) {
		return NULL;
	}

	size_t retlen = 0;
	size_t inbytesleft = strlen(src);
	if (!strncmp(from, CODE_UNICODE, 8)) {
		inbytesleft = wcslen((wchar_t *) src) * 2;
	}
	size_t outbytesleft = inbytesleft * 4 + 2;
	retlen = outbytesleft;


	char* tmp = resize(outbytesleft);

	char* inbuf = (char*) src;
	char* outbuf = (char*) tmp;

	iconv_t cd = iconv_open(to, from);
	size_t errorNo = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
	retlen -= outbytesleft;

	if (errorNo == (size_t) -1) {
		memcpy(tmp, src, inbytesleft);
	} else {
		memset(tmp + retlen, 0, outbytesleft);
	}

	iconv_close(cd);
	return tmp;
}

char* CharConvert::resize(size_t newSize) {
	if (size < newSize) {
		delete[] content;
		size = newSize;
		content = new char[newSize];
	}
	memset(content, 0, newSize);
	return content;
}

GBK::GBK(const char* gbk) {
	_gbk = gbk;
}

GBK* GBK::update(const char* gbk) {
	_gbk = gbk;
	return this;
}

char* GBK::toUTF8() {
	return foo(_gbk, CODE_GBK, CODE_UTF8);
}

wchar_t* GBK::toUnicode() {
	return (wchar_t*) foo(_gbk, CODE_GBK, CODE_UNICODE);
}

Unicode::Unicode(const wchar_t* unicode) {
	_unicode = unicode;
}

Unicode* Unicode::update(const wchar_t* unicode) {
	_unicode = unicode;
	return this;
}

char* Unicode::toGBK() {
	return foo((const char*) _unicode, CODE_UNICODE, CODE_GBK);
}

char* Unicode::toUTF8() {
	return foo((const char*) _unicode, CODE_UNICODE, CODE_UTF8);
}

UTF8::UTF8(const char *utf8) {
	_utf8 = utf8;
}

UTF8* UTF8::update(const char *utf8) {
	_utf8 = utf8;
	return this;
}

wchar_t* UTF8::toUnicode() {
	return (wchar_t*) foo(_utf8, CODE_UTF8, CODE_UNICODE);
}

char* UTF8::toGBK() {
	return foo(_utf8, CODE_UTF8, CODE_GBK);
}

