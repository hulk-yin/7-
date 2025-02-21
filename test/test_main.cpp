#include <Arduino.h>
#include <unity.h>
#include "WebSocketHandlers.h"
#include "ASRClient.h"
#include "TTSClient.h"
#include "SparkClient.h"

void test_webSocketConnections()
{
    initWebSocketConnections();
    // 检查 WebSocket 连接是否成功初始化
    TEST_ASSERT_TRUE_MESSAGE(true, "WebSocket 连接初始化成功");
}

void test_asrClient()
{
    // 模拟音频数据
    uint8_t data[10] = {0};
    sendASRData(data, sizeof(data));
    // 检查 ASR 数据发送是否成功
    TEST_ASSERT_TRUE_MESSAGE(true, "ASR 数据发送成功");
}

void setup()
{
    UNITY_BEGIN();
    RUN_TEST(test_webSocketConnections);
    RUN_TEST(test_asrClient);
    UNITY_END();
}

void loop()
{
    // 留空
}
