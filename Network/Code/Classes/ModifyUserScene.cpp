#include "ModifyUserScene.h"
#include "Utils.h"
#include "network\HttpClient.h"
#include "json\document.h"

using namespace cocos2d::network;

cocos2d::Scene * ModifyUserScene::createScene() {
  return ModifyUserScene::create();
}

bool ModifyUserScene::init() {
  if (!Scene::init()) return false;
  
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto postDeckButton = MenuItemFont::create("Post Deck", CC_CALLBACK_1(ModifyUserScene::putDeckButtonCallback, this));
  if (postDeckButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + postDeckButton->getContentSize().height / 2;
    postDeckButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(postDeckButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  deckInput = TextField::create("Deck json here", "arial", 24);
  if (deckInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    deckInput->setPosition(Vec2(x, y));
    this->addChild(deckInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void ModifyUserScene::putDeckButtonCallback(Ref * pSender) {
  // Your code here
  // TODO ModifyUser
  // Your code here
  // TODO Login
  if ((deckInput->getString()).length() == 0) {
    messageBox->setString("[ERR]: Content can't is null");
    return;
  }
  // Ω‚Œˆƒ⁄»›
  rapidjson::Document d;
  d.Parse<0>(deckInput->getString().c_str());
  if (d.HasParseError()) {
    messageBox->setString("[ERR]: Can't parse JSON from input.");
    return;
  }
  if (!d.IsArray()) {
    messageBox->setString("[ERR]: Can't parse JSON to Array.");
    return;
  }
  for (auto& card : d.GetArray()) {
    if (!card.IsObject()) {
      messageBox->setString("[ERR]: It isn't a object array.");
      return;
    }
    for (auto& obj : card.GetObjectW()) {
      if (!obj.value.IsInt()) {
        messageBox->setString("[ERR]: Card Number isn't int type.");
        return;
      }
    }
  }

  cocos2d::network::HttpClient::getInstance()->enableCookies(nullptr);
  HttpRequest* request = new HttpRequest();
  request->setRequestType(HttpRequest::Type::PUT);
  request->setUrl("http://127.0.0.1:8000/users");

  rapidjson::Document document;
  document.SetObject();
  rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
  document.AddMember("deck", d.GetArray(), allocator);

  StringBuffer buffer;
  rapidjson::Writer<StringBuffer> writer(buffer);
  document.Accept(writer);

  request->setRequestData(buffer.GetString(), buffer.GetSize());
  request->setResponseCallback(CC_CALLBACK_2(ModifyUserScene::onHttpPutDeckComplete, this));
  cocos2d::network::HttpClient::getInstance()->send(request);
  request->release();
}

void ModifyUserScene::onHttpPutDeckComplete(HttpClient *sender, HttpResponse *response) {
  if (!response) {
    messageBox->setString("Can't connect server.");
    return;
  }
  if (!response->isSucceed()) {
    messageBox->setString("You must be login.");
    return;
  }
  std::vector<char> *buffer = response->getResponseData();
  std::string str(buffer->begin(), buffer->end());
  rapidjson::Document d;
  d.Parse<0>(str.c_str());
  if (d.HasParseError()) {
    messageBox->setString("Can't parse JSON from server.");
    return;
  }
  if (d.IsObject() && d.HasMember("status")) {
    if (d["status"].GetBool() == true) {
      messageBox->setString("Modify Success.");
    }
    else {
      messageBox->setString("Modify Failed.");
    }
  }
  else {
    messageBox->setString("Can't parse JSON from server.");
  }
}