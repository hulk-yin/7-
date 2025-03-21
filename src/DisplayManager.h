#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include "OLEDDisplay.h"
#include "AudioProcessor.h"

// 转发到OLEDDisplay的兼容层类
class DisplayManager {
public:
    DisplayManager(int sdaPin = 21, int sclPin = 22) {}
    
    // 初始化方法，转发到OLEDDisplay的begin
    void init() {
        extern OLEDDisplay oledDisplay; // 修改为oledDisplay
        oledDisplay.begin();
    }
    
    // 清屏
    void clearDisplay() {
        extern OLEDDisplay oledDisplay; // 修改为oledDisplay
        oledDisplay.clear();
    }
    
    // 刷新显示
    void display() {
        extern OLEDDisplay oledDisplay; // 修改为oledDisplay
        oledDisplay.refresh();
    }
    
    // 设置光标位置，不在OLEDDisplay中直接支持，忽略
    void setCursor(int16_t x, int16_t y) {}
    
    // 文本显示，转发到printLog
    void print(const String &text) {
        extern OLEDDisplay oledDisplay; // 修改为oledDisplay
        oledDisplay.printLog(text);
    }
    
    // 其他方法保留为空实现，以便兼容现有代码
    void showWaveform(AudioProcessor* audioProcessor) {}
    void showFFT(AudioProcessor* audioProcessor) {}
    void println(const String &text) {}
    void drawText(const String &text, int16_t x, int16_t y) {}
    void showStatus(const String &status) {}
    void scanI2CDevices() {}
    void testScreenBoundaries() {}
};

#endif // DISPLAY_MANAGER_H
