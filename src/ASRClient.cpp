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

std::vector<uint8_t> audioBuffer;
unsigned long lastSendTime = 0;

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
  // 获取当前时间戳
  time_t now = time(nullptr);
  struct tm *timeinfo = gmtime(&now);

  // 组装鉴权 URL
  String asrHost = "iat-api.xfyun.cn";
  String asrPath = "/v2/iat";
  String asrAuthUrl = AuthUtils::assembleAuthUrl(asrHost, asrPath, apiKey, apiSecret, *timeinfo);

  // 初始化 WebSocket 连接并将鉴权信息写入 URL
  webSocket.setReconnectInterval(5000);                                             // 设置重连间隔
  webSocket.beginSSL(asrHost.c_str(), 443, (asrPath + asrAuthUrl).c_str(), "", ""); // 开始 SSL 连接
  webSocket.onEvent(webSocketEvent);
}

void ASRClient::loop()
{
  webSocket.loop();

  // 检查是否到了发送数据的时间
  unsigned long currentTime = millis();
  if (currentTime - lastSendTime >= SEND_INTERVAL)
  {
    lastSendTime = currentTime;

    // 检查缓冲区中是否有足够的数据
    if (audioBuffer.size() >= AUDIO_CHUNK_SIZE)
    {
      // 构建有效的 JSON 字符串
      String jsonData = "{\"common\":{\"app_id\":\"" + String(appId) + "\"},";
      jsonData += "\"business\":{\"language\":\"zh_cn\",\"domain\":\"iat\",\"accent\":\"mandarin\"},";
      jsonData += "\"data\":{\"status\":1,\"format\":\"audio/L16;rate=8000\",\"encoding\":\"raw\",\"audio\":\"";

      // 将音频数据转换为 base64 编码
      String audioBase64 = base64::encode(audioBuffer.data(), AUDIO_CHUNK_SIZE);
      jsonData += audioBase64 + "\"}}";

      // 发送数据到 ASR 服务
      webSocket.sendTXT(jsonData);

      // 从缓冲区中移除已发送的数据
      audioBuffer.erase(audioBuffer.begin(), audioBuffer.begin() + AUDIO_CHUNK_SIZE);
    }
  }
}

void ASRClient::sendData(uint8_t *data, size_t length)
{
  // 将音频数据添加到缓冲区
  audioBuffer.insert(audioBuffer.end(), data, data + length);
}
