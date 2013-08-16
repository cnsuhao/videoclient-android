#ifndef _MEDIA_AUDIO_H_
#define _MEDIA_AUDIO_H_
extern "C"
{
#include <AL\al.h>
#include <AL\alc.h>
};
#include <stdint.h>

class MediaAudio
{
public:
	MediaAudio(void);
	virtual ~MediaAudio(void);

public:
	bool initAL();
	
	void setAudioParameter(int nSampleRate);
	void refreshData(const char *lpBuffer, size_t nBufferSize);
protected:
	ALCdevice	*lpDevice_;
	ALCcontext	*lpContext_;
	int			nCurIndex_;
	ALuint		source_;
	int			nSampleRate_;
};
#endif
