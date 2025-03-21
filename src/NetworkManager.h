#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <sys/time.h>
#include "esp_sntp.h"
#include "OLEDDisplay.h"

class NetworkManager {
private:
    const char* ssid;
    const char* password;
    OLEDDisplay* display;
    
    // 内部方法，同步时间
    bool syncTime();

public:
    NetworkManager(const char* ssid, const char* password, OLEDDisplay* display);
    
    // 初始化并连接WiFi
    bool begin();
    
    // 获取网络连接状态
    bool isConnected();
    
    // 获取IP地址
    IPAddress getIP();
    
    // 获取当前时间信息
    bool getCurrentTime(struct tm* timeinfo);
};

#endif // NETWORK_MANAGER_H
