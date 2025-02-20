#include <Arduino.h>
#include <WiFi.h>
#include "WebSocketHandlers.h"
#include "driver/i2s.h"

// WiFi 配置信息
const char *ssid = "Xiaomi_73A0_Wi-Fi5";
const char *password = "03031902";

// 定义音频采集模块的引脚
const int audioPin = 34;

// 定义音频采样率
const int sampleRate = 16000; // 或者 8000

// 时间同步配置
void setupTimeSync()
{
  configTime(8 * 3600, 0, "cn.pool.ntp.org", "time.windows.com", "time.nist.gov"); // 中国时间
  Serial.println("正在同步时间...");
  while (time(nullptr) < 100000)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n时间同步成功！");
}

bool isNetworkAvailable()
{
  WiFiClient client;
  return client.connect("tts-api.xfyun.cn", 80);
}

void setup()
{
  Serial.begin(115200);

  // 连接到 WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("正在连接到 WiFi...");
  }
  Serial.println("WiFi 连接成功！");

  // 探测网络是否正常
  if (!isNetworkAvailable())
  {
    Serial.println("网络连接失败！");
  }
  else
  {
    Serial.println("网络连接正常！");
  }

  setupTimeSync(); // 调用时间同步配置
  // 初始化 WebSocket 连接
  initWebSocketConnections();

  // 初始化音频采集模块引脚
  pinMode(audioPin, INPUT);

  // 打印当前时间
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);
  Serial.printf("当前时间: %s", asctime(timeinfo));
}

void loop()
{

  handleWebSocketLoops();
}