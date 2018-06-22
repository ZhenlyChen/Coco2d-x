#include "LoginRegisterScene.h"
#include "ui\CocosGUI.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

USING_NS_CC;
using namespace cocos2d::network;
using namespace cocos2d::ui;

cocos2d::Scene * LoginRegisterScene::createScene() {
  return LoginRegisterScene::create();
}

bool LoginRegisterScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto loginButton = MenuItemFont::create("Login", CC_CALLBACK_1(LoginRegisterScene::loginButtonCallback, this));
  if (loginButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + loginButton->getContentSize().height / 2;
    loginButton->setPosition(Vec2(x, y));
  }

  auto registerButton = MenuItemFont::create("Register", CC_CALLBACK_1(LoginRegisterScene::registerButtonCallback, this));
  if (registerButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + registerButton->getContentSize().height / 2 + 100;
    registerButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(loginButton, registerButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  usernameInput = TextField::create("username", "arial", 24);
  if (usernameInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    usernameInput->setPosition(Vec2(x, y));
    this->addChild(usernameInput, 1);
  }

  passwordInput = TextField::create("password", "arial", 24);
  if (passwordInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 130.0f;
    passwordInput->setPosition(Vec2(x, y));
    this->addChild(passwordInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 200.0f;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void LoginRegisterScene::loginButtonCallback(cocos2d::Ref * pSender) {
  // Your code here
  // TODO Login
  cocos2d::network::HttpClient::getInstance()->enableCookies(nullptr);
  HttpRequest* request = new HttpRequest();
  request->setRequestType(HttpRequest::Type::POST);
  request->setUrl("http://127.0.0.1:8000/auth");

  rapidjson::Document document;
  document.SetObject();
  rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
  if ((usernameInput->getString()).length() == 0) {
    messageBox->setString("Username can't is null");
    return;
  }
  else if (passwordInput->getString().length() == 0) {
    messageBox->setString("Password can't is null");
    return;
  }
  rapidjson::Value username, password;
  username.SetString(rapidjson::StringRef(usernameInput->getString().c_str()));
  password.SetString(rapidjson::StringRef(passwordInput->getString().c_str()));
  document.AddMember("username", username, allocator);
  document.AddMember("password", password, allocator);

  StringBuffer buffer;
  rapidjson::Writer<StringBuffer> writer(buffer);
  document.Accept(writer);

  request->setRequestData(buffer.GetString(), buffer.GetSize());
  request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpLoginComplete, this));
  cocos2d::network::HttpClient::getInstance()->send(request);
  request->release();
}

void LoginRegisterScene::registerButtonCallback(Ref * pSender) {
  // Your code here
  // DONE Register
  HttpRequest* request = new HttpRequest();
  request->setRequestType(HttpRequest::Type::POST);
  request->setUrl("http://127.0.0.1:8000/users");

  rapidjson::Document document;
  document.SetObject();
  rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
  rapidjson::Value username, password;
  if ((usernameInput->getString()).length() == 0) {
    messageBox->setString("Username can't is null");
    return;
  }
  else if (passwordInput->getString().length() == 0) {
    messageBox->setString("Password can't is null");
    return;
  }
  username.SetString(rapidjson::StringRef(usernameInput->getString().c_str()));
  password.SetString(rapidjson::StringRef(passwordInput->getString().c_str()));
  document.AddMember("username", username, allocator);
  document.AddMember("password", password, allocator);
  
  StringBuffer buffer;
  rapidjson::Writer<StringBuffer> writer(buffer);
  document.Accept(writer);

  request->setRequestData(buffer.GetString(), buffer.GetSize());
  request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpRegisterComplete, this));
  cocos2d::network::HttpClient::getInstance()->send(request);
  request->release();
}

void LoginRegisterScene::onHttpRegisterComplete(HttpClient *sender, HttpResponse *response) {
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
      std::string text("Register Success.\n");
      text.append(d["msg"].GetString());
      messageBox->setString(text);
    }
    else {
      std::string text("Register Failed.\n");
      text.append(d["msg"].GetString());
      messageBox->setString(text);
    }
  }
  else {
    messageBox->setString("Can't parse JSON from server.");
  }
}

void LoginRegisterScene::onHttpLoginComplete(HttpClient *sender, HttpResponse *response) {
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
      std::string text("Login Success.\n");
      text.append(d["msg"].GetString());
      messageBox->setString(text);
    }
    else {
      std::string text("Login Failed.\n");
      text.append(d["msg"].GetString());
      messageBox->setString(text);
    }
  }
  else {
    messageBox->setString("Can't parse JSON from server.");
  }
}
