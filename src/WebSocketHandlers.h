#ifndef WEBSOCKETHANDLERS_H
#define WEBSOCKETHANDLERS_H

#include <Arduino.h> // 包含 uint8_t 和其他 Arduino 类型定义
#include <cstddef>   // 包含 size_t 类型定义

void initWebSocketConnections();
void handleWebSocketLoops();
void sendASRData(uint8_t *data, size_t length);

#endif // WEBSOCKETHANDLERS_H
