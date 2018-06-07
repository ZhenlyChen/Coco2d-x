#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "sqlite3.h"
#include <iostream>
#pragma execution_character_set("utf-8")

#define database	UserDefault::getInstance()
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
	if (!Scene::init())
	{
		return false;
	}
	over = false;
	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();
	Director::getInstance()->setDisplayStats(false);

	//根据文件路径快速导入瓦片地图
	tmx = TMXTiledMap::create("coco.tmx");

	//设置位置
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);

	//设置锚点
	tmx->setAnchorPoint(Vec2(0.5, 0.5));

	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	addChild(tmx, 0);

	objs = tmx->getObjectGroup("wall");


	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(tmx->convertToNodeSpace(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2)));
	// 地图内的第三层
	tmx->addChild(player, 1);

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

	auto buttonX = MenuItemLabel::create(labelX, CC_CALLBACK_1(HelloWorld::buttonXYCallBack, this, 1));
	auto buttonY = MenuItemLabel::create(labelY, CC_CALLBACK_1(HelloWorld::buttonXYCallBack, this, 0));
	auto buttonW = MenuItemLabel::create(labelW, CC_CALLBACK_1(HelloWorld::buttonWASDCallBack, this, 0));
	auto buttonA = MenuItemLabel::create(labelA, CC_CALLBACK_1(HelloWorld::buttonWASDCallBack, this, 1));
	auto buttonS = MenuItemLabel::create(labelS, CC_CALLBACK_1(HelloWorld::buttonWASDCallBack, this, 2));
	auto buttonD = MenuItemLabel::create(labelD, CC_CALLBACK_1(HelloWorld::buttonWASDCallBack, this, 3));

	buttonX->setPosition(Vec2(visibleSize.width - 100, 30));
	buttonY->setPosition(Vec2(visibleSize.width - 60, 80));
	buttonW->setPosition(Vec2(100, 80));
	buttonA->setPosition(Vec2(50, 30));
	buttonS->setPosition(Vec2(100, 30));
	buttonD->setPosition(Vec2(150, 30));

	auto menu = Menu::create(buttonX, buttonY, buttonW, buttonA, buttonS, buttonD, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	// 倒计时
	dtime = 180;
	char dTimeStr[30];
	sprintf(dTimeStr, "%d", dtime);
	time = Label::createWithTTF(dTimeStr, "fonts/arial.ttf", 36);
	time->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 100));
	this->addChild(time, 1);

	// 击杀个数
	char dKillStr[30];
	sprintf(dKillStr, "%d", killCount);
	kill = Label::createWithTTF(dKillStr, "fonts/arial.ttf", 36);
	kill->setColor(Color3B(255, 0, 0));
	kill->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 50));
	this->addChild(kill, 1);

	// 最高得分
	char dHighStr[30];
	//数据库指针
	sqlite3* pdb = nullptr;
	//数据库路径
	std::string path = FileUtils::getInstance()->getWritablePath() + "save.db";
	//根据路径path打开或创建数据库
	int result = sqlite3_open(path.c_str(), &pdb);
	//若成功result等于SQLITE_OK
	if (result == SQLITE_OK) {
		char **re;//查询结果
		int row, col;//行、列
		sqlite3_get_table(pdb, "select * from score;", &re, &row, &col, NULL);
		if (row == 0) {
			std::string sql = "create table score(ID int primary key not null, number int);";
			sqlite3_exec(pdb, sql.c_str(), nullptr, nullptr, nullptr);
			highScore = 0;
		}
		else {
			highScore = atoi(re[3]);
		}
	} else {
		highScore = 0;
	}
	// highScore = database->getIntegerForKey("killCount");
	sprintf(dHighStr, "High Score: %d", highScore);
	LabelHigh = Label::createWithTTF(dHighStr, "fonts/arial.ttf", 20);
	LabelHigh->setColor(Color3B(255, 255, 255));
	LabelHigh->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 15));
	this->addChild(LabelHigh, 1);

	// 更新时间
	this->schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);

	// 添加怪物
	this->schedule(schedule_selector(HelloWorld::addMonster), 2.0f);

	// 判断碰撞
	this->schedule(schedule_selector(HelloWorld::judge), 0.1f);

	// 键盘事件
	auto eventListener = EventListenerKeyboard::create();

	eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			quickMoveA(0);
			this->schedule(schedule_selector(HelloWorld::quickMoveA), 0.16f);
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			quickMoveD(0);
			this->schedule(schedule_selector(HelloWorld::quickMoveD), 0.16f);
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:
			quickMoveW(0);
			this->schedule(schedule_selector(HelloWorld::quickMoveW), 0.16f);
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:
			quickMoveS(0);
			this->schedule(schedule_selector(HelloWorld::quickMoveS), 0.16f);
			break;
		case EventKeyboard::KeyCode::KEY_J:
		case EventKeyboard::KeyCode::KEY_X:
			buttonXYCallBack(nullptr, 1);
			break;
		case EventKeyboard::KeyCode::KEY_K:
		case EventKeyboard::KeyCode::KEY_Y:
			buttonXYCallBack(nullptr, 0);
			break;
		}
	};

	eventListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			this->unschedule(schedule_selector(HelloWorld::quickMoveA));
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			this->unschedule(schedule_selector(HelloWorld::quickMoveD));
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:
			this->unschedule(schedule_selector(HelloWorld::quickMoveW));
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:
			this->unschedule(schedule_selector(HelloWorld::quickMoveS));
			break;
		}
	};

	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, player);

	return true;
}

