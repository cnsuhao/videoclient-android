#ifndef __FILEFILTER_H_
#define __FILEFILTER_H_
#include "CCGameScrollViewDelegate.h"
#include "CCGameScrollView.h"
#include <cocos2d.h>
#include <vector>
#include <string>

typedef struct ST_FileInfo
{
	std::string strFileName;
	std::string strFilePath;
}FileInfo, *LPFileInfo;

class FileFilter : public cocos2d::CCLayer, public CCCGameScrollViewDelegate
{
public:
	FileFilter(void);
	virtual ~FileFilter(void);

	static cocos2d::CCScene* scene();
	virtual bool init();
	void cancel();

	virtual void keyBackClicked();
	virtual void keyMenuClicked();

	CREATE_FUNC(FileFilter);

protected:
	virtual void scrollViewDidScroll( cocos2d::extension::CCScrollView* view );
	virtual void scrollViewDidZoom( cocos2d::extension::CCScrollView* view );
	virtual bool scrollViewInitPage( cocos2d::CCNode* pPage,int nPage );
	virtual void scrollViewClick( const cocos2d::CCPoint& oOffset,const cocos2d::CCPoint & oPoint ,cocos2d::CCNode * pPage,int nPage );
	virtual void scrollViewScrollEnd( cocos2d::CCNode * pPage,int nPage );

protected:
	CCCGameScrollView*			_ScrollView;
	std::vector<FileInfo>		_lsFile;
};

#endif