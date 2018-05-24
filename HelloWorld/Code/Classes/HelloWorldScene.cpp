#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* HelloWorld::createScene() {
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename) {
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init() {
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
        float y = origin.y + closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
    }

    // add a "about" label to show about info.
    auto labelAbout = Label::createWithTTF("Show", "fonts/Marker Felt.ttf", 24);
    auto aboutItem = MenuItemLabel::create(labelAbout,
                                            CC_CALLBACK_1(HelloWorld::menuAboutCallback, this));
    if (labelAbout == nullptr || aboutItem == nullptr) {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width - aboutItem->getContentSize().width;
        float y = origin.y + aboutItem->getContentSize().height / 2 + 8;
        aboutItem->setPosition(Vec2(x, y));
    }


    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, aboutItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...



    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("avatar.png");
    if (sprite == nullptr) {
        problemLoading("'avatar.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        sprite->setScale(0.5);
        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }

    // create and initialize a label

    // get string from xml
    auto *chnStrings = Dictionary::createWithContentsOfFile("Strings.xml");
    const char *strName = ((String*)chnStrings->objectForKey("name"))->getCString();
    const char *strNo = ((String*)chnStrings->objectForKey("no"))->getCString();

    // add a label shows my number
    auto labelNo = Label::createWithTTF(strNo, "fonts/Marker Felt.ttf", 24);
    if (labelNo == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        labelNo->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - labelNo->getContentSize().height - 50));
        labelNo->enableShadow();
        labelNo->enableOutline(Color4B::WHITE, 1);

        // add the label as a child to this layer
        this->addChild(labelNo, 1);
    }

    // add a label shows my name
    auto labelName = Label::createWithSystemFont(strName, "Microsoft YaHei UI", 24);
    if (labelName == nullptr) {
        problemLoading("'Microsoft YaHei UI'");
    }
    else
    {
        // position the label on the center of the screen
        labelName->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - labelNo->getContentSize().height - labelName->getContentSize().height - 50));
        labelName->enableGlow(Color4B::YELLOW);
        labelName->setColor(Color3B::RED);

        // add the label as a child to this layer
        this->addChild(labelName, 1);
    }

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender) {
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}


void HelloWorld::menuAboutCallback(Ref* pSender) {

    auto remove = getChildByName("emitter");
    if (remove != nullptr) {
        removeChild(remove);
    }

    auto emitter = ParticleFireworks::create();
    emitter->setName("emitter");

    addChild(emitter, 10);
}
