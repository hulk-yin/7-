#include "TTSClient.h"
#include <time.h>
#include "AuthUtils.h"

TTSClient::TTSClient(const char* appId, const char* apiKey, const char* apiSecret)
    : appId(appId), apiKey(apiKey), apiSecret(apiSecret) {}

void TTSClient::webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
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

void TTSClient::init()
{
    // 获取当前时间戳
    time_t now = time(nullptr);
    struct tm *timeinfo = gmtime(&now);

    // 组装鉴权 URL
    String ttsHost = "tts-api.xfyun.cn";
    String ttsPath = "/v2/tts";
    String ttsAuthUrl = AuthUtils::assembleAuthUrl(ttsHost, ttsPath, apiKey, apiSecret, *timeinfo);

    // 初始化 WebSocket 连接并将鉴权信息写入 URL
    webSocket.setReconnectInterval(5000); // 设置重连间隔
    webSocket.beginSSL(ttsHost.c_str(), 443, (ttsPath + ttsAuthUrl).c_str(), "", ""); // 开始 SSL 连接
    webSocket.onEvent(webSocketEvent);
}

void TTSClient::loop()
{
    webSocket.loop();
}

void TTSClient::sendText(const String &text)
{
    String payload = "{\"text\":\"" + text + "\",\"voice\":\"xiaoyan\",\"speed\":50,\"volume\":50,\"pitch\":50,\"engine_type\":\"intp65\"}";
    webSocket.sendTXT(payload);
}
