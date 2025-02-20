#ifndef TTS_CLIENT_H
#define TTS_CLIENT_H

#include <Arduino.h>
#include <WebSocketsClient.h>

class TTSClient {
public:
    TTSClient(const char* appId, const char* apiKey, const char* apiSecret);
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

#endif // TTS_CLIENT_H
