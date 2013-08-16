#include "xhDecodec.h"
#include "xhThreadSafe.h"
extern "C"
{
#include <libavutil/mathematics.h>
};


xhDecodec::xhDecodec(void) : lpMediaStream_(NULL), lpBuffer_(NULL), lpVSwsContext_(NULL),
	nDestWidth_(0), nDestHeight_(0), lpVideoContext_(NULL),
	lpAudioContext_(NULL), lpASwsContext_(NULL)
{
	av_register_all();
}


xhDecodec::~xhDecodec(void)
{
	CloseDecodec();
}

void xhDecodec::CloseDecodec()
{
	if (lpVSwsContext_)
	{
		sws_freeContext(lpVSwsContext_);
		lpVSwsContext_ = NULL;
	}

	if (lpBuffer_)
	{
		av_free(lpBuffer_);
		lpBuffer_ = NULL;
	}
}

bool xhDecodec::OpenDecodec(AVMediaType codecType)
{
	if (lpMediaStream_ == NULL)
		return false;

	if (codecType == AVMEDIA_TYPE_VIDEO)
	{
		CodecID codeID = lpMediaStream_->GetImageCodeID();
		AVCodec *lpVideoCodec = avcodec_find_decoder(codeID);
		if (lpVideoCodec == NULL)
		{
			return false;
		}

		lpVideoContext_ = avcodec_alloc_context3(lpVideoCodec);
		lpVideoContext_->width = lpMediaStream_->GetImageWidth();
		lpVideoContext_->height = lpMediaStream_->GetImageHight();
		lpMediaStream_->GetImageExtraData((char**)&lpVideoContext_->extradata, lpVideoContext_->extradata_size);

		/*warning This function is not thread safe!*/
		{
			xhThreadSafe xSafe;
			if (avcodec_open2(lpVideoContext_, lpVideoCodec, NULL) < 0)
			{
				av_free(lpVideoContext_);
				lpVideoContext_ = NULL;
				return false;
			}
		}
	}
	else if (codecType == AVMEDIA_TYPE_AUDIO)
	{
		CodecID codeID = lpMediaStream_->GetAudioCodeID();
		AVCodec *lpAudioCodec = avcodec_find_decoder(codeID);
		if (lpAudioCodec == NULL)
		{
			return false;
		}

		lpAudioContext_ = avcodec_alloc_context3(lpAudioCodec);
		lpMediaStream_->GetAudioParam(lpAudioContext_->channels, lpAudioContext_->sample_rate, 
			lpAudioContext_->bit_rate, lpAudioContext_->block_align);
		lpMediaStream_->GetAudioExtraData((char**)&lpAudioContext_->extradata, lpAudioContext_->extradata_size);
		/*warning This function is not thread safe!*/
		{
			xhThreadSafe xSafe;
			if (avcodec_open2(lpAudioContext_, lpAudioCodec, NULL) < 0)
			{
				av_free(lpAudioContext_);
				lpAudioContext_ = NULL;
				return false;
			}
		}
	}

	return true;
}

AVSampleFormat xhDecodec::GetAudioFrameType()
{
	if (lpAudioContext_ == NULL)
		return AV_SAMPLE_FMT_NONE;
	else
		return lpAudioContext_->sample_fmt;
}

PixelFormat xhDecodec::GetImageFrameType()
{
	if (lpVideoContext_ == NULL)
		return PIX_FMT_NONE;
	else
		return lpVideoContext_->pix_fmt;
}
#include <cocos2d.h>
bool xhDecodec::GetNextFrame(AVFrame* lpAVFrame, AVMediaType StreamType, unsigned int &nAVTime)
{
	if (lpMediaStream_ == NULL)
		return false;

	int nSucess = 0;
	int64_t nPTS = AV_NOPTS_VALUE;
	while (!nSucess)
	{
		AVPacket avpk;
		av_init_packet(&avpk);
		if (StreamType == AVMEDIA_TYPE_VIDEO)
		{
			if (lpMediaStream_->GetVideoStreamPacket(avpk) && lpVideoContext_->codec != NULL)
			{
				if (avcodec_decode_video2(lpVideoContext_, lpAVFrame, &nSucess, &avpk) == 0)
				{
					av_free_packet(&avpk);
					return false;
				}

				nAVTime = avpk.pts * 1000;
				av_free_packet(&avpk);
			}
			else
			{
				av_free_packet(&avpk);
				return false;
			}
		}
		else if (StreamType == AVMEDIA_TYPE_AUDIO)
		{
			if (lpMediaStream_->GetAudioStreamPacket(avpk) && lpAudioContext_->codec != NULL)
			{
				if (avcodec_decode_audio4(lpAudioContext_, lpAVFrame, &nSucess, &avpk) <= 0)
				{
					av_free_packet(&avpk);
					return false;
				}

				nAVTime = avpk.pts * 1000;
				av_free_packet(&avpk);
			}
			else
			{
				av_free_packet(&avpk);
				return false;
			}
		}
	}
	return true;
}

