#include <Arduino.h>
#include "AudioProcessor.h"
#include "ASRClient.h"
#include "WebSocketHandlers.h"
#include "OLEDDisplay.h" // 改为使用OLEDDisplay

// 定义引脚
#define I2S_SCK_PIN 26
#define I2S_WS_PIN  25
#define I2S_SD_PIN  33

// 创建对象
AudioProcessor audioProcessor(I2S_SCK_PIN, I2S_WS_PIN, I2S_SD_PIN);
extern ASRClient asrClient; // 从WebSocketHandlers.cpp中导入已定义的ASRClient实例
OLEDDisplay oledDisplay; // 使用oledDisplay类

unsigned long lastDisplayUpdate = 0;
const unsigned long displayUpdateInterval = 20; // 20ms更新一次显示

void setup() {
  Serial.begin(115200);
  
  // 初始化WiFi连接和ASR
  initWebSocketConnections();
  
  // 初始化音频处理器
  audioProcessor.init();
  audioProcessor.setASRClient(&asrClient);
  
  // 初始化显示
  oledDisplay.begin(); // 修改为begin方法
  oledDisplay.showBootScreen(); // 使用oledDisplay的启动画面
  
  // 启动语音识别
  audioProcessor.startSpeechRecognition();
}

void loop() {
  // 处理WebSocket事件
  handleWebSocketLoops();
  
  // 捕获音频
  audioProcessor.captureAudio();
  audioProcessor.processAudio();
  
  // 更新ASR状态
  audioProcessor.updateASR();
  
  // 检查并重置显示错误
  oledDisplay.checkAndResetOnError();
  
  // 更新显示
  unsigned long currentTime = millis();
  if (currentTime - lastDisplayUpdate >= displayUpdateInterval) {
    lastDisplayUpdate = currentTime;
    
    // 清除显示缓冲区
    oledDisplay.clear();
    
    // 显示识别的文本
    String recognizedText = audioProcessor.getRecognizedText();
    if (recognizedText.length() > 0) {
      // 限制文本长度，避免溢出
      if (recognizedText.length() > 20) {
        recognizedText = recognizedText.substring(0, 17) + "...";
      }
      
      // 使用printLog方法
      oledDisplay.printLog(recognizedText, 0);
      
      // 也打印到串口以便调试
      Serial.println("识别文本: " + recognizedText);
    } else {
      // 显示状态信息
      oledDisplay.showStatus(millis() / 1000, true);
    }
  }
}