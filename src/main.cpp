#include <Arduino.h>
#include "OLEDDisplay.h"
#include "NetworkManager.h"

// WiFi credentials
const char *ssid = "Xiaomi_73A0";         // 将 your_SSID 替换为你的 WiFi SSID
const char *password = "03031902"; // 将 your_PASSWORD 替换为你的 WiFi 密码

// 创建OLED显示对象
OLEDDisplay display;
// 创建网络管理对象
NetworkManager *networkManager;

void setup()
{
  Serial.begin(115200);
  Serial.println("启动OLED 128x32屏幕测试");

  // 扫描I2C设备
  display.scanI2CDevices();

  // 初始化显示屏
  if (display.begin())
  {
    // 测试屏幕边界
    display.testScreenBoundaries();
    Serial.println("已绘制屏幕边界测试图形");

    // 延时查看边界测试结果
    delay(1000);
    display.clear();
    // 显示开机画面
    display.showBootScreen();
    Serial.println("显示开机画面");

    delay(1000);

    // 初始化网络管理器
    networkManager = new NetworkManager(ssid, password, &display);

    // 连接WiFi并同步时间
    networkManager->begin();

    delay(1000);
  }
}

void loop()
{
  // 检查并处理显示错误
  if (display.checkAndResetOnError())
  {
    // 获取当前时间并显示
    if (networkManager->isConnected())
    {
      struct tm timeinfo;
      if (networkManager->getCurrentTime(&timeinfo))
      {
        char timeStr[20];
        sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        display.printLog(timeStr, 0);
        // display.showStatus("系统运行中", 1);
      }
      else
      {
        // display.showStatus(millis() / 1000, true);
      }
    }
    else
    {
      // display.showStatus(millis() / 1000, true);
    }
  }
  else
  {
    // 当显示有问题时，输出到Serial控制台
    Serial.println("显示屏当前状态异常，等待重新初始化...");
  }

  delay(1000);
}