void xhDecodec::SetAudioParam(int nSampleRate, AVSampleFormat avFormat)
{
	nSampleRate_ = nSampleRate;
	avAudioFormat_ = avFormat;
}

bool xhDecodec::DecodecAudio(AVFrame *lpAvFrame, uint8_t *lpBuffer, size_t &nBufferSize)
{
	if (lpMediaStream_ == NULL || nSampleRate_ == 0)
		return false;

	if (lpASwsContext_ == NULL)
	{
		lpASwsContext_ = swr_alloc_set_opts(NULL, lpAudioContext_->channel_layout, avAudioFormat_, 
			lpAudioContext_->sample_rate, lpAudioContext_->channel_layout, lpAudioContext_->sample_fmt, 
			lpAudioContext_->sample_rate, 0, NULL);
		swr_init(lpASwsContext_);
	}

	int nPerChannelSize = swr_convert(lpASwsContext_, &lpBuffer, lpAvFrame->nb_samples, 
		(const uint8_t**)lpAvFrame->extended_data, lpAvFrame->nb_samples);
	nBufferSize = nPerChannelSize * lpAudioContext_->channels * 2;
	
	return true;
}

void xhDecodec::SetDestImageParam(int nWidth, int nHeight, PixelFormat avFormat)
{
	nDestWidth_ = nWidth;
	nDestHeight_ = nHeight;
	avDestFormat_ = avFormat;
}

void xhDecodec::SetSrcImageParam(int nWidth, int nHeight, PixelFormat avFormat)
{
	nSrcWidth_ = nWidth;
	nSrcHeight_ = nHeight;
	avSrcFormat_ = avFormat;
}

bool xhDecodec::ConvertImage(AVFrame *lpAvFrame, AVFrame *lpDestAvFrame)
{
	if (nDestWidth_ == 0 || nDestHeight_ == 0 || lpAvFrame == NULL || lpDestAvFrame == NULL ||
		nSrcWidth_ == 0 || nSrcHeight_ == 0 || lpDestAvFrame == NULL)
		return false;

	if (lpVSwsContext_ == NULL)
	{
		//sometimes is SWS_FAST_BILINEAR,sometimes will be wrong
		lpVSwsContext_ = sws_getContext(nSrcWidth_, nSrcHeight_, avSrcFormat_, nDestWidth_, nDestHeight_, 
			avDestFormat_, SWS_POINT, NULL, NULL, NULL);
		if (lpVSwsContext_ == NULL)
		{
			return false;
		}
	}

	//if only use to change image size,don't upside down the image data.
	if (avDestFormat_ != PIX_FMT_YUV420P)
	{
		switch (avSrcFormat_)
		{
		case PIX_FMT_YUV420P:
		case PIX_FMT_YUVJ420P:
		case PIX_FMT_RGB555:
			{
				lpAvFrame->data [0] += lpAvFrame->linesize[0] *  nSrcHeight_;
				lpAvFrame->data [1] += lpAvFrame->linesize[1] *  nSrcHeight_/2;
				lpAvFrame->data [2] += lpAvFrame->linesize[2] *  nSrcHeight_/2;
				lpAvFrame->linesize[0] = -lpAvFrame->linesize[0];
				lpAvFrame->linesize[1] = -lpAvFrame->linesize[1];
				lpAvFrame->linesize[2] = -lpAvFrame->linesize[2];
				break;
			}
		default:
			break;
		}
	}

	//Malloc the memory to save the data of original, because the frame of p depend on previous frame,the frame of b depend on both.
	if (lpBuffer_ == NULL)
	{
		int nDataSize = avpicture_get_size(avDestFormat_, nDestWidth_, nDestHeight_);
		lpBuffer_ = static_cast<uint8_t*>(av_malloc_array(nDataSize, sizeof(uint8_t)));
	}
	avpicture_fill((AVPicture *)lpDestAvFrame, lpBuffer_, avDestFormat_, nDestWidth_, nDestHeight_);

	if (sws_scale(lpVSwsContext_, lpAvFrame->data, lpAvFrame->linesize, 0, nSrcHeight_, 
		lpDestAvFrame->data, lpDestAvFrame->linesize) <= 0)
	{
		return false;
	}

	return true;
}