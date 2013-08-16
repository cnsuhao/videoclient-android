#include "HelloWorldScene.h"
#include "FileFilter.h"

USING_NS_CC;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    // 3. Add add a splash screen, show the cocos2d splash image.
    CCSprite* pSprite = CCSprite::create("bg.png");
    pSprite->setPosition(ccp(visibleSize.width/2, visibleSize.height/2));
	this->addChild(pSprite, 0);

    // Add the sprite to HelloWorld layer as a child layer.
	this->scheduleOnce(SEL_SCHEDULE(&HelloWorld::FileFilterScene), 1.0);
    
    return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::FileFilterScene(float dt)
{
	CCDirector::sharedDirector()->replaceScene(FileFilter::scene());
}