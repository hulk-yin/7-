#ifndef SPARK_CLIENT_H
#define SPARK_CLIENT_H

#include <Arduino.h>
#include <WebSocketsClient.h>

class SparkClient {
public:
    SparkClient(const char* appId, const char* apiKey, const char* apiSecret);
    void init();
    void loop();
    void sendText(const String &text);

private:
    WebSocketsClient webSocket;
    const char* appId;
    const char* apiKey;
    const char* apiSecret;

    static void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
};

#endif // SPARK_CLIENT_H
