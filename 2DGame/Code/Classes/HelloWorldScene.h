#pragma once
#include "cocos2d.h"
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

	void playerMove(int offsetX, int offsetY);

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	int dtime;
	cocos2d::ProgressTimer* pT;
};
