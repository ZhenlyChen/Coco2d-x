#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	Director::getInstance()->setDisplayStats(false);

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	auto texture1 = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	int frameCount = 22;
	dead.reserve(frameCount);
	for (int i = 0; i < frameCount; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	frameCount = 8;
	run.reserve(frameCount);
	for (int i = 0; i < frameCount; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}


	// 控制按钮
	auto labelX = Label::createWithTTF("X", "fonts/arial.ttf", 36);
	auto labelY = Label::createWithTTF("Y", "fonts/arial.ttf", 36);

	auto labelW = Label::createWithTTF("W", "fonts/arial.ttf", 36);
	auto labelA = Label::createWithTTF("A", "fonts/arial.ttf", 36);
	auto labelS = Label::createWithTTF("S", "fonts/arial.ttf", 36);
	auto labelD = Label::createWithTTF("D", "fonts/arial.ttf", 36); 
	

	auto buttonX = MenuItemLabel::create(labelX, CC_CALLBACK_1(HelloWorld::buttonXYCallBack, this, 0));
	auto buttonY = MenuItemLabel::create(labelY, CC_CALLBACK_1(HelloWorld::buttonXYCallBack, this, 1));
	auto buttonW = MenuItemLabel::create(labelW, CC_CALLBACK_1(HelloWorld::buttonWASDCallBack, this, 0));
	auto buttonA = MenuItemLabel::create(labelA, CC_CALLBACK_1(HelloWorld::buttonWASDCallBack, this, 1));
	auto buttonS = MenuItemLabel::create(labelS, CC_CALLBACK_1(HelloWorld::buttonWASDCallBack, this, 2));
	auto buttonD = MenuItemLabel::create(labelD, CC_CALLBACK_1(HelloWorld::buttonWASDCallBack, this, 3));

	buttonX->setPosition(Vec2(visibleSize.width - 140, 50));
	buttonY->setPosition(Vec2(visibleSize.width - 100, 80));
	buttonW->setPosition(Vec2(100, 80));
	buttonA->setPosition(Vec2(70, 50));
	buttonS->setPosition(Vec2(100, 50));
	buttonD->setPosition(Vec2(130, 50));

	auto menu = Menu::create(buttonX, buttonY, buttonW, buttonA, buttonS, buttonD, NULL);
	menu->setPosition(Vec2::ZERO);

	this->addChild(menu, 1);

	// 调度器
	dtime = 180;
	char dTimeStr[30];
	sprintf(dTimeStr, "%d", dtime);
	time = Label::createWithTTF(dTimeStr, "fonts/arial.ttf", 36);
	time->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 100));
	this->addChild(time, 1);

	this->schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);
	
	// 键盘事件
	auto eventListener = EventListenerKeyboard::create();

	eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			playerMove(-10, 0);
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			playerMove(10, 0);
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:
			playerMove(0, 10);
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:
			playerMove(0, -10);
			break;
		}
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, player);

    return true;
}


void HelloWorld::updateTime(float dt) {
	if (dtime > 0) {
		dtime = dtime - 1;
		char dTimeStr[30];
		sprintf(dTimeStr, "%d", dtime);
		time->setString(dTimeStr);
	}
	else {
		this->unschedule(schedule_selector(HelloWorld::updateTime));
	}
}

void HelloWorld::buttonWASDCallBack(Ref* pSender, int dir) {
	float offsetX = 0;
	float offsetY = 0;
	switch (dir) {
	case 0:
		offsetY = 10;
		break;
	case 1:
		offsetX = -10;
		break;
	case 2:
		offsetY = -10;
		break;
	case 3:
		offsetX = 10;
		break;
	}
	playerMove(offsetX, offsetY);
}

void HelloWorld::playerMove(int offsetX, int offsetY) {
	auto postion = player->getPosition();
	if (postion.x + offsetX < 20 || postion.x + offsetX > visibleSize.width - 20 ||
		postion.y + offsetY < 30 || postion.y + offsetY > visibleSize.height - 30 ||
		(player->getActionByTag(634) != nullptr && !(player->getActionByTag(634)->isDone())) ||
		(player->getActionByTag(534) != nullptr && !(player->getActionByTag(534)->isDone()))) return;
	auto animation = Animation::createWithSpriteFrames(run, 0.05f);
	auto move = MoveBy::create(0.4f, Vec2(offsetX, offsetY));
	auto action = Spawn::createWithTwoActions(Animate::create(animation), move);
	action->setTag(634);
	player->runAction(action);
}

void HelloWorld::buttonXYCallBack(Ref* pSender, int type) {
	// 防止重复发生
	if ((player->getActionByTag(634) != nullptr && !(player->getActionByTag(634)->isDone())) ||
		(player->getActionByTag(534) != nullptr && !(player->getActionByTag(534)->isDone()))) return;
	Animation* animation;
	if (type == 0) {
		animation = Animation::createWithSpriteFrames(dead, 0.1f);
		if (pT->getPercentage() >= 10) pT->setPercentage(pT->getPercentage() - 10);
	} else {
		animation = Animation::createWithSpriteFrames(attack, 0.1f);
		if (pT->getPercentage() <= 90) pT->setPercentage(pT->getPercentage() + 10);
	}
	auto idleAnimation = Animation::createWithSpriteFrames(idle, 0.1f);
	auto action = Sequence::create(Animate::create(animation), Animate::create(idleAnimation), nullptr);
	action->setTag(534);
	player->runAction(action);
}
