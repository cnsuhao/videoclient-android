#ifndef __MEDIA_PLAY_SCENE_H__
#define __MEDIA_PLAY_SCENE_H__
#include <cocos2d.h>
#include "xhMediaFile.h"
#include "xhDecodec.h"
#include "MediaPlayer.h"
#include <boost\thread.hpp>
#include "MediaAudio.h"

class MediaPlayScene : public cocos2d::CCLayer
{
public:
	MediaPlayScene(void);
	virtual ~MediaPlayScene(void);

	static cocos2d::CCScene* scene(std::string strFilePath);
	bool initWithFile(std::string strFilePath);
	void cancel();

	virtual void keyBackClicked();
	virtual void keyMenuClicked();
protected:
	void ThreadAudio(int64_t nAVTime);
	void ThreadVideo(int64_t nAVTime);
protected:
	xhMediaFile		mediaFile_;
	boost::thread	threadVideo_;
	boost::thread	threadAudio_;
	MediaPlayer		*lpPlayer_;
	MediaAudio		*lpAudio_;
};

#endif