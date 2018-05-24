#include "MenuScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include <random>
#include <time.h>

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);

    auto goldMinerText = Sprite::create("gold-miner-text.png");
    goldMinerText->setAnchorPoint(Vec2(0.5, 0.5));
    goldMinerText->setPosition(Vec2(visibleSize.width / 2 + origin.x, 
                                    visibleSize.height - goldMinerText->getContentSize().height - 60));
    this->addChild(goldMinerText);

	auto face = Sprite::createWithSpriteFrameName("miner-face-whistle-0.png");
	Animate* faceAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("faceAnimation"));
	face->runAction(RepeatForever::create(faceAnimate));
	face->setPosition(200-37, 500-136);
	this->addChild(face,2);

    auto startGold = Sprite::create("menu-start-gold.png");
    startGold->setPosition(Vec2(visibleSize.width + origin.x - 207, origin.y + 143));
    this->addChild(startGold, 1);

    auto startButton = MenuItemImage::create("start-0.png", "start-1.png", 
        CC_CALLBACK_1(MenuScene::startMenuCallback, this));

    startButton->setPosition(Vec2(visibleSize.width + origin.x - 203, origin.y + 193));
    auto menu = Menu::create(startButton, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    return true;
}

void MenuScene::startMenuCallback(cocos2d::Ref * pSender){
    Director::getInstance()->replaceScene(TransitionFlipX::create(1, GameSence::createScene()));
}

