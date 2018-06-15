#include "Thunder.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;
#define OPEN false

Scene* Thunder::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Thunder::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Thunder::init() {
	if (!Layer::init()) {
		return false;
	}
	gameOver = false;
	stoneType = 0;
	isMove = false;  // 是否点击飞船
	visibleSize = Director::getInstance()->getVisibleSize();

	// 创建背景
	auto bgsprite = Sprite::create("bg.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
		visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	// 创建飞船
	player = Sprite::create("player.png");
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPosition(visibleSize.width / 2, player->getContentSize().height);
	player->setName("player");
	this->addChild(player, 1);

	// 显示陨石和子弹数量
	enemysNum = Label::createWithTTF("enemys: 0", "fonts/arial.TTF", 20);
	enemysNum->setColor(Color3B(255, 255, 255));
	enemysNum->setPosition(50, 60);
	this->addChild(enemysNum, 3);
	bulletsNum = Label::createWithTTF("bullets: 0", "fonts/arial.TTF", 20);
	bulletsNum->setColor(Color3B(255, 255, 255));
	bulletsNum->setPosition(50, 30);
	this->addChild(bulletsNum, 3);

	addEnemy(5);        // 初始化陨石
	preloadMusic();     // 预加载音乐
	playBgm();          // 播放背景音乐
	explosion();        // 创建爆炸帧动画

	// 添加监听器
	addTouchListener();
	addKeyboardListener();
	addCustomListener();

	// 调度器
	schedule(schedule_selector(Thunder::update), 0.04f, kRepeatForever, 0);

	return true;
}

//预加载音乐文件
void Thunder::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/explore.wav");
	audio->preloadEffect("music/fire.wav");
}

//播放背景音乐
void Thunder::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

//初始化陨石
void Thunder::addEnemy(int n) {
	enemys.clear();
	for (unsigned i = 0; i < 3; ++i) {
		char enemyPath[20];
		sprintf(enemyPath, "stone%d.png", 3 - i);
		double width = visibleSize.width / (n + 1.0),
			height = visibleSize.height - (50 * (i + 1));
		for (int j = 0; j < n; ++j) {
			auto enemy = Sprite::create(enemyPath);
			enemy->setAnchorPoint(Vec2(0.5, 0.5));
			enemy->setScale(0.5, 0.5);
			enemy->setPosition(width * (j + 1), height);
			enemys.push_back(enemy);
			addChild(enemy, 1);
		}
	}
}

// 陨石向下移动并生成新的一行(加分项)
void Thunder::newEnemy() {
	for (auto enemy : enemys) {
		auto moveAnimate = MoveBy::create(0.1f, Vec2(0, -50));
		enemy->runAction(moveAnimate);
		if (enemy->getPosition().y < 100) stopAc();
	}
	char enemyPath[20];
	stoneType++;
	sprintf(enemyPath, "stone%d.png", stoneType);
	if (stoneType > 2) stoneType = 0;
	double width = visibleSize.width / 6.0f,
		height = visibleSize.height - 50;
	for (int j = 0; j < 5; j++) {
		auto enemy = Sprite::create(enemyPath);
		enemy->setAnchorPoint(Vec2(0.5, 0.5));
		enemy->setScale(0.5, 0.5);
		enemy->setPosition(width * j + 77, height);
		enemys.push_back(enemy);
		addChild(enemy, 1);
	}
}

// 移动飞船
void Thunder::movePlane(char c) {
	float offset = 0;
	if (c == 'A') {
		offset = -20;
	}
	else {
		offset = 20;
	}
	auto pos = player->getPosition();
	if ((pos.x + offset < 50 && offset < 0)
		|| (pos.x + offset > visibleSize.width - 50 && offset > 0)) return;
	auto moveAnimate = MoveBy::create(0.03f, Vec2(offset, 0));
	player->runAction(moveAnimate);
}

//发射子弹
void Thunder::fire() {
	if (gameOver) return;
	auto bullet = Sprite::create("bullet.png");
	bullet->setAnchorPoint(Vec2(0.5, 0.5));
	bullets.push_back(bullet);
	bullet->setPosition(player->getPosition());
	addChild(bullet, 1);
#if OPEN == true
	SimpleAudioEngine::getInstance()->playEffect("music/fire.wav");
#endif
	// 移除飞出屏幕外的子弹
	auto shotAnimate = MoveTo::create(1.0f, Vec2(bullet->getPosition().x, visibleSize.height));
	list<Sprite*> *pBullets = &bullets;
	auto action = Sequence::create(
		shotAnimate,
		CallFunc::create([pBullets, bullet] {
			pBullets->remove(bullet);
			bullet->removeFromParentAndCleanup(true);
		}),
		nullptr);
	bullet->runAction(action);
}