// 连续移动调度函数
void HelloWorld::quickMoveW(float dt) {
	playerMove(0);
}
void HelloWorld::quickMoveA(float dt) {
	playerMove(1);
}
void HelloWorld::quickMoveS(float dt) {
	playerMove(2);
}
void HelloWorld::quickMoveD(float dt) {
	playerMove(3);
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
	playerMove(dir);
}

void HelloWorld::playerMove(int dir) {
	if (over) return;
	int offsetX = offset[dir][0];
	int offsetY = offset[dir][1];
	auto postion = tmx->convertToWorldSpace(player->getPosition());

	// 防止攻击动作与移动同时发生
	if ((player->getActionByTag(534) != nullptr && !(player->getActionByTag(534)->isDone()))) return;
	// 防止人物移动到错误的地方
	if ((postion.x + offsetX < 35 && offsetX < 0) ||
		(postion.x + offsetX > visibleSize.width - 35 && offsetX > 0) ||
		(postion.y + offsetY < 90 && offsetY < 0) ||
		(postion.y + offsetY > visibleSize.height - 35 && offsetY > 0)
		// || (player->getActionByTag(634) != nullptr && !(player->getActionByTag(634)->isDone()))
		|| isToWall(Vec2(postion.x + offsetX, postion.y + offsetY))
		) {
		offsetX = 0;
		offsetY = 0;
	}
	// 反转人物
	if (dir == 1 && ldir != 1) {
		player->setFlipX(true);
		ldir = dir;
	}
	else if (dir == 3 && ldir != 3) {
		player->setFlipX(false);
		ldir = dir;
	}
	// 执行移动动画
	auto animation = Animation::createWithSpriteFrames(run, 0.02f);
	auto move = MoveBy::create(0.16f, Vec2(offsetX, offsetY));
	auto action = Spawn::createWithTwoActions(Animate::create(animation), move);
	action->setTag(634);
	player->runAction(action);
}

// 判断坐标是否在墙内
bool HelloWorld::isToWall(Vec2 loc) {
	// 从对象层中获取对象数组
	ValueVector container = objs->getObjects();
	// 遍历对象
	for (auto obj : container) {
		ValueMap values = obj.asValueMap();
		// 获取纵横轴坐标（cocos2dx坐标）  
		int x = values.at("x").asInt() - 10;
		int y = values.at("y").asInt() - 10;
		int w = values.at("width").asInt() + 20;
		int h = values.at("height").asInt() + 20;
		auto wloc = tmx->convertToNodeSpace(loc);
		if ((wloc.x > x && wloc.x < x + w) &&
			(wloc.y > y && wloc.y < y + h)) {
			return true;
		}
	}
	return false;
}

