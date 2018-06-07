#pragma once
#include "cocos2d.h"
#include "Monster.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	/// <param name='dir'>方向： 0 - w, 1 - a, 2 - s, 3 - d</param>  
	virtual void buttonWASDCallBack(Ref* pSender, int dir);

	/// <param name='type'>类型 0 - x, 1 - y</param>
	virtual void buttonXYCallBack(Ref* pSender, int type);

	void updateTime(float dt);

	void addProgress(float dt);

	void subProgress(float dt);

	void playerMove(int dir);

	void addMonster(float dt);

	void addHp(int val);

	void subHp(int val);

	void judge(float dt);

	void quickMoveW(float dt);
	void quickMoveA(float dt);
	void quickMoveS(float dt);
	void quickMoveD(float dt);

	bool isToWall(Vec2 loc);

	// implement the "static create()" method manually
	CREATE_FUNC(HelloWorld);
private:
	// wasd
	int offset[4][2] = { { 0, 20 },{ -20, 0 },{ 0, -20 },{ 20, 0 } };
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	
	cocos2d::Size visibleSize;
	cocos2d::TMXObjectGroup* objs;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* kill;
	cocos2d::Label* LabelHigh;
	int dtime;
	cocos2d::ProgressTimer* pT;
	int ldir;
	TMXTiledMap* tmx;
	int moveDir;
	int killCount;
	bool over;
	int highScore;
};


