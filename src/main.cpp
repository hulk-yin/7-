#include <Arduino.h>
#include "OLEDDisplay.h"

// 创建OLED显示对象
OLEDDisplay display;

void setup() {
  Serial.begin(115200);
  Serial.println("启动OLED 128x32屏幕测试");
  
  // 扫描I2C设备
  display.scanI2CDevices();
  
  // 初始化显示屏
  if (display.begin()) {
    // 测试屏幕边界
    display.testScreenBoundaries();
    Serial.println("已绘制屏幕边界测试图形");
    
    // 延时查看边界测试结果
    delay(3000);
    
    // 显示开机画面
    display.showBootScreen();
    Serial.println("显示开机画面");
    
    delay(2000);
  }
}

void loop() {
  // 检查并处理显示错误
  if (display.checkAndResetOnError()) {
    // 显示正常时更新状态信息
    display.showStatus(millis() / 1000, true);
  } else {
    // 当显示有问题时，输出到Serial控制台
    Serial.println("显示屏当前状态异常，等待重新初始化...");
  }
  
  delay(1000);
}