// 切割爆炸动画帧
void Thunder::explosion() {
	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	int frameCount = 8;
	explore.reserve(frameCount);
	for (int i = 0; i < frameCount - 3; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(190 * i, 0, 190, 190)));
		explore.pushBack(frame);
	}
	for (int i = 5; i < frameCount; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(190 * i, 190, 190, 190)));
		explore.pushBack(frame);
	}
}

void Thunder::update(float f) {
	// 实时更新页面内陨石和子弹数量(不得删除)
	// 要求数量显示正确(加分项)
	char str[15];
	sprintf(str, "enemys: %d", enemys.size());
	enemysNum->setString(str);
	sprintf(str, "bullets: %d", bullets.size());
	bulletsNum->setString(str);

	// 飞船移动
	if (isMove)
		this->movePlane(movekey);

	static int ct = 0;
	static int dir = 4;
	++ct;
	if (ct == 120)
		ct = 40, dir = -dir;
	else if (ct == 80) {
		dir = -dir;
		newEnemy();  // 陨石向下移动并生成新的一行(加分项)
	}
	else if (ct == 20)
		ct = 40, dir = -dir;

	//陨石左右移动
	for (Sprite* s : enemys) {
		if (s != NULL) {
			s->setPosition(s->getPosition() + Vec2(dir, 0));
		}
	}

	// 分发自定义事件
	EventCustom e("meet");
	_eventDispatcher->dispatchEvent(&e);

}

// 自定义碰撞事件
void Thunder::meet(EventCustom * event) {
	list<Sprite*>* pEnemys = &enemys;
	list<Sprite*>* pBullets = &bullets;
	for (auto enemy : enemys) {
		if (enemy->getPosition().getDistance(player->getPosition()) < 50) stopAc();
		for (auto bullet : bullets) {
			if (enemy->getPosition().getDistance(bullet->getPosition()) < 25) {
#if OPEN == true
				SimpleAudioEngine::getInstance()->playEffect("music/explore.wav");
#endif
				enemy->runAction(
					Sequence::create(
						CallFunc::create([bullet, pBullets, enemy, pEnemys] {
							pEnemys->remove(enemy);
							try {
								bullet->removeFromParentAndCleanup(true);
								pBullets->remove(bullet);
							}
							catch(int i) {
								// 已经被移除
							}
						}),
						Animate::create(
							Animation::createWithSpriteFrames(explore, 0.05f, 1)
						),
						CallFunc::create([enemy, pEnemys] {
							enemy->removeFromParentAndCleanup(true);
						}),
						nullptr
					)
				);
			}
		}
	}
	// 判断子弹是否打中陨石并执行对应操作
}

void Thunder::stopAc() {
	auto gameOver = Sprite::create("gameOver.png");
	gameOver->setAnchorPoint(Vec2(0.5, 0.5));
	gameOver->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(gameOver, 2);
	this->unscheduleAllSelectors();
#if OPEN == true
	SimpleAudioEngine::getInstance()->playEffect("music/explore.wav");
#endif
	player->runAction(
		Animate::create(
			Animation::createWithSpriteFrames(explore, 0.05f, 1)
		)
	);
	this->gameOver = true;
}



// 添加自定义监听器
void Thunder::addCustomListener() {
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	this->getEventDispatcher()->addEventListenerWithFixedPriority(meetListener, 1);
}

// 添加键盘事件监听器
void Thunder::addKeyboardListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		fire();
		break;
	}
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
		if (movekey == 'A') isMove = false;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		if (movekey == 'D') isMove = false;
		break;
	}
}

// 添加触摸事件监听器
void Thunder::addTouchListener() {
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// 鼠标点击发射炮弹
bool Thunder::onTouchBegan(Touch *touch, Event *event) {
	if (gameOver) return true;
	if (touch->getLocation().getDistance(player->getPosition()) <= 30)
		isClick = true;
	else
		fire();
	return true;
}

void Thunder::onTouchEnded(Touch *touch, Event *event) {
	isClick = false;
}

// 当鼠标按住飞船后可控制飞船移动 (加分项)
void Thunder::onTouchMoved(Touch *touch, Event *event) {
	if (isClick == true && gameOver == false) {
		if (touch->getLocation().x < 0 || touch->getLocation().x > visibleSize.width) return;
		player->setPosition(Vec2(touch->getLocation().x, player->getPosition().y));
	}
}
