#include <Arduino.h>
#include "OLEDDisplay.h"
#include "NetworkManager.h"
#include "AudioProcessor.h"  // 添加音频处理器头文件
#include "ASRClient.h"       // 添加语音识别客户端头文件

// WiFi credentials
const char *ssid = "Xiaomi_73A0";
const char *password = "03031902";

// 使用extern引用WebSocketHandlers.cpp中已定义的变量
extern const char *appId;
extern const char *apiSecret;
extern const char *apiKey;
extern ASRClient *asrClient;

// 创建OLED显示对象
OLEDDisplay display;
// 创建网络管理对象
NetworkManager *networkManager;
// 创建音频处理器
AudioProcessor *audioProcessor;

// 定义按钮引脚
const int buttonPin = 15;  // GPIO15 作为按钮输入
bool isListening = false;  // 跟踪当前是否在"聆听"状态
unsigned long buttonPressTime = 0;  // 记录按钮按下的时间

// 麦克风I2S引脚定义
const int I2S_SCK_PIN = 32;  // 位时钟
const int I2S_WS_PIN = 25;   // 左右声道时钟
const int I2S_SD_PIN = 33;   // 数据线

void setup()
{
  Serial.begin(115200);
  Serial.println("启动OLED 128x32屏幕测试");

  // 配置按钮引脚
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.println("按钮引脚已配置");

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
    
    // 初始化音频处理器
    audioProcessor = new AudioProcessor(I2S_SCK_PIN, I2S_WS_PIN, I2S_SD_PIN);
    // audioProcessor->init();
    Serial.println("音频处理器初始化完成");
    
    // 使用已存在的ASRClient实例，确保初始化
    if (asrClient) {
      // 确保ASRClient已初始化
      // asrClient->init();
      Serial.println("语音识别客户端初始化完成");
      
      // 将ASR客户端绑定到音频处理器
      // audioProcessor->setASRClient(asrClient);
    } else {
      Serial.println("警告：ASRClient未创建，语音识别功能将不可用");
    }
  }
}

void loop()
{
  // 检测按钮状态
  bool buttonPressed = (digitalRead(buttonPin) == LOW);  // 按钮按下时为LOW
  
  // 按钮状态变化处理
  if (buttonPressed && !isListening && audioProcessor) {
    // 按钮刚被按下
    isListening = true;
    buttonPressTime = millis();
    display.clear();
    display.printLog("Listening...", 1);
    Serial.println("按钮按下，开始录音...");
    
    // 开始录音和语音识别
    // audioProcessor->startSpeechRecognition();
  } 
  else if (!buttonPressed && isListening && audioProcessor) {
    // 按钮刚被松开
    isListening = false;
    display.clear();
    Serial.println("按钮松开，停止录音");
    
    // 停止录音和语音识别
    // audioProcessor->stopSpeechRecognition();
    
    // 显示识别结果
    String recognizedText = audioProcessor->getRecognizedText();
    if (recognizedText.length() > 0) {
      display.printLog("识别结果:", 0);
      display.printLog(recognizedText, 1);
      Serial.println("识别结果: " + recognizedText);
    }
  } 
   if (!isListening) {
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
            sprintf(timeStr, "%02d:%02d:%02d",  
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
          display.printLog(timeStr,1);
        }
      }
    }
    else
    {
      // 当显示有问题时，输出到Serial控制台
      Serial.println("显示屏当前状态异常，等待重新初始化...");
    }
  }

  // 添加一个短暂的延迟以防止CPU占用过高
  delay(10);  // 录音时使用更短的延迟以提高采样响应速度
}