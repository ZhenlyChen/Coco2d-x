#pragma execution_character_set("utf-8")
#include "HitBrick.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()
// #define MUSIC

USING_NS_CC;
using namespace CocosDenshion;

void HitBrick::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* HitBrick::createScene() {
  srand((unsigned)time(NULL));
  auto scene = Scene::createWithPhysics();

  scene->getPhysicsWorld()->setAutoStep(true);

  // Debug 模式
  // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
  scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
  auto layer = HitBrick::create();
  layer->setPhysicsWorld(scene->getPhysicsWorld());
  layer->setJoint();
  scene->addChild(layer);
  return scene;
}

// on "init" you need to initialize your instance
bool HitBrick::init() {
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }
  visibleSize = Director::getInstance()->getVisibleSize();


  auto edgeSp = Sprite::create();  //创建一个精灵
  auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f), 3);  //edgebox是不受刚体碰撞影响的一种刚体，我们用它来设置物理世界的边界
  boundBody->setCategoryBitmask(0xFFFFFFFF);
  boundBody->setCollisionBitmask(0xFFFFFFFF);
  boundBody->setContactTestBitmask(0xF0000000);
  boundBody->setTag(11);
  edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //位置设置在屏幕中央
  edgeSp->setPhysicsBody(boundBody);

  addChild(edgeSp);

#ifdef MUSIC
  preloadMusic(); // 预加载音效
#endif // MUSIC

  addSprite();    // 添加背景和各种精灵
  addListener();  // 添加监听器 
  addPlayer();    // 添加球与板
  BrickGeneraetd();  // 生成砖块


  schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);

  onBall = true;

  spFactor = 0;
  return true;
}

// 关节连接，固定球与板子
// Todo - Doing
void HitBrick::setJoint() {
	joint1 = PhysicsJointPin::construct(player->getPhysicsBody(), ball->getPhysicsBody(), Vec2(0, 38), Vec2::ZERO);
	m_world->addJoint(joint1);
}



// 预加载音效
void HitBrick::preloadMusic() {
  auto sae = SimpleAudioEngine::getInstance();
  sae->preloadEffect("gameover.mp3");
  sae->preloadBackgroundMusic("bgm.mp3");
  sae->playBackgroundMusic("bgm.mp3", true);
}

// 添加背景和各种精灵
void HitBrick::addSprite() {
  // add background
  auto bgSprite = Sprite::create("bg.png");
  bgSprite->setPosition(visibleSize / 2);
  bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
  this->addChild(bgSprite, 0);


  // add ship
  ship = Sprite::create("ship.png");
  ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
  ship->setPosition(visibleSize.width / 2, 0);
  auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 1.0f));
  shipbody->setCategoryBitmask(0x0FFFFFFF);
  shipbody->setCollisionBitmask(0x0FFFFFFF);
  shipbody->setContactTestBitmask(0x0FFFFFFF);
  shipbody->setTag(1);
  shipbody->setDynamic(false);
  ship->setPhysicsBody(shipbody);
  this->addChild(ship, 1);

  // add sun and cloud
  auto sunSprite = Sprite::create("sun.png");
  sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
  this->addChild(sunSprite);
  auto cloudSprite1 = Sprite::create("cloud.png");
  cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite1);
  auto cloudSprite2 = Sprite::create("cloud.png");
  cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite2);
}

// 添加监听器
void HitBrick::addListener() {
  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// 创建角色
void HitBrick::addPlayer() {

  player = Sprite::create("bar.png");
  int xpos = visibleSize.width / 2;

  player->setScale(0.1f, 0.1f);
  player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height*0.1f));
  // 设置板的刚体属性
  // Todo - Done
  auto playerBody = PhysicsBody::createBox(player->getContentSize(), PhysicsMaterial(1000.0f, 1.0f, 0.0f));
  playerBody->setGravityEnable(true);
  playerBody->setCategoryBitmask(0xF000000F);
  playerBody->setContactTestBitmask(0xF0000000);
  playerBody->setDynamic(false);
  playerBody->setTag(20);
  player->setPhysicsBody(playerBody);

  this->addChild(player, 2);
  
  ball = Sprite::create("ball.png");
  ball->setPosition(Vec2(xpos, player->getPosition().y + ball->getContentSize().height*0.1f));
  ball->setScale(0.1f, 0.1f);
  // 设置球的刚体属性
  // Todo - Done
  auto ballBody = PhysicsBody::createCircle(ball->getContentSize().height / 2, PhysicsMaterial(0.1f, 1.0f, 0.0f));
  ballBody->setCollisionBitmask(1);
  ballBody->setCategoryBitmask(1);
  ballBody->setContactTestBitmask(1);
  ball->setPhysicsBody(ballBody);
  ball->setTag(2);


  addChild(ball, 3);
  
}

// 实现简单的蓄力效果
// Todo - Done
void HitBrick::update(float dt) {
	if (spHolded && spFactor < 800) {
		spFactor += 30;
	}
}




