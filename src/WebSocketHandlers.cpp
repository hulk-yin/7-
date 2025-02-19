#include "WebSocketHandlers.h"
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <mbedtls/md.h>
#include <base64.h>
#include <WiFi.h>
#include <time.h>
#include "AuthUtils.h"

// 初始化 WebSocket 客户端
WebSocketsClient webSocketTTS;
WebSocketsClient webSocketASR;
WebSocketsClient webSocketSpark;

// 科大讯飞 API 的鉴权信息
const char *appId = "57e5c152";
const char *apiSecret = "2b4d77de806ece14675de7ed359cd995";
const char *apiKey = "e92cfdec778718ef7348c8d4fb303893";
String userInputText; // 存储用户输入的文本

// 辅助函数声明
String urlEncode(const String &value);

// TTS WebSocket 事件处理函数
void webSocketTTSEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.println("TTS 连接断开！");
    break;
  case WStype_CONNECTED:
    Serial.println("TTS 连接成功！");
    break;
  case WStype_TEXT:
    Serial.printf("TTS 接收到文本：%s\n", payload);
    break;
  case WStype_BIN:
    Serial.printf("TTS 接收到二进制数据，长度：%u\n", length);
    // 将接收到的二进制数据通过蓝牙音箱播放
    //   SerialBT.write(payload, length);
    break;
  case WStype_ERROR:
    Serial.println("TTS 发生错误！");
    break;
  case WStype_PING:
    Serial.println("TTS 接收到 PING！");
    break;
  case WStype_PONG:
    Serial.println("TTS 接收到 PONG！");
    break;
  default:
    Serial.println("未知的 TTS 事件类型！");
    break;
  }
}

// ASR WebSocket 事件处理函数
void webSocketASREvent(WStype_t type, uint8_t *payload, size_t length)
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
  {
    Serial.printf("ASR 接收到文本：%s\n", payload);
    userInputText = String((char *)payload);
    // 将用户输入文本发送到讯飞 Spark4.0 Ultra 模型进行处理
    String sparkRequestPayload = "{\"text\":\"" + userInputText + "\",\"model\":\"Spark4.0 Ultra\"}";
    webSocketSpark.sendTXT(sparkRequestPayload);
    break;
  }
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

// Spark WebSocket 事件处理函数
void webSocketSparkEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.println("Spark 连接断开！");
    break;
  case WStype_CONNECTED:
    Serial.println("Spark 连接成功！");
    break;
  case WStype_TEXT:
  {
    Serial.printf("Spark 接收到文本：%s\n", payload);
    // 将 Spark4.0 Ultra 模型的回答发送到 TTS 服务进行语音合成
    String ttsRequestPayload = "{\"text\":\"" + String((char *)payload) + "\",\"voice\":\"xiaoyan\",\"speed\":50,\"volume\":50,\"pitch\":50,\"engine_type\":\"intp65\"}";
    webSocketTTS.sendTXT(ttsRequestPayload);
    break;
  }
  case WStype_BIN:
    Serial.printf("Spark 接收到二进制数据，长度：%u\n", length);
    break;
  case WStype_ERROR:
    Serial.println("Spark 发生错误！");
    break;
  case WStype_PING:
    Serial.println("Spark 接收到 PING！");
    break;
  case WStype_PONG:
    Serial.println("Spark 接收到 PONG！");
    break;
  default:
    Serial.println("未知的 Spark 事件类型！");
    break;
  }
}

void initWebSocketConnections()
{
  // 获取当前时间戳
  time_t now = time(nullptr);
  struct tm *timeinfo = gmtime(&now);

  // 初始化 ASR WebSocket 连接并将鉴权信息写入 URL 
  String asrHost = "iat-api.xfyun.cn";
  String asrPath = "/v2/iat";
  String asrAuthUrl = AuthUtils::assembleAuthUrl(asrHost, asrPath, apiKey, apiSecret,  *timeinfo);
  webSocketASR.setReconnectInterval(5000); // 设置重连间隔
  webSocketASR.beginSSL(asrHost.c_str(), 443, (asrPath + asrAuthUrl).c_str(), "", ""); // 开始 SSL 连接
  webSocketASR.onEvent(webSocketASREvent);

  // // 初始化 TTS WebSocket 连接并将鉴权信息写入 URL 
  // String ttsHost = "tts-api.xfyun.cn";
  // String ttsPath = "/v2/tts";
  // String ttsAuthUrl = AuthUtils::assembleAuthUrl(ttsHost, ttsPath, apiKey, apiSecret,  *timeinfo);
  // webSocketTTS.setReconnectInterval(5000);
  // webSocketTTS.beginSSL(ttsHost.c_str(), 443, (ttsPath + ttsAuthUrl).c_str(), "", "");
  // webSocketTTS.onEvent(webSocketTTSEvent);

  // // 初始化 Spark WebSocket 连接并将鉴权信息写入 URL 
  // String sparkHost = "spark-api.xf-yun.com";
  // String sparkPath = "/v4.0/chat";
  // String sparkAuthUrl = AuthUtils::assembleAuthUrl(sparkHost, sparkPath, apiKey, apiSecret,  *timeinfo);
  // webSocketSpark.setReconnectInterval(5000);
  // webSocketSpark.beginSSL(sparkHost.c_str(), 443, (sparkPath + sparkAuthUrl).c_str(), "", "");
  // webSocketSpark.onEvent(webSocketSparkEvent);
}

void handleWebSocketLoops()
{
  webSocketTTS.loop();
  webSocketASR.loop();
  webSocketSpark.loop();
}

void sendASRData(uint8_t *data, size_t length)
{
  // 构建有效的 JSON 字符串
  String jsonData = "{\"common\":{\"app_id\":\"" + String(appId) + "\"},";
  jsonData += "\"business\":{\"language\":\"zh_cn\",\"domain\":\"iat\",\"accent\":\"mandarin\"},";
  jsonData += "\"data\":{\"status\":1,\"format\":\"audio/L16;rate=16000\",\"encoding\":\"raw\",\"audio\":\"";

  // 将音频数据转换为 base64 编码
  String audioBase64 = base64::encode(data, length);
  jsonData += audioBase64 + "\"}}";

  // 发送数据到 ASR 服务
  webSocketASR.sendTXT(jsonData);
}

// 获取RFC1123格式时间
String getRFC1123Time()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    return "";
  }
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &timeinfo);
  return String(buffer);
}