#include "../include/utils.h"
#include "utils.h"
#include <ArduinoJson.h>

#if SENSOR_TYPE == DHT22
  #include <DHT.h>
  DHT dht(SENSOR_PIN, DHT22);
#endif

#if BLE_ENABLED
  #include <BLEDevice.h>
  #include <BLEServer.h>
  #include <BLEUtils.h>
  #include <BLE2902.h>
  
  BLEServer *pServer = NULL;
  BLECharacteristic *pCharacteristic = NULL;
  bool deviceConnected = false;
  
  #define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
  #define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
  
  // BLE服务器回调类
  class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    }
    
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
  };
#endif

bool debounceButton(uint8_t pin, unsigned long debounceTime) {
  static uint8_t lastButtonState = HIGH;
  static unsigned long lastDebounceTime = 0;
  bool buttonState = digitalRead(pin);
  
  if (buttonState != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceTime) {
    if (buttonState == LOW) {
      lastButtonState = buttonState;
      return true;
    }
  }
  
  lastButtonState = buttonState;
  return false;
}

void safeDelay(unsigned long ms) {
  unsigned long startTime = millis();
  while (millis() - startTime < ms) {
    yield(); // 允许其他任务执行
  }
}

int safeStringToInt(const String &str, int defaultValue) {
  if (str.length() == 0) {
    return defaultValue;
  }
  
  for (unsigned int i = 0; i < str.length(); i++) {
    if (i == 0 && str[i] == '-') continue;
    if (!isdigit(str[i])) return defaultValue;
  }
  
  return str.toInt();
}

bool initializeSensor() {
#if SENSOR_TYPE == DHT22
  dht.begin();
  delay(2000); // 给DHT一些启动时间
  return true;
#else
  return false;
#endif
}

SensorData readSensorData() {
  SensorData data;
  data.timestamp = millis();
  data.isValid = false;

#if SENSOR_TYPE == DHT22
  data.temperature = dht.readTemperature();
  data.humidity = dht.readHumidity();
  
  // 检查读取是否成功
  if (!isnan(data.temperature) && !isnan(data.humidity)) {
    data.isValid = true;
  }
#endif

  return data;
}

void printSensorData(const SensorData &data) {
  if (data.isValid) {
    Serial.println("=========传感器数据=========");
    Serial.print("温度: ");
    Serial.print(data.temperature);
    Serial.println(" °C");
    Serial.print("湿度: ");
    Serial.print(data.humidity);
    Serial.println(" %");
    Serial.print("时间戳: ");
    Serial.println(data.timestamp);
    Serial.println("==========================");
  }
}

bool isAlarmCondition(const SensorData &data) {
  if (!data.isValid) {
    return false;
  }
  
  // 检查是否超出阈值
  if (data.temperature > TEMP_HIGH_THRESHOLD || 
      data.temperature < TEMP_LOW_THRESHOLD ||
      data.humidity > HUMID_HIGH_THRESHOLD ||
      data.humidity < HUMID_LOW_THRESHOLD) {
    return true;
  }
  
  return false;
}

void setupBLE() {
#if BLE_ENABLED
  // 初始化BLE设备
  BLEDevice::init(DEVICE_NAME);
  BLEDevice::setPower(BLE_TX_POWER);
  
  // 创建BLE服务器
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  // 创建BLE服务
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  // 创建BLE特征
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  
  pCharacteristic->addDescriptor(new BLE2902());
  
  // 启动服务
  pService->start();
  
  // 开始广播
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE初始化完成, 等待连接...");
#endif
}

void broadcastData(const SensorData &data) {
#if BLE_ENABLED
  if (deviceConnected && data.isValid) {
    String jsonData = formatDataAsJson(data);
    pCharacteristic->setValue(jsonData.c_str());
    pCharacteristic->notify();
    Serial.println("BLE数据已发送");
  }
#endif
}

void enterSleepMode(unsigned long duration) {
#ifdef ESP32
  esp_sleep_enable_timer_wakeup(duration * 1000);
  esp_deep_sleep_start();
#else
  delay(duration);
#endif
}

String formatDataAsJson(const SensorData &data) {
  StaticJsonDocument<200> doc;
  
  doc["temperature"] = data.temperature;
  doc["humidity"] = data.humidity;
  doc["timestamp"] = data.timestamp;
  doc["firmware"] = FIRMWARE_VERSION;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  return jsonString;
}
