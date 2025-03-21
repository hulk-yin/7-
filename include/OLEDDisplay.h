#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLEDDisplay
{
public:
  OLEDDisplay();
  bool begin();
  void clear();
  void showBootScreen();
  void testScreenBoundaries();
  void printLog(const char *message, int line);
  void scanI2CDevices();
  bool checkAndResetOnError();

private:
  Adafruit_SSD1306 display;
  unsigned long lastRefreshTime = 0;
  const unsigned long refreshInterval = 30; // 30ms刷新间隔，可根据实际情况调整
  
  void switchToChineseFont();
  void switchToLatinFont();
};

#endif