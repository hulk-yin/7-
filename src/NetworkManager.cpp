#include "NetworkManager.h"

NetworkManager::NetworkManager(const char *ssid, const char *password, OLEDDisplay *display)
{
  this->ssid = ssid;
  this->password = password;
  this->display = display;
}

bool NetworkManager::begin()
{
  // 连接WiFi
  WiFi.begin(ssid, password);
  display->printLog("正在连接WiFi...", 0);
  Serial.println("正在连接WiFi...");

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 10)
  {
    delay(1000);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi连接成功");
    Serial.print("IP 地址: ");
    Serial.println(WiFi.localIP());
    display->printLog("WiFi已连接", 0);
    display->printLog(WiFi.localIP().toString().c_str(), 1);

    // 连接成功后同步时间
    return syncTime();
  }
  else
  {
    Serial.println("");
    Serial.println("WiFi连接失败");
    display->printLog("WiFi连接失败", 0);
    return false;
  }
}

bool NetworkManager::isConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

IPAddress NetworkManager::getIP()
{
  return WiFi.localIP();
}

bool NetworkManager::syncTime()
{
  // 设置时区为北京时间 (GMT+8)
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("正在等待 NTP 时间同步...");
  display->printLog("正在等待 NTP 时间同步...", 0);
  // 等待时间同步
  time_t now = 0;
  struct tm timeinfo = {0};
  int retry = 0;

  while (timeinfo.tm_year < (2022 - 1900) && ++retry < 30)
  {
    Serial.print(".");
    delay(500);
    time(&now);
    localtime_r(&now, &timeinfo);
  }

  Serial.println("");

  if (timeinfo.tm_year >= (2022 - 1900))
  {
    Serial.println("时间同步成功");
    Serial.print("当前时间: ");
    Serial.print(timeinfo.tm_year + 1900);
    Serial.print("-");
    Serial.print(timeinfo.tm_mon + 1);
    Serial.print("-");
    Serial.print(timeinfo.tm_mday);
    Serial.print(" ");
    Serial.print(timeinfo.tm_hour);
    Serial.print(":");
    Serial.print(timeinfo.tm_min);
    Serial.print(":");
    Serial.println(timeinfo.tm_sec);

    // 在OLED上显示同步后的时间
    char timeStr[20];
    sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    display->printLog("时间已同步", 0);
    display->printLog(timeStr, 1);
    return true;
  }
  else
  {
    Serial.println("时间同步失败");
    display->printLog("时间同步失败", 0);
    return false;
  }
}

bool NetworkManager::getCurrentTime(struct tm *timeinfo)
{
  if (!isConnected())
  {
    return false;
  }

  time_t now;
  time(&now);
  localtime_r(&now, timeinfo);

  return timeinfo->tm_year >= (2022 - 1900);
}
