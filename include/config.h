#ifndef CONFIG_H
#define CONFIG_H

// 设备信息
#define DEVICE_NAME "ESP32_TempHumid"
#define FIRMWARE_VERSION "1.0.0"

// 传感器配置
#define SENSOR_TYPE DHT22           // 传感器类型
#define SENSOR_PIN 4                // 传感器连接的引脚
#define SAMPLING_INTERVAL 60000     // 采样间隔(毫秒)

// 蓝牙配置
#define BLE_ENABLED true            // 启用蓝牙功能
// 将整数值改为ESP32 BLE库中定义的esp_power_level_t枚举值
#define BLE_TX_POWER ESP_PWR_LVL_P3  // 蓝牙发射功率 (dBm)，使用有效的枚举值

// 报警阈值
#define TEMP_HIGH_THRESHOLD 30.0    // 温度高阈值(°C)
#define TEMP_LOW_THRESHOLD 10.0     // 温度低阈值(°C)
#define HUMID_HIGH_THRESHOLD 80.0   // 湿度高阈值(%)
#define HUMID_LOW_THRESHOLD 20.0    // 湿度低阈值(%)

// 低功耗设置
#define LOW_POWER_MODE true         // 启用低功耗模式
#define SLEEP_DURATION 50000        // 休眠时间(毫秒)

// 硬件配置
#define LED_PIN 13
#define BUTTON_PIN 2

// 功能参数
#define DEBOUNCE_TIME 50
#define SERIAL_BAUD_RATE 115200

// 网络配置（如适用）
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_RETRY 10

#endif // CONFIG_H