// Todo - Done
void HitBrick::BrickGeneraetd() {

for (int i = 0; i < 3; i++) {
	int cw = 0;
	while (cw <= visibleSize.width) {
		auto box = Sprite::create("box.png");
		// 为砖块设置刚体属性
		// Todo - Done
		auto physicsBody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(300.0f, 1.f, 0.0f));
		physicsBody->setGravityEnable(false);
		physicsBody->setCategoryBitmask(1);
		physicsBody->setCollisionBitmask(1);
		physicsBody->setContactTestBitmask(1);
		physicsBody->setGroup(-10);
		physicsBody->setTag(10);
		box->setPhysicsBody(physicsBody);
		box->setAnchorPoint(Vec2(0.0f, 1.0f));
		box->setPosition(Vec2(cw, visibleSize.height - i * box->getContentSize().height * 1.5));
		cw += box->getContentSize().width + 4;
		if (cw > visibleSize.width) break;
		addChild(box);
	}

 }

}


// 左右
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {

  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
  case cocos2d::EventKeyboard::KeyCode::KEY_A:
	  log("%f", player->getPosition().x);
	  if (player->getPosition().x < 55) return;
	  player->getPhysicsBody()->setVelocity(Vec2(-700, 0));
	  isLeft = true;
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
  case cocos2d::EventKeyboard::KeyCode::KEY_D:
	  log("%f", player->getPosition().x);
	  if (player->getPosition().x > 1145) return;
	  player->getPhysicsBody()->setVelocity(Vec2(700, 0));
	  isLeft = false;
    // 左右移动
    // Todo - Done
    break;

  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: // 开始蓄力
	  if (onBall) spHolded = true;
      break;
  default:
    break;
  }
}

// 释放按键
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	  if (isLeft) player->getPhysicsBody()->setVelocity(Vec2(0, 0));
	  break;
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	  if (!isLeft) player->getPhysicsBody()->setVelocity(Vec2(0, 0));
    // 停止运动
    // Todo - Done
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:   // 蓄力结束，小球发射
	  if (onBall) {
		  onBall = false;
		  spHolded = false;
		  m_world->removeJoint(joint1);
		  if (spFactor < 200) spFactor = 200;
		  ball->getPhysicsBody()->setVelocity(Vec2(0, spFactor));
		  ball->getPhysicsBody()->setGravityEnable(false);
		  spFactor = 0;
		  auto particle = ParticleGalaxy::create();
		  particle->setScale(3.0f);
		  particle->setPosition(player->getContentSize().width / 2, player->getContentSize().height / 2);
		  player->addChild(particle);

	  }
    break;

  default:
    break;
  }
}

// 碰撞检测
// Todo
bool HitBrick::onConcactBegin(PhysicsContact & contact) {
  auto c1 = contact.getShapeA(), c2 = contact.getShapeB();
  auto tag1 = c1->getBody()->getTag(), tag2 = c2->getBody()->getTag();
  log("%d, %d", tag1, tag2);
  if (tag1 == 1 || tag2 == 1) {
	  GameOver();
	  return false;
  }
  if (tag1 == 10) {
	  auto p = ParticleFlower::create();
	  p->setPosition(c1->getBody()->getPosition());
	  addChild(p);
	  p->runAction(Sequence::create(DelayTime::create(2.0f), RemoveSelf::create(true), nullptr));
	  c1->getBody()->getNode()->removeFromParentAndCleanup(true);
  }
  if (tag2 == 10) {
	  auto p = ParticleFlower::create();
	  p->setPosition(c2->getBody()->getPosition());
	  addChild(p);
	  p->runAction(Sequence::create(DelayTime::create(2.0f), RemoveSelf::create(true), nullptr));
	  c2->getBody()->getNode()->removeFromParentAndCleanup(true);
  }
  if (tag1 == 11 && tag2 == 20) {
	  c2->getBody()->setVelocity(Vec2::ZERO);
  }
  if (tag2 == 11 && tag1 == 20) {
	  c1->getBody()->setVelocity(Vec2::ZERO);
  }

  return true;
}


void HitBrick::GameOver() {

	_eventDispatcher->removeAllEventListeners();
	ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));

#ifdef MUSIC
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
	SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);
#endif // MUSIC


  auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
  label1->setColor(Color3B(0, 0, 0));
  label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
  this->addChild(label1);

  auto label2 = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 40);
  label2->setColor(Color3B(0, 0, 0));
  auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
  Menu* replay = Menu::create(replayBtn, NULL);
  replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(replay);

  auto label3 = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 40);
  label3->setColor(Color3B(0, 0, 0));
  auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
  this->addChild(exit);
}

// 继续或重玩按钮响应函数
void HitBrick::replayCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(HitBrick::createScene());
}

// 退出
void HitBrick::exitCallback(Ref * pSender) {
  Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}
