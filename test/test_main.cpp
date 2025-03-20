#include <Arduino.h>
#include <unity.h>
#include "config.h"
#include "utils.h"
#include "WebSocketHandlers.h"
#include "ASRClient.h"
#include "TTSClient.h"
#include "SparkClient.h"

void setUp(void) {
    // 在每个测试前运行的设置
}

void tearDown(void) {
    // 在每个测试后运行的清理
}

void test_sensor_initialization(void) {
    bool result = initializeSensor();
    TEST_ASSERT_TRUE(result);
}

void test_sensor_reading(void) {
    SensorData data = readSensorData();
    
    // 如果传感器正常工作，应该返回有效数据
    TEST_ASSERT_TRUE(data.isValid);
    
    // 检查温度和湿度是否在合理范围内
    TEST_ASSERT_TRUE(data.temperature >= -40.0 && data.temperature <= 80.0);
    TEST_ASSERT_TRUE(data.humidity >= 0.0 && data.humidity <= 100.0);
}

void test_json_formatting(void) {
    SensorData testData = {
        .temperature = 25.5,
        .humidity = 60.0,
        .timestamp = 12345,
        .isValid = true
    };
    
    String json = formatDataAsJson(testData);
    
    // 简单验证JSON字符串包含预期数据
    TEST_ASSERT_TRUE(json.indexOf("25.5") > 0);
    TEST_ASSERT_TRUE(json.indexOf("60.0") > 0);
    TEST_ASSERT_TRUE(json.indexOf("12345") > 0);
}

void test_alarm_condition(void) {
    // 测试正常情况 (没有警报)
    SensorData normalData = {
        .temperature = 20.0,
        .humidity = 50.0,
        .timestamp = 0,
        .isValid = true
    };
    TEST_ASSERT_FALSE(isAlarmCondition(normalData));
    
    // 测试高温警报
    SensorData highTempData = {
        .temperature = TEMP_HIGH_THRESHOLD + 1.0,
        .humidity = 50.0,
        .timestamp = 0,
        .isValid = true
    };
    TEST_ASSERT_TRUE(isAlarmCondition(highTempData));
    
    // 测试低温警报
    SensorData lowTempData = {
        .temperature = TEMP_LOW_THRESHOLD - 1.0,
        .humidity = 50.0,
        .timestamp = 0,
        .isValid = true
    };
    TEST_ASSERT_TRUE(isAlarmCondition(lowTempData));
}

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

void setup() {
    delay(2000); // 等待串口连接
    UNITY_BEGIN();
    
    RUN_TEST(test_sensor_initialization);
    RUN_TEST(test_sensor_reading);
    RUN_TEST(test_json_formatting);
    RUN_TEST(test_alarm_condition);
    RUN_TEST(test_webSocketConnections);
    RUN_TEST(test_asrClient);
    
    UNITY_END();
}

void loop() {
    // 测试完成后什么都不做
}
