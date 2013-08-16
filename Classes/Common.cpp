#include "common.h"
#include <cocos2d.h>
#ifdef WIN32
#include <third_party\win32\iconv\iconv.h>
#pragma comment(lib, "libiconv.lib")
#else
#include <iconv.h>
#endif // DEBUG


AppCommand* AppCommand::_instance = NULL;
FileCommand::FileCommand(void)
{
}


FileCommand::~FileCommand(void)
{
}

AppCommand::~AppCommand(void)
{
}

AppCommand::AppCommand(void)
{
}

AppCommand* AppCommand::GetInstance()
{
	if (_instance == NULL)
		_instance = new AppCommand;

	return _instance;		
}

bool AppCommand::IConvConvert(char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t &outlen)
{
	iconv_t cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
	{
		CCLOG("open iconv is failed");
		return false;
	}

#ifdef WIN32
	size_t ret = iconv(cd, (const char**)&inbuf, &inlen, (char**)&outbuf, &outlen);
#else
	size_t ret = iconv(cd, &inbuf, &inlen, (char**)&outbuf, &outlen);
#endif // WIN32

	iconv_close(cd);
	if (ret == (size_t)-1)
		return false;

	return true;
}


void AppCommand::IConvConvert_GBKToUTF8(std::string strText, std::string &strOut)
{
	if (strText.empty() || strText.length() >= 1024)
		return;

	static char szText[1024] = {0};
	memmove(szText, strText.c_str(), strText.length());
	szText[strText.length()] = 0;

	char szOut[2048] = {0};
	size_t nOutSize = 2048;
	if (!IConvConvert("gb2312", "utf-8", szText, strText.length(), szOut, nOutSize))
	{
		CCLOG("convert char format is failed");
	}
	else
	{
		strOut.assign(szOut, strlen(szOut));
	}
}
