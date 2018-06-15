这个星期Cocos是做一个并没有黄金矿工的黄金矿工。

首先这个提供了一个Demo，和素材，这样做起来就十分地舒服

## 开始场景

这一部分需要添加三个东西进去，分别是标题，大石头和开始按钮

标题和大石头都是普通的Sprite，因此用最普通的方法，计算好他们的坐标，然后添加进去就可以了。

然后开始按钮要求是按下是有一定的交互的，因此可以使用`MenuItemImage`来实现

```cpp
auto startButton = MenuItemImage::create("start-0.png", "start-1.png", 
                                         CC_CALLBACK_1(MenuScene::startMenuCallback, this));
startButton->setPosition(Vec2(visibleSize.width + origin.x - 203, origin.y + 193));
auto menu = Menu::create(startButton, NULL);
menu->setPosition(Vec2::ZERO);
this->addChild(menu, 1);
```

这些都是十分常规的操作。

然后切换场景也是十分地简单， 只需要调用`Director`的实例的对应的`replaceScene`方法，就可以完成了。

甚至我们还可以加一些特效。`TranstionFlipX` 就是以x轴为中心平面式地旋转切换。

```cpp
void MenuScene::startMenuCallback(cocos2d::Ref * pSender){
    Director::getInstance()->replaceScene(TransitionFlipX::create(1, GameSence::createScene()));
}
```

然后就可以得出以下效果

![1526567021405](Report.assets\1526567021405.png)





## 游戏场景

首先就是要布置场景，需要放置的有一只会动的老鼠，一块石头，一个背景还有一个Shoot的菜单按钮

背景和石头都是普通的Sprite， Shoot和上一个场景的Start是类似的，不过换成了一个`Label`而已，代码也是大同小异的，重要的是那只会动的老鼠。

不过这部分需要使用到两个`Layer`， 这就需要额外设置一下锚点和位置，只要调用`setAnchorPoint`和`setPosition`方法就可以了。

至于这只老鼠，我们可以仿照在上一个场景中那个抖脚矿工的动画，在`AppDelegate.cpp`中加载`plist`资源，然后把动画存在`AnimationCache`当中，也是很容易就搞定了。

```cpp
SpriteFrameCache::getInstance()->addSpriteFramesWithFile("level-sheet.plist");
char mouseFrames = 7;
char mouseFramesName[30];
Animation* mouseAnimation = Animation::create();

for (int i = 0; i < mouseFrames; i++) {
    sprintf(mouseFramesName, "pulled-gem-mouse-%d.png", i);
    mouseAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(mouseFramesName));
}
mouseAnimation->setDelayPerUnit(0.1);
AnimationCache::getInstance()->addAnimation(mouseAnimation, "mouseAnimation");
```

然后在`GameScene.cpp`中加入

```cpp
auto mouse = Sprite::createWithSpriteFrameName("pulled-gem-mouse-0.png");
Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
mouse->runAction(RepeatForever::create(mouseAnimate));
mouse->setPosition(visibleSize.width / 2, 0);
mouse->setName("mouse");
mouseLayer->addChild(mouse, 2);
```

然后就可以得到这个场景了：

![1526567037775](Report.assets\1526567037775.png)

然后触摸事件里面添加一些方法，使得点击的地方生成一个奶酪并且老鼠会移动过去。

因为我的老鼠是在`mouseLayer`里面的，这就涉及到了局部坐标和世界坐标的转换。

这个只需要调用对应`layer`的`convertToNodeSpace`方法就可以了

```cpp
auto gotoCheese = MoveTo::create(5.0f, mouseLayer->convertToNodeSpace(Vec2(location)));
```

而奶酪需要出现后等待一定时候后淡出，而且还需要移除，因此我们可以使用一个`Sequence`来实现

```cpp
auto newCheese = Sprite::create("cheese.png");
newCheese->setPosition(location); 
auto fadeOut = FadeOut::create(5.0f);
auto delay = DelayTime::create(1.0f);
auto action = Sequence::create(delay, fadeOut, RemoveSelf::create(), nullptr);
newCheese->runAction(action);
this->addChild(newCheese, 1);
```

然后就可以完成这个部分

![1526567373987](Report.assets\1526567373987.png)

最后需要发射石头到老鼠位置，并且老鼠留下钻石并随机逃跑。

这个和上面老鼠移动到奶酪位置的做法是差不多的，不过这个需要生成一个指定范围的随机坐标。

这个使用`C++11`中的`Random`库就可以搞定

```cpp
std::default_random_engine randomEngine(time(NULL));
Vec2 GameSence::getRandomVec2() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    std::uniform_real_distribution<float> disW(30.0, visibleSize.width - 60);
    std::uniform_real_distribution<float> disH(20.0, visibleSize.height - 200);
    return Vec2(disW(randomEngine), disH(randomEngine));
}
```

我这里加了限制使得老鼠逃离的地方在泥土里面。

然后就可以实现最后的功能了。

![1526567393546](Report.assets\1526567393546.png)

这个项目做到这里基本就完成了，不过还有一个小问题，就是如果你在老鼠移动未完成的时候再次执行移动的动作，那么老鼠最终到达的位置就不会是指定的位置，这是因为他的移动未完成之前，再次调用移动就会导致新的移动的坐标叠加旧的移动的目标地址上，也就是说，如果在一个地方同时点击两次，那么老鼠就会移动两倍的距离，这是非常影响体验的。因此我这里作于一个改进

```cpp
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
```

就是给老鼠移动到奶酪或者移动到新地址的动作加上一个`tag`，然后再次执行移动的时候，先通过`tag`将当前的动作停止，然后老鼠的坐标就会被设置到当前位置，新的移动的坐标就会被叠加到当前位置上，就可以解决这个问题了。
