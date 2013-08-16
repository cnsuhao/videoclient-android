#include "xhMediaFile.h"

#ifdef WIN32
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#endif // DEBUG
xhMediaFile::xhMediaFile(void) : nVideoIndex_(-1), nAudioIndex_(-1), lpAudioConetxt_(NULL), lpVideoContext_(NULL),
	lpMediaFile_(NULL), lpOutputFmt_(NULL)
{
	av_register_all();
}


xhMediaFile::~xhMediaFile(void)
{
	Close();
}

void xhMediaFile::Close()
{
	if (lpAudioConetxt_)
	{
		avformat_close_input(&lpAudioConetxt_);
		lpAudioConetxt_ = NULL;
	}

	if (lpVideoContext_)
	{
		avformat_close_input(&lpVideoContext_);
		lpVideoContext_ = NULL;
	}

	if (lpMediaFile_)
	{
		av_write_trailer(lpMediaFile_);
		avio_close(lpMediaFile_->pb);
		avformat_free_context(lpMediaFile_);
		lpMediaFile_ = NULL;
	}
}

bool xhMediaFile::OpenFile(std::string strFilePath)
{
	this->Close();

	if (avformat_open_input(&lpVideoContext_, strFilePath.c_str(), NULL, NULL) != 0)
	{
		return false;
	}
	if (avformat_find_stream_info(lpVideoContext_, NULL) < 0)
	{
		return false;
	}

	for (unsigned int i = 0; i < lpVideoContext_->nb_streams; i++)
	{
		if (lpVideoContext_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			nVideoIndex_ = i;
		}
		else if (lpVideoContext_->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			nAudioIndex_ = i;
		}
	}

	if (nVideoIndex_ == -1)
	{
		av_close_input_file(lpVideoContext_);
		lpVideoContext_ = NULL;
	}
	if (nAudioIndex_ == -1)
		return true;

	if (avformat_open_input(&lpAudioConetxt_, strFilePath.c_str(), NULL, NULL) != 0)
	{
		av_close_input_file(lpVideoContext_);
		lpVideoContext_ = NULL;
		return false;
	}
	if (avformat_find_stream_info(lpAudioConetxt_, NULL) < 0)
	{
		return false;
	}

	return true;
}

bool xhMediaFile::CreateFile(std::string strFilePath)
{
	this->Close();

	lpOutputFmt_ = av_guess_format(NULL, strFilePath.c_str(), NULL);
	if (lpOutputFmt_ == NULL)
		return false;

	lpMediaFile_ = avformat_alloc_context();
	lpMediaFile_->oformat = lpOutputFmt_;

	memmove(lpMediaFile_->filename, strFilePath.c_str(), strFilePath.length());
	if (avio_open(&lpMediaFile_->pb, strFilePath.c_str(), 2) != 0)
	{
		avformat_free_context(lpMediaFile_);
		lpMediaFile_ = NULL;
		return false;
	}

	return true;
}

int xhMediaFile::AddStreamData(int nWidth, int nHeight)
{
	if (lpMediaFile_ == NULL)
		return -1;

	int nIndex = lpMediaFile_->nb_streams + 1;
	AVStream *lpVideoStream = av_new_stream(lpMediaFile_, nIndex);
	lpVideoStream->codec->codec_id = lpOutputFmt_->video_codec;
	lpVideoStream->codec->codec_type = AVMEDIA_TYPE_VIDEO;
	lpVideoStream->codec->width = nWidth;
	lpVideoStream->codec->height = nHeight;
	lpVideoStream->codec->time_base.num = 1;
	lpVideoStream->codec->time_base.den = 25;
	lpVideoStream->codec->bit_rate = 800000;
	lpVideoStream->codec->max_b_frames = 1;

	if (lpVideoStream->codec->codec_id == CODEC_ID_MPEG2VIDEO) 
	{
        /* just for testing, we also add B frames */
        lpVideoStream->codec->max_b_frames = 2;
    }
    if (lpVideoStream->codec->codec_id == CODEC_ID_MPEG1VIDEO)
	{
        /* needed to avoid using macroblocks in which some coeffs overflow 
           this doesnt happen with normal video, it just happens here as the 
           motion of the chroma plane doesnt match the luma plane */
        lpVideoStream->codec->mb_decision = 2;
    }

    // some formats want stream headers to be seperate
    if(!strcmp(lpMediaFile_->oformat->name, "mp4") || !strcmp(lpMediaFile_->oformat->name, "mov") 
		|| !strcmp(lpMediaFile_->oformat->name, "3gp"))
        lpVideoStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

	if (avformat_write_header(lpMediaFile_, NULL) != 0)
		return -1;

	return nIndex;
}

