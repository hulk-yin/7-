#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// 屏幕定义
#define SCREEN_WIDTH 128 // OLED宽度像素
#define SCREEN_HEIGHT 32 // OLED高度像素
#define SCREEN_ADDRESS 0x3C // 屏幕I2C地址

// I2C引脚定义
#define SDA_PIN 21 // NodeMCU-32S的默认SDA引脚
#define SCL_PIN 22 // NodeMCU-32S的默认SCL引脚

class OLEDDisplay {
public:
  OLEDDisplay();
  
  // 初始化函数
  bool begin();
  
  // 扫描I2C设备
  void scanI2CDevices();
  
  // 测试屏幕边界
  void testScreenBoundaries();
  
  // 显示开机画面
  void showBootScreen();
  
  // 日志打印接口
  void printLog(const char* text, int line = 0);
  void printLog(String text, int line = 0);
  
  // 清屏
  void clear();
  
  // 刷新显示
  void refresh();
  
  // 状态显示
  void showStatus(unsigned long uptime, bool isNormal = true);
  void showStatus(const char* line1, int line = 0); // 新增的重载函数
  
  // 错误处理
  bool checkAndResetOnError();
  
private:
  U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2;
  bool displayError;
  unsigned long lastDisplayResetAttempt;
  const unsigned long resetInterval = 5000; // 5秒重试间隔
};

#endif // OLED_DISPLAY_H
