#include "SparkClient.h"
#include <time.h>
#include "AuthUtils.h"

// 初始化 Spark WebSocket 客户端
WebSocketsClient webSocketSpark;

// 科大讯飞 API 的鉴权信息
extern const char *appId;
extern const char *apiSecret;
extern const char *apiKey;

SparkClient::SparkClient(const char* appId, const char* apiKey, const char* apiSecret)
    : appId(appId), apiKey(apiKey), apiSecret(apiSecret) {}

void SparkClient::webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
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
        Serial.printf("Spark 接收到文本：%s\n", payload);
        break;
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

void SparkClient::init()
{
    // 获取当前时间戳
    time_t now = time(nullptr);
    struct tm *timeinfo = gmtime(&now);

    // 组装鉴权 URL
    String sparkHost = "spark-api.xf-yun.com";
    String sparkPath = "/v4.0/chat";
    String sparkAuthUrl = AuthUtils::assembleAuthUrl(sparkHost, sparkPath, apiKey, apiSecret, *timeinfo);

    // 初始化 WebSocket 连接并将鉴权信息写入 URL
    webSocket.setReconnectInterval(5000); // 设置重连间隔
    webSocket.beginSSL(sparkHost.c_str(), 443, (sparkPath + sparkAuthUrl).c_str(), "", ""); // 开始 SSL 连接
    webSocket.onEvent(webSocketEvent);
}

void SparkClient::loop()
{
    webSocket.loop();
}

void SparkClient::sendText(const String &text)
{
    String payload = "{\"text\":\"" + text + "\",\"model\":\"Spark4.0 Ultra\"}";
    webSocket.sendTXT(payload);
}
