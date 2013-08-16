#ifndef __COMMAND_H_
#define __COMMAND_H_
#include <boost\filesystem.hpp>
#include <string.h>

class FileCommand
{
public:
	FileCommand(void);
	virtual ~FileCommand(void);

public:
};

class AppCommand
{
public:
	virtual ~AppCommand(void);

public:
	static AppCommand* GetInstance();
	void IConvConvert_GBKToUTF8(std::string strText, std::string &strOut);

protected:
	AppCommand(void);
	//bool IConvConvert(char *from_charset, char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen);
	bool IConvConvert(char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t &outlen);
	static AppCommand *_instance;
};
#endif // !__COMMAND_H_
