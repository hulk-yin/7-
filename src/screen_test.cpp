#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// 常见的OLED驱动IC和尺寸组合
// 1. SSD1306 128x64
// 2. SSD1306 128x32
// 3. SH1106 128x64
// 4. SSD1309 128x64

// I2C地址通常是0x3C或0x3D
#define SDA_PIN 21 // NodeMCU-32S默认SDA引脚
#define SCL_PIN 22 // NodeMCU-32S默认SCL引脚

// 使用预编译条件选择不同的屏幕配置
#define TEST_SCREEN 2 // 修改为2=SSD1306 128x32，与BOM清单匹配

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // 扫描I2C设备
  Serial.println("扫描I2C设备...");
  byte error, address;
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("找到I2C设备，地址: 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  
  Serial.println("尝试初始化屏幕...");
  
  // 根据选择的配置初始化不同的屏幕
  bool initSuccess = false;
  
#if TEST_SCREEN == 1
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
  Serial.println("测试 SSD1306 128x64");
#elif TEST_SCREEN == 2
  U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
  Serial.println("测试 SSD1306 128x32");
#elif TEST_SCREEN == 3
  U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
  Serial.println("测试 SH1106 128x64");
#elif TEST_SCREEN == 4
  U8G2_SSD1309_128X64_NONAME0_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
  Serial.println("测试 SSD1309 128x64");
#endif

  initSuccess = u8g2.begin();
  
  if (initSuccess) {
    Serial.println("屏幕初始化成功!");
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Screen Test");
    
#if TEST_SCREEN == 1
    u8g2.drawStr(0, 24, "SSD1306 128x64");
#elif TEST_SCREEN == 2
    u8g2.drawStr(0, 24, "SSD1306 128x32");
#elif TEST_SCREEN == 3
    u8g2.drawStr(0, 24, "SH1106 128x64");
#elif TEST_SCREEN == 4
    u8g2.drawStr(0, 24, "SSD1309 128x64");
#endif
    
    // 绘制边框
    u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight());
    
    u8g2.sendBuffer();
  } else {
    Serial.println("屏幕初始化失败!");
  }
}

void loop() {
  // 空循环
}
