#pragma once
#include "xhMediaFile.h"
#include <iostream>
#include <fstream>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};

class xhDecodec
{
public:
	xhDecodec(void);
	virtual ~xhDecodec(void);
public:
	void SetMediaStream(xhMediaFile* lpStream){lpMediaStream_=lpStream;};

	bool OpenDecodec(AVMediaType codecType);
	void CloseDecodec();

	AVSampleFormat GetAudioFrameType();
	PixelFormat GetImageFrameType();

	bool GetNextFrame(AVFrame* lpAVFrame, AVMediaType StreamType, unsigned int &nAVTime);

	void SetAudioParam(int nSampleRate, AVSampleFormat avFormat);
	bool DecodecAudio(AVFrame *lpAvFrame, uint8_t *lpBuffer, size_t &nBufferSize);

public:
	void SetSrcImageParam(int nWidth, int nHeight, PixelFormat avFormat);
	void SetDestImageParam(int nWidth, int nHeight, PixelFormat avFormat);
	bool ConvertImage(AVFrame *lpAvFrame, AVFrame *lpDestAvFrame);
private:
	AVCodecContext *lpVideoContext_;
	AVCodecContext *lpAudioContext_;
	xhMediaFile		*lpMediaStream_;
	SwsContext		*lpVSwsContext_;
	SwrContext		*lpASwsContext_;
	uint8_t			*lpBuffer_;
	int				nDestWidth_;
	int				nDestHeight_;
	int				nSrcWidth_;
	int				nSrcHeight_;
	int				nSampleRate_;
	PixelFormat		avDestFormat_;
	PixelFormat		avSrcFormat_;
	AVSampleFormat	avAudioFormat_;
};

