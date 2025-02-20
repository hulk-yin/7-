#include "WebSocketHandlers.h"
#include "ASRClient.h"
#include "TTSClient.h"
#include "SparkClient.h"
#include <Arduino.h>

// 科大讯飞 API 的鉴权信息
const char *appId = "57e5c152";
const char *apiSecret = "2b4d77de806ece14675de7ed359cd995";
const char *apiKey = "e92cfdec778718ef7348c8d4fb303893";

ASRClient asrClient(appId, apiKey, apiSecret);
TTSClient ttsClient(appId, apiKey, apiSecret);
SparkClient sparkClient(appId, apiKey, apiSecret);

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

void initWebSocketConnections()
{
  asrClient.init();
  ttsClient.init();
  sparkClient.init();
}

void handleWebSocketLoops()
{
  asrClient.loop();
  ttsClient.loop();
  sparkClient.loop();
}