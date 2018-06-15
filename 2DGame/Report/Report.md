# Cocos2d-x 2D横板游戏控制

这次的项目的要求比较简单，就是一个简单的横板游戏（虽然并没有什么可以玩的。涉及到的关键点也不多，因此很快就可以完成了。



## 布局

### 血条

这个是素材当中的一个元素，这里可以通过从素材图片中截取指定的矩形来提取出来。提取出来后得到一个血条的背景和一段粉红色的条。

```c++
Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));
```

然后就是对于一些类型和位置的设置

```c++
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
```

### 角色人物

这里的素材是从一段帧动画从提取的第一帧画面，首先创建一个贴图，然后切割出一个关键帧，从而创建一个精灵，那么就可以了

```c++
//创建一张贴图
auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
//从贴图中以像素单位切割，创建关键帧
auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
//使用第一帧创建精灵
player = Sprite::createWithSpriteFrame(frame0);
player->setPosition(Vec2(origin.x + visibleSize.width / 2,
                         origin.y + visibleSize.height / 2));
addChild(player, 3);
```

### 控制按钮

控制按钮由`MenuItemLabel`组成，首先是使用`Label::createWithTTF`从指定的`ttf`文件创建`Label`， 然后绑定指定的回调事件，最后添加到`Menu`里面，就可以搞定了。

这里比较需要关注的是回调函数，因为这6个按钮可以分为两种类型，因此可以设置两个回调函数，然后根据参数的不同而呈现不同的动作，这样就可以避免大量的重复代码

这里举一个例子：

```c++
// .cpp
auto buttonW = MenuItemLabel::create(labelW, CC_CALLBACK_1(HelloWorld::buttonWASDCallBack, this, 0));

// .h
/// <param name='dir'>方向： 0 - w, 1 - a, 2 - s, 3 - d</param>  
virtual void buttonWASDCallBack(Ref* pSender, int dir);
```

这里将`WASD`的移动动作写到一个函数里面，然后根据不同的参数决定不同的移动方向.

这里使用的回调类型为`CC_CALLBACK_1`， 这个1的意思就是这个函数有一个自定义的参数，同样的有`0`, `2`, `3`等其他回调类型。



## 动画

### 帧动画

这里使用到一些帧动画，是由一张图片切割出不同的帧组成的。

至于具体的切割操作也是比较简单

比如说是死亡动画

```c++
// 死亡动画(帧数：22帧，高：90，宽：79）
auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
int frameCount = 22;
dead.reserve(frameCount);
for (int i = 0; i < frameCount; i++) {
    auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
    dead.pushBack(frame);
}
```

只要知道这个素材的帧数和高宽，就可以轻易生成一个动画了。

之后的使用方法:

```c++
auto animation = Animation::createWithSpriteFrames(run, 0.05f);
player->runAction(Animate::create(animation));
```

从这个容器里面加载出素材，指定速度创建一个`Animation`，然后再创建为`Animate`就可以了。



### 防止动画重复

因为有些动画在逻辑上是不应该同时或者重复发生的，因此需要做一些措施防止他们重复发生。

最简单的方法就是给指定的动画通过`setTag`设置一个`Tag`，然后通过调用`getActionByTag(xxx)->isDone()`判断他们是否执行完成再进行下一步操作。

```c++
if (player->getActionByTag(534) != nullptr && !(player->getActionByTag(534)->isDone())) return;
// ....
action->setTag(534);
player->runAction(action);
```



### 同时发生的动画

对于移动操作，我们需要走路动画和移动动画同时发生，因此需要两个`Animate`同时发生，因此可以使用`Spawn`创建一个同时序列

```c++
auto action = Spawn::createWithTwoActions(Animate::create(animation), move);
```





## 调度器

这次项目要求显示一个倒计时，这个可以通过自定义一个1s的调度器实现。

调度器的生成

```c++
void updateTime(float dt);
this->schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);
```

调度器的停止

```c++
this->unschedule(schedule_selector(HelloWorld::updateTime));
```

需要注意的是，调度器中的函数的参数必须是float，否则会报错。



## 键盘事件

由于是横板游戏，在PC端肯定是需要加入一些键盘事件的，不然总是鼠标点击会异常反人类

创建键盘事件的方法也比较简单，如下：

```c++
// 键盘事件
auto eventListener = EventListenerKeyboard::create();

eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
    switch (keyCode) {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case EventKeyboard::KeyCode::KEY_A:
            // ...
    }
};
```





## 结果

![1527226036810](Report.assets\1527226036810.png)



![1527226049329](Report.assets\1527226049329.png)



![1527226068990](Report.assets\1527226068990.png)