int xhMediaFile::AddStreamData(int nBitRate, int nSampleRate, int nChannel)
{
	if (lpMediaFile_ == NULL)
		return -1;

	int nIndex = ++lpMediaFile_->nb_streams;
	AVStream *lpAudioStream = av_new_stream(lpMediaFile_, nIndex);
	lpAudioStream->codec->codec_id = lpOutputFmt_->audio_codec;
	lpAudioStream->codec->codec_type = AVMEDIA_TYPE_AUDIO;
	lpAudioStream->codec->bit_rate = nBitRate;
	lpAudioStream->codec->sample_rate = nSampleRate;
	lpAudioStream->codec->channels = nChannel;
	if (avformat_write_header(lpMediaFile_, NULL) != 0)
		return -1;

	return nIndex;
}

int xhMediaFile::WriteFrame(AVPacket *lpPkt)
{
	if (lpPkt == NULL || lpMediaFile_ == NULL)
		return -1;

	if (av_interleaved_write_frame(lpMediaFile_, lpPkt) != 0)
		return -1;

	return 1;
}

CodecID xhMediaFile::GetImageCodeID()
{
	if (lpVideoContext_ == NULL || nVideoIndex_ == -1)
		return CODEC_ID_NONE;

	return lpVideoContext_->streams[nVideoIndex_]->codec->codec_id;
}

CodecID xhMediaFile::GetAudioCodeID()
{
	if (lpVideoContext_ == NULL || nAudioIndex_ == -1)
		return CODEC_ID_NONE;

	return lpVideoContext_->streams[nAudioIndex_]->codec->codec_id;
}

int xhMediaFile::GetImageWidth()
{
	if (lpVideoContext_ == NULL || nVideoIndex_ == -1)
		return 0;

	return lpVideoContext_->streams[nVideoIndex_]->codec->width;
}

int xhMediaFile::GetImageHight()
{
	if (lpVideoContext_ == NULL || nVideoIndex_ == -1)
		return 0;

	return lpVideoContext_->streams[nVideoIndex_]->codec->height;
}

void xhMediaFile::GetAudioParam(int &nChannel, int &nSamplesPer, int &nBit, int &block_align)
{
	if (lpAudioConetxt_ == NULL || nAudioIndex_ == -1)
		return;

	nChannel = lpAudioConetxt_->streams[nAudioIndex_]->codec->channels;
	nSamplesPer = lpAudioConetxt_->streams[nAudioIndex_]->codec->sample_rate;
	nBit = lpAudioConetxt_->streams[nAudioIndex_]->codec->bit_rate;
	block_align = lpAudioConetxt_->streams[nAudioIndex_]->codec->block_align;
}

void xhMediaFile::GetAudioExtraData(char **lpExtraData, int &nDataSize)
{ 
	if (lpAudioConetxt_ == NULL || nAudioIndex_ == -1)
		return;

	nDataSize = lpAudioConetxt_->streams[nAudioIndex_]->codec->extradata_size;
	if (nDataSize != 0)
	{
		*lpExtraData = new char[nDataSize];
		memmove(*lpExtraData, lpAudioConetxt_->streams[nAudioIndex_]->codec->extradata, nDataSize);
	}
}

void xhMediaFile::GetImageExtraData(char **lpExtraData, int &nDataSize)
{
	if (lpVideoContext_ == NULL || nVideoIndex_ == -1)
		return;

	nDataSize = lpVideoContext_->streams[nVideoIndex_]->codec->extradata_size;
	if (nDataSize != 0)
	{
		*lpExtraData = new char[nDataSize];
		memmove(*lpExtraData, lpVideoContext_->streams[nVideoIndex_]->codec->extradata, nDataSize);
	}
}


bool xhMediaFile::GetVideoStreamPacket(AVPacket &pkt)
{
	if (lpVideoContext_ == NULL)
		return false;

	bool bRet = false;
	while (av_read_frame(lpVideoContext_, &pkt) == 0)
	{
		if (pkt.stream_index == nVideoIndex_)
		{
			bRet = true;
			break;
		}
		else
		{
			av_free_packet(&pkt);
		}
	}
	 
	return bRet;
}

bool xhMediaFile::GetAudioStreamPacket(AVPacket &pkt)
{
	if (lpAudioConetxt_ == NULL)
		return false;

	bool bRet = false;
	while (av_read_frame(lpAudioConetxt_, &pkt) == 0)
	{
		if (pkt.stream_index == nAudioIndex_)
		{
			bRet = true;
			break;
		}
		else
		{
			av_free_packet(&pkt);
		}
	}

	return bRet;
}
