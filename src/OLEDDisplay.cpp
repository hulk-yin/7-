#include "OLEDDisplay.h"

OLEDDisplay::OLEDDisplay() 
  : u8g2(U8G2_R0, U8X8_PIN_NONE), 
    displayError(false), 
    lastDisplayResetAttempt(0) {
}

bool OLEDDisplay::begin() {
  // 先检查是否已有其他库使用I2C总线
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100); // 给I2C总线启动一些时间
  
  // 检查总线上是否有设备
  scanI2CDevices();
  
  // 初始化U8g2屏幕
  u8g2.setI2CAddress(SCREEN_ADDRESS * 2); // U8G2需要左移一位的地址
  bool success = u8g2.begin();
  if (!success) {
    Serial.println("U8G2显示屏初始化失败");
    displayError = true;
    return false;
  }
  
  u8g2.enableUTF8Print(); // 启用UTF8文本支持
  u8g2.setContrast(255);  // 最大对比度
  
  // 测试显示
  testDisplay();
  
  Serial.println("U8G2显示屏初始化成功");
  return true;
}

void OLEDDisplay::testDisplay() {
  // 测试显示是否正常工作
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_10x20_tf); // 使用较大字体确保可见
  u8g2.setCursor(0, 20);
  u8g2.print("U8G2测试");
  u8g2.sendBuffer();
  delay(1000);
  
  // 清除显示
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void OLEDDisplay::scanI2CDevices() {
  byte error, address;
  int deviceCount = 0;
  
  Serial.println("扫描I2C总线上的设备...");
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("发现I2C设备，地址: 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      if (address == SCREEN_ADDRESS) {
        Serial.print(" (与配置的屏幕地址匹配)");
      } else if (address == 0x3D) {
        Serial.print(" (可能是OLED屏幕备用地址)");
      }
      Serial.println();
      deviceCount++;
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("未找到I2C设备！检查接线。");
  } else {
    Serial.print("共发现 ");
    Serial.print(deviceCount);
    Serial.println(" 个I2C设备");
  }
}

void OLEDDisplay::testScreenBoundaries() {
  if (displayError) return;
  
  u8g2.clearBuffer();
  
  // 绘制边框
  u8g2.drawFrame(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  
  // 在四个角落标记像素
  u8g2.drawPixel(0, 0);
  u8g2.drawPixel(SCREEN_WIDTH-1, 0);
  u8g2.drawPixel(0, SCREEN_HEIGHT-1);
  u8g2.drawPixel(SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
  
  // 显示屏幕尺寸文本
  u8g2.setFont(u8g2_font_10x20_tf); // 使用较大字体确保可见
  u8g2.setCursor(10, 20);
  u8g2.print(SCREEN_WIDTH);
  u8g2.print("x");
  u8g2.print(SCREEN_HEIGHT);
  
  u8g2.sendBuffer();
}

void OLEDDisplay::showBootScreen() {
  if (displayError) return;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_chinese1);
  
  // 显示开机信息
  u8g2.setCursor(0, 12);
  u8g2.print("系统启动中...");
  
  u8g2.setCursor(0, 26);
  u8g2.print("OLED显示屏初始化完成");
  
  u8g2.sendBuffer();
  // 修复条件表达式错误
  if (u8g2.getBufferTileHeight() <= 0) {
    displayError = true;
    lastDisplayResetAttempt = millis();
    Serial.println("显示开机画面时发生错误");
  }
}

void OLEDDisplay::printLog(const char* text, int line) {
  if (displayError) return;
  
    int y_start = line * 14; // 行的起始y坐标
    u8g2.setDrawColor(0);    // 0表示黑色(清除)
    u8g2.drawBox(0, y_start, SCREEN_WIDTH, 14); // 绘制覆盖整行的黑色矩形
    u8g2.setDrawColor(1);    // 1表示白色(绘制)
  
  // 每行文字大致高度14像素，最多2行
  int y = line * 14 + 12; // 增加行高和基线位置
  if (y > SCREEN_HEIGHT) y = SCREEN_HEIGHT - 2;
  
  u8g2.setFont(u8g2_font_wqy12_t_chinese1);
  u8g2.setCursor(0, y);
  u8g2.print(text);
  u8g2.sendBuffer();
  
  // 修复条件表达式错误
  if (u8g2.getBufferTileHeight() <= 0) {
    displayError = true;
    lastDisplayResetAttempt = millis();
    Serial.println("打印日志时发生错误");
  }
}

void OLEDDisplay::printLog(String text, int line) {
  printLog(text.c_str(), line);
}

void OLEDDisplay::clear() {
  if (displayError) return;
  
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  // 修复条件表达式错误
  if (u8g2.getBufferTileHeight() <= 0) {
    displayError = true;
    lastDisplayResetAttempt = millis();
    Serial.println("清屏时发生错误");
  }
}

void OLEDDisplay::refresh() {
  if (displayError) return;
  
  u8g2.sendBuffer();
  // 修复条件表达式错误
  if (u8g2.getBufferTileHeight() <= 0) {
    displayError = true;
    lastDisplayResetAttempt = millis();
    Serial.println("刷新显示时发生错误");
  }
}

void OLEDDisplay::showStatus(unsigned long uptime, bool isNormal) {
  if (displayError) return;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_chinese1);
  
  // 第一行：尺寸和地址
  u8g2.setCursor(0, 12);
  u8g2.print("尺寸:128x32 地址:0x");
  u8g2.print(SCREEN_ADDRESS, HEX);
  
  // 第二行：运行时间和状态
  u8g2.setCursor(0, 28);
  u8g2.print("运行:");
  u8g2.print(uptime);
  u8g2.print("秒 状态:");
  u8g2.print(isNormal ? "正常" : "异常");
  
  u8g2.sendBuffer();
  // 修复条件表达式错误
  if (u8g2.getBufferTileHeight() <= 0) {
    displayError = true;
    lastDisplayResetAttempt = millis();
    Serial.println("显示状态时发生错误");
  }
}

bool OLEDDisplay::checkAndResetOnError() {
  if (displayError && (millis() - lastDisplayResetAttempt > resetInterval)) {
    Serial.println("尝试重新初始化显示屏...");
    lastDisplayResetAttempt = millis();
    
    // 尝试重新初始化
    if (u8g2.begin()) {
      displayError = false;
      u8g2.enableUTF8Print();
      Serial.println("显示屏重新初始化成功");
      return true;
    } else {
      Serial.println("显示屏重新初始化失败");
    }
  }
  
  return !displayError;
}
