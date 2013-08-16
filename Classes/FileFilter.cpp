#include "FileFilter.h"
#include "HelloWorldScene.h"
#include <boost\filesystem.hpp>
#include "Common.h"
#include "MediaPlayScene.h"

using namespace cocos2d;
FileFilter::FileFilter(void)
{
}


FileFilter::~FileFilter(void)
{
}

cocos2d::CCScene* FileFilter::scene()
{
	CCScene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = CCScene::create();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		FileFilter *layer = FileFilter::create();
		CC_BREAK_IF(!layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

void GetMediaFile(std::vector<FileInfo> &lsFile, std::string strDirctory, int nDeep = 2)
{
	if (!boost::filesystem::exists(strDirctory))
		return;

	try
	{
		boost::filesystem::path curPath(strDirctory.c_str());
		boost::filesystem::directory_iterator dir_end;
		for (boost::filesystem::directory_iterator iterIdx(curPath); iterIdx != dir_end; ++iterIdx)
		{
			if (strcmp(".", iterIdx->path().filename().string().c_str()) == 0 ||
				strcmp("..", iterIdx->path().filename().string().c_str()) == 0)
			continue;

			if (boost::filesystem::is_directory(*iterIdx))
			{
				if (nDeep > 0)
					GetMediaFile(lsFile, iterIdx->path().string(), nDeep-1);
				continue;
			}

			if (strcmp(".rmvb", boost::filesystem::extension(*iterIdx).c_str()) != 0)
				continue;

			FileInfo fl;
#ifdef WIN32
			AppCommand::GetInstance()->IConvConvert_GBKToUTF8(iterIdx->path().filename().string(), fl.strFileName);
			AppCommand::GetInstance()->IConvConvert_GBKToUTF8(iterIdx->path().string(), fl.strFilePath);
#else
			fl.strFileName = iterIdx->path().filename().string();
			fl.strFilePath = iterIdx->path().string();
#endif // WIN32
			lsFile.push_back(fl);
		}
	}
	catch (...)
	{
	}
}

bool FileFilter::init()
{
	CCLayer::init();

	this->setKeypadEnabled(true);

#ifdef WIN32
	std::string strRoot = boost::filesystem::complete(boost::filesystem::path()).parent_path().string();
#else
	std::string strRoot;
	AppCommand::GetInstance()->IConvConvert_GBKToUTF8("/mnt/sdcard", strRoot);
	if (boost::filesystem::exists(boost::filesystem::path(strRoot)))
	{
		CCLog("don't have the sdcard");
		strRoot = boost::filesystem::complete(boost::filesystem::path()).string();
	}
#endif
	CCLOG("Get media file from %s", strRoot.c_str());
	GetMediaFile(_lsFile, strRoot);
	CCLOG("Get file count is %d", _lsFile.size());

	if (!_lsFile.empty())
	{
		_ScrollView = CCCGameScrollView::create();    //创建一个scrollview
		_ScrollView->createContainer(
			this,
			_lsFile.size(),//总共的Page数量
			CCSizeMake(CCDirector::sharedDirector()->getWinSize().width*0.7,CCDirector::sharedDirector()->getWinSize().height));//每一个Page的尺寸
		_ScrollView->setPosition(ccp(0,0));
		_ScrollView->setContentOffset(CCPointZero);
		_ScrollView->setViewSize(CCDirector::sharedDirector()->getWinSize());
		_ScrollView->setDirection(cocos2d::extension::kCCScrollViewDirectionHorizontal);  //设置滚动的方向，有三种可以选择

		this->addChild(_ScrollView, 1);

		CCSize size = CCDirector::sharedDirector()->getVisibleSize();
		CCSprite* pSprite = CCSprite::create("bg.png");
		pSprite->setPosition(ccp(size.width/2, size.height/2));
		this->addChild(pSprite, 0);
	}

	return true;
}

void FileFilter::cancel()
{
	CCDirector::sharedDirector()->replaceScene(HelloWorld::scene());
}

bool FileFilter::scrollViewInitPage( cocos2d::CCNode* pPage, int nPage)
{
	CCLOG("init page index is %d", nPage);
	if (nPage >= _lsFile.size())
		return false;

	CCSize size = pPage->getContentSize();
	CCSprite *sprite = CCSprite::create("rmvb.png");
	sprite->setPosition(ccp(size.width/2, size.height/2));
	pPage->addChild(sprite, 1);

	CCLOG("show file name is %s", _lsFile[nPage].strFileName.c_str());
	CCLabelTTF* lpLable = CCLabelTTF::create(_lsFile[nPage].strFileName.c_str(), "STHeitiJ-Light", 24);
	lpLable->setPosition(ccp(sprite->getPosition().x, sprite->getPosition().y-sprite->getContentSize().height));
	pPage->addChild(lpLable, 1);

	return true;
}

void FileFilter::scrollViewDidScroll( cocos2d::extension::CCScrollView* view )
{

}

void FileFilter::scrollViewDidZoom( cocos2d::extension::CCScrollView* view )
{

}

void FileFilter::scrollViewClick(const CCPoint &oOffset, const CCPoint &oPoint, CCNode *pPage, int nPage)
{
	if (_lsFile.size() > nPage)
	{
		CCLOG("Select file path is %s,index is %d", _lsFile[nPage].strFilePath.c_str(), nPage);
		CCDirector::sharedDirector()->replaceScene(MediaPlayScene::scene(_lsFile[nPage].strFilePath));
	}
}

void FileFilter::scrollViewScrollEnd(CCNode * pPage, int nPage )
{

}

void FileFilter::keyBackClicked()
{
	CCLOG("file filter back key clicked");
	CCDirector::sharedDirector()->end();
}

void FileFilter::keyMenuClicked()
{
	CCLOG("file filter menu key clicked");
	CCDirector::sharedDirector()->end();
}
