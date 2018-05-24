#include "GameScene.h"

USING_NS_CC;


std::default_random_engine randomEngine(time(NULL));
Scene* GameSence::createScene()
{
	return GameSence::create();
}

// on "init" you need to initialize your instance
bool GameSence::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto bg = Sprite::create("level-background-0.jpg");
    bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(bg, 0);

    stoneLayer = Layer::create();
    stoneLayer->setAnchorPoint(Vec2(0, 0));
    stoneLayer->setPosition(Vec2(0, 0));
    stoneLayer->setName("stoneLayer");

    stone = Sprite::create("stone.png");
    stone->setPosition(Vec2(560, 480));
    stoneLayer->addChild(stone, 1);

    mouseLayer = Layer::create();
    mouseLayer->setAnchorPoint(Vec2(0, 0));
    mouseLayer->setPosition(Vec2(0, visibleSize.height / 2));
    mouseLayer->setName("mouseLayer");

    mouse = Sprite::createWithSpriteFrameName("pulled-gem-mouse-0.png");
    Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
    mouse->runAction(RepeatForever::create(mouseAnimate));
    mouse->setPosition(visibleSize.width / 2, 0);
    mouse->setName("mouse");
    mouseLayer->addChild(mouse, 2);

    auto shootText = Label::createWithTTF("Shoot", "fonts/Marker Felt.ttf", 60);
    // this->addChild(shootText, 1);
    auto shootButton = MenuItemLabel::create(shootText, CC_CALLBACK_1(GameSence::shootMenuCallback, this));
    shootButton->setPosition(Vec2(visibleSize.width - 140, visibleSize.height - 150));
    auto menu = Menu::create(shootButton, NULL);
    menu->setPosition(Vec2::ZERO);
    
    this->addChild(menu, 1);
    this->addChild(stoneLayer, 2);
    this->addChild(mouseLayer, 2);

	return true;
}


Vec2 GameSence::getRandomVec2() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    std::uniform_real_distribution<float> disW(30.0, visibleSize.width - 60);
    std::uniform_real_distribution<float> disH(20.0, visibleSize.height - 200);
    return Vec2(disW(randomEngine), disH(randomEngine));
}

// cocos2d::MoveTo *moveMouse;
void GameSence::shootMenuCallback(cocos2d::Ref * pSender) {

    auto mouseLayer = getChildByName("mouseLayer");
    auto mouse = mouseLayer->getChildByName("mouse");
    auto mouseLoc = mouseLayer->convertToWorldSpace(mouse->getPosition());

    auto newStone = Sprite::create("stone.png");
    newStone->setPosition(Vec2(560, 480));
    auto stoneLayer = getChildByName("stoneLayer");
    auto moveStone = MoveTo::create(0.5f, stoneLayer->convertToNodeSpace(mouseLoc));
    auto fadeOut = FadeOut::create(1.0f);
    auto delay = DelayTime::create(0.25f);
    auto action = Sequence::create(moveStone, delay, fadeOut, RemoveSelf::create(), nullptr);
    newStone->runAction(action);
    stoneLayer->addChild(newStone, 1);
    
    auto newDiamond = Sprite::create("diamond.png");
    newDiamond->setPosition(mouseLoc);
    stoneLayer->addChild(newDiamond, 0);

	Animate* diamondAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("diamondAnimation"));
	newDiamond->runAction(RepeatForever::create(diamondAnimate));

    auto moveMouse = MoveTo::create(3.0f, mouseLayer->convertToNodeSpace(getRandomVec2()));
    auto moveEaseIn = EaseElasticOut::create(moveMouse);
    if (mouse->getActionByTag(634) != nullptr) {
        mouse->stopActionByTag(634);
    }
    if (mouse->getActionByTag(534) != nullptr) {
        mouse->stopActionByTag(534);
    }
    moveEaseIn->setTag(634);
    mouse->runAction(moveEaseIn);

}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {

	auto location = touch->getLocation();

    auto newCheese = Sprite::create("cheese.png");
    newCheese->setPosition(location); 
    auto fadeOut = FadeOut::create(5.0f);
    auto delay = DelayTime::create(1.0f);
    auto action = Sequence::create(delay, fadeOut, RemoveSelf::create(), nullptr);
    newCheese->runAction(action);
    this->addChild(newCheese, 1);

    auto mouseLayer = getChildByName("mouseLayer");
    auto mouse = mouseLayer->getChildByName("mouse");
    
    auto gotoCheese = MoveTo::create(5.0f, mouseLayer->convertToNodeSpace(Vec2(location)));
    auto gotoEase = EaseElasticOut::create(gotoCheese);
    if (mouse->getActionByTag(634) != nullptr) {
        mouse->stopActionByTag(634);
    }
    if (mouse->getActionByTag(534) != nullptr) {
        mouse->stopActionByTag(534);
    }
    gotoEase->setTag(534);
    mouse->runAction(gotoEase);

	return true;
}
