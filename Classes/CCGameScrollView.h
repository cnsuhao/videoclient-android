#ifndef __GAMESCROLLVIEW_H__
#define __GAMESCROLLVIEW_H__
#include <cocos-ext.h>
#include "CCGameScrollViewDelegate.h"

// 校正滑动动画速度
#define ADJUST_ANIM_VELOCITY 2000
class CCCGameScrollView : public cocos2d::extension::CCScrollView
{
public:
	CCCGameScrollView(void);
	virtual ~CCCGameScrollView(void);
public:
	CREATE_FUNC(CCCGameScrollView);

	bool init();

	bool createContainer(CCCGameScrollViewDelegate* pDele, int nCount,const cocos2d::CCSize & oSize );

	virtual bool ccTouchBegan( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent );

	virtual void ccTouchMoved( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent );

	virtual void ccTouchEnded( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent );

	virtual void ccTouchCancelled( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent );

	void scrollToPage(int nPage);
	void scrollToNextPage();
	void scrollToPrePage();

	int getCurPage();
protected:
	void adjustScrollView(const cocos2d::CCPoint& oBegin,const cocos2d::CCPoint & oEnd);

	virtual void onScrollEnd(float fDelay);
protected:
	int m_nPageCount;
	int m_nPrePage;
	cocos2d::CCPoint m_BeginOffset;
	cocos2d::CCSize m_CellSize;
	float m_fAdjustSpeed;
};
#endif
