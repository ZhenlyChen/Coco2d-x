#include "Factory.h"

Factory* Factory::instance = nullptr;
Factory* Factory::getInstance() {
	if (instance == nullptr) {
		instance = new Factory();
	}
	return instance;
}

Sprite* Factory::createMonster() {
	Sprite* monster = Sprite::create("doge.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 200, 200)));
	monsters.pushBack(monster);
	return monster;
}