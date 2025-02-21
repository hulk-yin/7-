#include <Arduino.h>
#include <WiFi.h>
#include "WebSocketHandlers.h"
#include "AudioProcessor.h"

// WiFi 配置信息
const char *ssid = "Xiaomi_73A0_Wi-Fi5";
const char *password = "03031902";

// 定义音频采集模块的引脚
const int audioPin = 34;

// 定义音频缓冲区大小
const int bufferSize = 1024;
AudioProcessor audioProcessor(audioPin, bufferSize);

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

TaskHandle_t audioTaskHandle;
TaskHandle_t webSocketTaskHandle;

void audioTask(void *parameter)
{
  while (true)
  {
    // 采集音频信号
    audioProcessor.captureAudio();

    // 发送音频数据到 ASR 服务
    // sendASRData(audioProcessor.getAudioBuffer(), audioProcessor.getBufferSize());

    vTaskDelay(10 / portTICK_PERIOD_MS); // 延迟以避免任务占用过多 CPU 时间
  }
}

void webSocketTask(void *parameter)
{
  while (true)
  {
    handleWebSocketLoops();
    vTaskDelay(10 / portTICK_PERIOD_MS); // 延迟以避免任务占用过多 CPU 时间
  }
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

  // 初始化音频处理器
  audioProcessor.init();

  // 创建音频任务，运行在核心 0 上
  xTaskCreatePinnedToCore(
      audioTask,        // 任务函数
      "Audio Task",     // 任务名称
      4096,             // 任务堆栈大小
      NULL,             // 任务参数
      1,                // 任务优先级
      &audioTaskHandle, // 任务句柄
      0                 // 运行在核心 0 上
  );

  // 创建 WebSocket 任务，运行在核心 1 上
  xTaskCreatePinnedToCore(
      webSocketTask,        // 任务函数
      "WebSocket Task",     // 任务名称
      4096,                 // 任务堆栈大小
      NULL,                 // 任务参数
      2,                    // 任务优先级
      &webSocketTaskHandle, // 任务句柄
      1                     // 运行在核心 1 上
  );

  // 打印当前时间
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);
  Serial.printf("当前时间: %s", asctime(timeinfo));
}

void loop()
{
  // 空的 loop 函数，因为逻辑已经移到任务中
}