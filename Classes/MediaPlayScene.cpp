#include "MediaPlayScene.h"
#include "FileFilter.h"

using namespace cocos2d;
MediaPlayScene::MediaPlayScene(void) : lpPlayer_(NULL)
{
}

MediaPlayScene::~MediaPlayScene(void)
{
}

cocos2d::CCScene* MediaPlayScene::scene(std::string strFilePath)
{
	CCScene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		MediaPlayScene *layer = new MediaPlayScene;
		layer->setKeypadEnabled(true);
		layer->setPosition(ccp(0, 0));
		layer->initWithFile(strFilePath);
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

bool MediaPlayScene::initWithFile(std::string strFilePath)
{
	CCSize size = CCDirector::sharedDirector()->getVisibleSize();

	if (!mediaFile_.OpenFile(strFilePath))
	{
		CCLog("open media file is failed");
		CCDirector::sharedDirector()->replaceScene(FileFilter::scene());		
	}

	lpAudio_ = new MediaAudio;
	if (!lpAudio_->initAL())
	{
		delete lpAudio_;
		lpAudio_ = NULL;
		CCLog("init the OpenAL environment is failed");
		CCDirector::sharedDirector()->replaceScene(FileFilter::scene());
	}

	lpPlayer_ = new MediaPlayer;
	if (!lpPlayer_->initGL(MediaPlayer::YUVTORGB))
	{
		delete lpPlayer_;
		lpPlayer_ = NULL;
		CCLog("init the OpenGL ES 2.0 environment is failed");
		CCDirector::sharedDirector()->replaceScene(FileFilter::scene());
	}

	this->addChild(lpPlayer_);

	int64_t nAVTime = av_gettime() + 500;
	threadVideo_ = boost::thread(boost::bind(&MediaPlayScene::ThreadVideo, this, nAVTime));
	threadAudio_ = boost::thread(boost::bind(&MediaPlayScene::ThreadAudio, this, nAVTime));
	return true;
}

void MediaPlayScene::keyBackClicked()
{
	CCLOG("media play scene key back clicked");
	threadVideo_.interrupt();
	threadAudio_.interrupt();
	mediaFile_.Close();
	CCDirector::sharedDirector()->replaceScene(FileFilter::scene());	
}

void MediaPlayScene::keyMenuClicked()
{
	CCLOG("media play scene key menu clicked");
	CCDirector::sharedDirector()->end();
}

void MediaPlayScene::ThreadAudio(int64_t nAVTime)
{
	xhDecodec codec;
	codec.SetMediaStream(&mediaFile_);
	codec.OpenDecodec(AVMEDIA_TYPE_AUDIO);

	int nChannel = 0, nSampleRate = 0, nBitRate = 0, nBlockAlign = 0;
	mediaFile_.GetAudioParam(nChannel, nSampleRate, nBitRate, nBlockAlign);
	lpAudio_->setAudioParameter(nSampleRate);

	uint8_t *lpBuffer = NULL;
	if (codec.GetAudioFrameType() != AV_SAMPLE_FMT_S16)
	{
		codec.SetAudioParam(nSampleRate, AV_SAMPLE_FMT_S16);
		lpBuffer = new uint8_t[nSampleRate];
	}

	unsigned int nPTime = 0;
	while (!boost::this_thread::interruption_requested())
	{
		AVFrame *lpAVFrame = avcodec_alloc_frame();
		avcodec_get_frame_defaults(lpAVFrame);
		if (!codec.GetNextFrame(lpAVFrame, AVMEDIA_TYPE_AUDIO, nPTime))
			break;

		while(av_gettime() - nAVTime < nPTime)
			boost::this_thread::sleep(boost::posix_time::milliseconds(5));

		if (lpBuffer)
		{
			size_t nSize = 0;
			codec.DecodecAudio(lpAVFrame, lpBuffer, nSize);
			lpAudio_->refreshData((const char*)lpBuffer, nSize);
		}

		av_free(lpAVFrame);
	}

	if (lpBuffer)
	{
		delete []lpBuffer;
		lpBuffer = NULL;
	}
}

void MediaPlayScene::ThreadVideo(int64_t nAVTime)
{
	CCSize size = CCDirector::sharedDirector()->getVisibleSize();

	xhDecodec codec;
	codec.SetMediaStream(&mediaFile_);
	codec.SetSrcImageParam(mediaFile_.GetImageWidth(), mediaFile_.GetImageHight(), PIX_FMT_YUV420P);
	codec.SetDestImageParam(size.width, size.height,  PIX_FMT_YUV420P);
	codec.OpenDecodec(AVMEDIA_TYPE_VIDEO);

	unsigned int nPTime = 0;
	AVFrame *lpPAVFrame = NULL;
	size_t nSize = size.width*size.height*3/2;
	//size_t nSize = size.width*size.height*3;
	do
	{
		AVFrame *lpAVFrame = avcodec_alloc_frame();
		avcodec_get_frame_defaults(lpAVFrame);
		if (!codec.GetNextFrame(lpAVFrame, AVMEDIA_TYPE_VIDEO, nPTime))
		{
			av_free(lpAVFrame);
			break;
		}

		AVFrame *lpImage = avcodec_alloc_frame();
		avcodec_get_frame_defaults(lpImage);
		codec.ConvertImage(lpAVFrame, lpImage);

		if (lpPAVFrame == NULL)
		{
			lpPAVFrame = lpImage;
			nPTime = lpAVFrame->pts;
		}
		else if (lpAVFrame->pts > nPTime)
		{
			while(av_gettime() - nAVTime < nPTime)
				boost::this_thread::sleep(boost::posix_time::milliseconds(5));
			lpPlayer_->refreshData((const char*)lpPAVFrame->data[0], nSize);
			av_free(lpPAVFrame);
			lpPAVFrame = lpImage;
			nPTime = lpAVFrame->pts;
		}
		else
		{
			while(av_gettime() - nAVTime < nPTime)
				boost::this_thread::sleep(boost::posix_time::milliseconds(5));
			lpPlayer_->refreshData((const char*)lpPAVFrame->data[0], nSize);
			av_free(lpImage);
		}
		av_free(lpAVFrame);
	} while(1);

	codec.CloseDecodec();
}