void HelloWorld::buttonXYCallBack(Ref* pSender, int type) {
	if (over) return;

	// 防止重复发生
	if (// (player->getActionByTag(634) != nullptr && !(player->getActionByTag(634)->isDone())) ||
		(player->getActionByTag(534) != nullptr && !(player->getActionByTag(534)->isDone()))) return;

	// 根据方向确定攻击范围
	Rect playerRect = player->getBoundingBox();
	Rect attackRect;
	if (ldir == 1) {
		attackRect = Rect(playerRect.getMinX() - 60, playerRect.getMinY(),
			playerRect.getMaxX() - playerRect.getMinX() + 70,
			playerRect.getMaxY() - playerRect.getMinY());
	}
	else {
		attackRect = Rect(playerRect.getMinX() - 10, playerRect.getMinY(),
			playerRect.getMaxX() - playerRect.getMinX() + 70,
			playerRect.getMaxY() - playerRect.getMinY());
	}
	// 消灭指定范围内的所有怪物
	auto fac = Factory::getInstance();
	auto sp = fac->collider(attackRect);
	while (sp != nullptr) {
		fac->removeMonster(sp);
		addHp(5);
		sp = fac->collider(attackRect);
		killCount++;
		char dKillStr[30];
		sprintf(dKillStr, "%d", killCount);
		kill->setString(dKillStr);
	}

	// 攻击动画动作
	auto callbackShock = CallFunc::create([&]() {
		auto delay = DelayTime::create(0.5f);
		auto upBG = MoveBy::create(0.03f, Vec2(0, -5));
		auto downBG = MoveBy::create(0.03f, Vec2(0, 5));
		auto updown = Sequence::create(delay, upBG, downBG, nullptr);
		tmx->runAction(updown);
	});
	auto idleAnimation = Animation::createWithSpriteFrames(idle, 0.1f);
	auto animation = Animation::createWithSpriteFrames(attack, 0.05f);
	auto action = Sequence::create(callbackShock, Animate::create(animation), Animate::create(idleAnimation), nullptr);
	action->setTag(534);
	player->runAction(action);
}

void HelloWorld::addHp(int val) {
	schedule(schedule_selector(HelloWorld::addProgress), 0.03f, val, 0);
}

void HelloWorld::addProgress(float dt) {
	if (pT->getPercentage() <= 99) pT->setPercentage(pT->getPercentage() + 1);
}


void HelloWorld::subHp(int val) {
	schedule(schedule_selector(HelloWorld::subProgress), 0.03f, val, 0);
}

void HelloWorld::subProgress(float dt) {
	if (pT->getPercentage() >= 1) {
		pT->setPercentage(pT->getPercentage() - 1);
	}
	else {
		// 游戏结束
		this->unscheduleAllSelectors();
		time->setString("GameOver!");
		over = true;
		if (killCount > highScore) {
			//数据库指针
			sqlite3* pdb = nullptr;
			//数据库路径
			std::string path = FileUtils::getInstance()->getWritablePath() + "save.db";
			//根据路径path打开或创建数据库
			int result = sqlite3_open(path.c_str(), &pdb);
			//若成功result等于SQLITE_OK
			if (result == SQLITE_OK) {
				char sql[50] = "delete from score where id=1;";
				int rc = sqlite3_exec( pdb, sql, nullptr, nullptr, nullptr);
				sprintf(sql, "insert into score values(1,'%d');", killCount);
				rc = sqlite3_exec(pdb, sql, nullptr, nullptr, nullptr);
			}
			database->setIntegerForKey("killCount", killCount);
			char dHighStr[30];
			sprintf(dHighStr, "High Score: %d", killCount);
			LabelHigh->setString(dHighStr);
		}
		auto animation = Animation::createWithSpriteFrames(dead, 0.1f);
		player->runAction(Animate::create(animation));
	}

}


void HelloWorld::addMonster(float dt) {
	auto fac = Factory::getInstance();
	// 允许场上最多的怪物数量
	if (fac->getCount() > 20) return;
	for (int i = 0; i < 3; i++) {
		auto m = fac->createMonster();
		auto loc = Vec2(random(origin.x, visibleSize.width), 
						random(origin.x, visibleSize.height));
		m->setPosition(tmx->convertToNodeSpace(loc));
		tmx->addChild(m, 1);
	}
	fac->moveMonster(player->getPosition(), 0.5f);
}

void HelloWorld::judge(float dt) {
	auto fac = Factory::getInstance();
	Rect playerRect = player->getBoundingBox();
	auto sp = fac->collider(playerRect);
	if (sp != nullptr) {
		fac->removeMonster(sp);
		subHp(10);
	}
}