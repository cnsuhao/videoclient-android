#ifndef __GAMESCROLLVIEWDELEGATE_H__
#define __GAMESCROLLVIEWDELEGATE_H__
#include <cocos-ext.h>

class CCCGameScrollViewDelegate : public cocos2d::extension::CCScrollViewDelegate
{
public:
	CCCGameScrollViewDelegate(void);
	virtual ~CCCGameScrollViewDelegate(void);
public:
	//初始化每个单独Page的回调
	virtual bool scrollViewInitPage(cocos2d::CCNode* pPage,int nPage)=0;
	//点击一个Page的回调
	virtual void scrollViewClick(const cocos2d::CCPoint& oOffset,const cocos2d::CCPoint & oPoint ,cocos2d::CCNode * pPage,int nPage )=0;
	//每一次切换Page的回调
	virtual void scrollViewScrollEnd(cocos2d::CCNode * pPage,int nPage)=0;
};

#endif