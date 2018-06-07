#pragma once
#include "cocos2d.h"
using namespace cocos2d;


class Factory
{
public:
	static Factory* getInstance();
	Sprite* createMonster();
private:
	static Factory* instance;
	Vector<Sprite*> monsters;
};