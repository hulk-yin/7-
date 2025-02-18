#include <Arduino.h>
#include <WiFi.h>
#include "WebSocketHandlers.h"


// WiFi 配置信息
const char *ssid = "Xiaomi_73A0_Wi-Fi5";
const char *password = "03031902";

// 定义音频采集模块的引脚
const int audioPin = 34;

void setup() {
  Serial.begin(115200);
  
  // 连接到 WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("正在连接到 WiFi...");
  }
  Serial.println("WiFi 连接成功！");

  // 初始化 WebSocket 连接
  initWebSocketConnections();

  // 初始化音频采集模块引脚
  pinMode(audioPin, INPUT);

}

void loop() {

  // 读取音频采集模块数据
  int audioValue = analogRead(audioPin);

  // 将音频数据转换为科大讯飞 ASR 支持的数据流
  uint8_t audioData[2];
  audioData[0] = (audioValue >> 8) & 0xFF;
  audioData[1] = audioValue & 0xFF;
  
  // 发送音频数据到 ASR 服务

  sendASRData(audioData, sizeof(audioData));
  handleWebSocketLoops();
 

}