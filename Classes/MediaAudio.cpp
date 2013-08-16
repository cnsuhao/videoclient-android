#include "MediaAudio.h"
#include <cocos2d.h>

#ifdef WIN32
#pragma comment(lib, "libOpenAL32.dll.a")
#endif
using namespace cocos2d;

#define NULL 0
#define QUEUECOUNT 10

MediaAudio::MediaAudio(void) : lpDevice_(NULL), nSampleRate_(0), lpContext_(NULL),
	nCurIndex_(0)
{
}


MediaAudio::~MediaAudio(void)
{
}

bool MediaAudio::initAL()
{
	lpDevice_ = alcOpenDevice(NULL);
	if (lpDevice_ == NULL)
	{
		CCLog("OpenAL open device is failed");
		return false;
	}

	lpContext_ = alcCreateContext(lpDevice_, NULL);
	if (!alcMakeContextCurrent(lpContext_))
	{
		CCLog("make context is failed");
		alcDestroyContext(lpContext_);
		lpContext_ = NULL;
		alcCloseDevice(lpDevice_);
		lpDevice_ = NULL;
		return false;
	}

	alGenSources(1, &source_);
	alSpeedOfSound(1.0);
	alSourcef(source_, AL_PITCH, 1.0f);
	alSourcef(source_, AL_GAIN, 1.0f);
	if (alGetError() != 0)
	{
		CCLog("set sources is failed");
		alcDestroyContext(lpContext_);
		lpContext_ = NULL;
		alcCloseDevice(lpDevice_);
		lpDevice_ = NULL;
		return false;
	}

	return true;
}

void MediaAudio::setAudioParameter(int nSampleRate)
{
	nSampleRate_ = nSampleRate;
}

void MediaAudio::refreshData(const char *lpBuffer, size_t nBufferSize)
{
	ALuint		buffer_;
	alGenBuffers((ALuint)1, &buffer_);
	alBufferData(buffer_, AL_FORMAT_STEREO16, lpBuffer, nBufferSize, nSampleRate_);
	if (alGetError() != 0)
	{
		CCLog("set data to buffer is failed");
		return;
	}
	alSourceQueueBuffers(source_, 1, &buffer_);

	ALint value; 
	alGetSourcei(source_, AL_SOURCE_STATE, &value);
	if (value != AL_PLAYING)
	{
		alSourcePlay(source_);
		if (alGetError() != 0)
		{
			CCLog("start play audio is failed");
			return;
		}
	}

	ALint nProcessed = 0, nQueued = 0;
	alGetSourcei(source_, AL_BUFFERS_PROCESSED, &nProcessed); 
	alGetSourcei(source_, AL_BUFFERS_QUEUED, &nQueued);
	while (nProcessed--) 
	{ 
		ALuint  buffer; 
		alSourceUnqueueBuffers(source_, 1, &buffer);
		alDeleteBuffers(1, &buffer); 
	} 
}