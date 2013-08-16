#ifndef __GAMESCROLLVIEWDELEGATE_H__
#define __GAMESCROLLVIEWDELEGATE_H__
#include <cocos-ext.h>

class CCCGameScrollViewDelegate : public cocos2d::extension::CCScrollViewDelegate
{
public:
	CCCGameScrollViewDelegate(void);
	virtual ~CCCGameScrollViewDelegate(void);
public:
	//��ʼ��ÿ������Page�Ļص�
	virtual bool scrollViewInitPage(cocos2d::CCNode* pPage,int nPage)=0;
	//���һ��Page�Ļص�
	virtual void scrollViewClick(const cocos2d::CCPoint& oOffset,const cocos2d::CCPoint & oPoint ,cocos2d::CCNode * pPage,int nPage )=0;
	//ÿһ���л�Page�Ļص�
	virtual void scrollViewScrollEnd(cocos2d::CCNode * pPage,int nPage)=0;
};

#endif