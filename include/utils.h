#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "config.h"

// 数据结构定义
struct SensorData {
    float temperature;
    float humidity;
    unsigned long timestamp;
    bool isValid;
};

// 工具函数声明
bool initializeSensor();
SensorData readSensorData();
void printSensorData(const SensorData &data);
bool isAlarmCondition(const SensorData &data);
void setupBLE();
void broadcastData(const SensorData &data);
void enterSleepMode(unsigned long duration);
String formatDataAsJson(const SensorData &data);

// 按钮去抖动函数
bool debounceButton(uint8_t pin, unsigned long debounceTime);

// 安全延迟函数（可以在中断中使用）
void safeDelay(unsigned long ms);

// 字符串转数值的安全函数
int safeStringToInt(const String &str, int defaultValue = 0);

#endif // UTILS_H
