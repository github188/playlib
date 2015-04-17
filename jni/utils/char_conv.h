#ifndef _CHAR_CONV_H
#define _CHAR_CONV_H

#include <stddef.h>

#define CODE_GBK		"GBK"
#define CODE_UTF8		"UTF-8"
#define CODE_UNICODE	"UTF-16LE"

class CharConvert {
private:
	size_t size;
	char* content;

public:
	CharConvert();
	virtual ~CharConvert();

	char* foo(const char* src, const char* from, const char* to);

private:
	char *resize(size_t newSize);
};

class GBK: public CharConvert {
private:
	const char* _gbk;

public:
	GBK(const char* gbk);

	GBK* update(const char* gbk);

	char* toUTF8();
	wchar_t* toUnicode();
};

class Unicode: public CharConvert {
private:
	const wchar_t* _unicode;

public:
	Unicode(const wchar_t* unicode);

	Unicode* update(const wchar_t* unicode);

	char* toGBK();
	char* toUTF8();
};

class UTF8: public CharConvert {
private:
	const char* _utf8;

public:
	UTF8(const char* utf8);

	UTF8* update(const char* utf8);

	wchar_t* toUnicode();
	char* toGBK();
};

#endif
