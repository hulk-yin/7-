#include "ASRClient.h"
#include <base64.h>
#include <time.h>
#include "AuthUtils.h"
#include <vector>

// 初始化 ASR WebSocket 客户端
WebSocketsClient webSocketASR;

// 科大讯飞 API 的鉴权信息
extern const char *appId;
extern const char *apiSecret;
extern const char *apiKey;

// 定义缓冲区和相关变量
#define AUDIO_CHUNK_SIZE 1280
#define SEND_INTERVAL 40
#define SILENCE_THRESHOLD 5 * 1000  // 5秒无声音阈值
#define ENERGY_THRESHOLD 50         // 单样本能量阈值（最小值）
#define CONSECUTIVE_ACTIVE_FRAMES 3 // 连续有效帧数判定

std::vector<uint8_t> audioBuffer;
unsigned long lastSendTime = 0;
unsigned long lastAudioTime = 0;
int sendStatus = -1;          // -1: 不发送, 0: 首帧, 1: 正常发送, 2: 结束帧
int activeFrameCounter = 0;   // 有效帧计数器
int inactiveFrameCounter = 0; // 无效帧计数器

ASRClient::ASRClient(const char *appId, const char *apiKey, const char *apiSecret)
    : appId(appId), apiKey(apiKey), apiSecret(apiSecret) {}

void ASRClient::webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.println("ASR 连接断开！");
    break;
  case WStype_CONNECTED:
    Serial.println("ASR 连接成功！");
    break;
  case WStype_TEXT:
    Serial.printf("ASR 接收到文本：%s\n", payload);
    break;
  case WStype_BIN:
    Serial.printf("ASR 接收到二进制数据，长度：%u\n", length);
    break;
  case WStype_ERROR:
    Serial.println("ASR 发生错误！");
    break;
  case WStype_PING:
    Serial.println("ASR 接收到 PING！");
    break;
  case WStype_PONG:
    Serial.println("ASR 接收到 PONG！");
    break;
  default:
    Serial.println("未知的 ASR 事件类型！");
    break;
  }
}

void ASRClient::init()
{
  time_t now = time(nullptr);
  struct tm *timeinfo = gmtime(&now);
  String asrHost = "iat-api.xfyun.cn";
  String asrPath = "/v2/iat";
  String asrAuthUrl = AuthUtils::assembleAuthUrl(asrHost, asrPath, apiKey, apiSecret, *timeinfo);
  webSocket.setReconnectInterval(5000);
  webSocket.beginSSL(asrHost.c_str(), 443, (asrPath + asrAuthUrl).c_str(), "", "");
  webSocket.onEvent(webSocketEvent);
}

void ASRClient::loop()
{
  webSocket.loop();
  unsigned long currentTime = millis();
  if (currentTime - lastSendTime >= SEND_INTERVAL)
  {
    lastSendTime = currentTime;
    if (sendStatus == -1)
    {
      return;
    }
    if (audioBuffer.size() >= AUDIO_CHUNK_SIZE || (sendStatus == 2 && !audioBuffer.empty()))
    {
      String jsonData = "{\"common\":{\"app_id\":\"" + String(appId) + "\"},";
      jsonData += "\"business\":{\"language\":\"zh_cn\",\"domain\":\"iat\",\"accent\":\"mandarin\"},";
      jsonData += "\"data\":{\"status\":";
      if (sendStatus == 2 && audioBuffer.empty())
      {
        jsonData += "2";
      }
      else if (sendStatus == 2)
      {
        jsonData += "1";
      }
      else
      {
        jsonData += String(sendStatus);
      }
      jsonData += ",\"format\":\"audio/L16;rate=8000\",\"encoding\":\"raw\",\"audio\":\"";
      String audioBase64 = base64::encode(audioBuffer.data(), AUDIO_CHUNK_SIZE);
      jsonData += audioBase64 + "\"}}";
      webSocket.sendTXT(jsonData);
      audioBuffer.erase(audioBuffer.begin(), audioBuffer.begin() + AUDIO_CHUNK_SIZE);
      if (sendStatus == 0)
      {
        sendStatus = 1;
      }
      lastAudioTime = currentTime;
      if (sendStatus == 2 && audioBuffer.empty())
      {
        sendStatus = -1; // 重置状态
        activeFrameCounter = 0;
        inactiveFrameCounter = 0;
      }
    }
  }
}

void ASRClient::sendData(uint8_t *data, size_t length)
{
  if (length == 0 || length % 2 != 0)
    return;                                         // 无效输入检查
  audioBuffer.reserve(audioBuffer.size() + length); // 预分配空间

  uint32_t frameEnergy = 0;
  bool hasValidAudio = false;

  // 计算帧能量（16位PCM）
  for (size_t i = 0; i < length; i += 2)
  {
    int16_t sample = (int16_t)(data[i] | (data[i + 1] << 8)); // 小端序
    frameEnergy += abs(sample);
  }

  // 动态噪声基底
  static uint32_t noiseFloor = 100; // 初始值
  if (sendStatus == -1 || sendStatus == 2)
  {
    noiseFloor = (noiseFloor * 7 + frameEnergy) / 8; // 仅在非语音状态更新
  }

  // 有效音频判断
  if (frameEnergy > std::max(static_cast<uint32_t>(noiseFloor * 1.3), static_cast<uint32_t>(ENERGY_THRESHOLD)))
  {
    activeFrameCounter++;
    inactiveFrameCounter = 0;
    if (activeFrameCounter >= CONSECUTIVE_ACTIVE_FRAMES)
    {
      hasValidAudio = true;
    }
  }
  else
  {
    inactiveFrameCounter++;
    if (inactiveFrameCounter >= CONSECUTIVE_ACTIVE_FRAMES)
    {
      activeFrameCounter = 0;
    }
  }

  if (hasValidAudio)
  {
    audioBuffer.insert(audioBuffer.end(), data, data + length);
    lastAudioTime = millis();
    if (sendStatus == -1)
    {
      sendStatus = 0;
    }
  }
  else if (sendStatus != -1 && millis() - lastAudioTime >= SILENCE_THRESHOLD)
  {
    sendStatus = 2; // 设置为结束帧
  }
}