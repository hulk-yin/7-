#ifndef ASR_CLIENT_H
#define ASR_CLIENT_H

#include <Arduino.h>
#include <WebSocketsClient.h>

class ASRClient
{
public:
    ASRClient(const char *appId, const char *apiKey, const char *apiSecret);
    void init();
    void loop();
    void sendData(uint8_t *data, size_t length);

private:
    WebSocketsClient webSocket;
    const char *appId;
    const char *apiKey;
    const char *apiSecret;

    static void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
};

#endif // ASR_CLIENT_H
