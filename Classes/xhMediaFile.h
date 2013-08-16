#pragma once
#include <iostream>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswscale/swscale.h>
};
#include <boost\thread\recursive_mutex.hpp>

class xhMediaFile
{
public:
	xhMediaFile(void);
	virtual ~xhMediaFile(void);
public:
	bool OpenFile(std::string strFilePath);
	bool CreateFile(std::string strFilePath);
	void Close();

	CodecID GetImageCodeID();
	CodecID GetAudioCodeID();

	int GetImageWidth();
	int GetImageHight();

	int AddStreamData(int nBitRate, int nSampleRate, int nChannel);
	int AddStreamData(int nWidth, int nHeight);

	int WriteFrame(AVPacket *lpPkt);

	void GetAudioParam(int &nChannel, int &nSamplesPer, int &nBit, int &block_align);
	void GetAudioExtraData(char **lpExtraData, int &nDataSize);
	void GetImageExtraData(char **lpExtraData, int &nDataSize);

	/*Get packet of stream*/
	bool GetVideoStreamPacket(AVPacket &pkt);
	bool GetAudioStreamPacket(AVPacket &pkt);

private:
	AVFormatContext	*lpMediaFile_;
	AVOutputFormat	*lpOutputFmt_;
	AVFormatContext *lpVideoContext_;
	AVFormatContext *lpAudioConetxt_;
	int				nAudioIndex_;
	int				nVideoIndex_;
};

