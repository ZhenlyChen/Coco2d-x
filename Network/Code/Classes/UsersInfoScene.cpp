#include "UsersInfoScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

using namespace cocos2d::network;
using namespace rapidjson;

cocos2d::Scene * UsersInfoScene::createScene() {
  return UsersInfoScene::create();
}

bool UsersInfoScene::init() {
  if (!Scene::init()) return false;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto getUserButton = MenuItemFont::create("Get User", CC_CALLBACK_1(UsersInfoScene::getUserButtonCallback, this));
  if (getUserButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + getUserButton->getContentSize().height / 2;
    getUserButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(getUserButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  limitInput = TextField::create("limit", "arial", 24);
  if (limitInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    limitInput->setPosition(Vec2(x, y));
    this->addChild(limitInput, 1);
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

void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
  // Your code here
  // TODO UserInfo
  HttpRequest* request = new HttpRequest();
  request->setRequestType(HttpRequest::Type::GET);
  std::string url = "http://127.0.0.1:8000/users?limit=";
  url.append(limitInput->getString());
  request->setUrl(url.c_str());
  request->setResponseCallback(CC_CALLBACK_2(UsersInfoScene::onHttpGetUserComplete, this));
  cocos2d::network::HttpClient::getInstance()->send(request);
  request->release();
}

void UsersInfoScene::onHttpGetUserComplete(HttpClient *sender, HttpResponse *response) {
  if (!response) {
    messageBox->setString("Can't connect server.");
    return;
  }
  if (!response->isSucceed()) {
    messageBox->setString("Can't connect server.");
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
      std::string text;
      for (auto& v : d["data"].GetArray()) {
        auto user = v.GetObjectW();
        text.append("Username:");
        text.append(v["username"].GetString());
        text.append("\nDeck:\n");
        for (auto& card : v["deck"].GetArray()) {
          for (auto& obj : card.GetObjectW()) {
            char tmp[1024];
            sprintf(tmp, "  %s: %d\n", obj.name.GetString(), obj.value.GetInt());
            text.append(tmp);
          }
          text.append("  ---\n");
        }
        text.append("---\n");
      }
      messageBox->setString(text);
    }
    else {
      std::string text("GetInfo Failed.\n");
      text.append(d["msg"].GetString());
      messageBox->setString(text);
    }
  }
  else {
    messageBox->setString("Can't parse JSON from server.");
  }
}