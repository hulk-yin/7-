#include "wifi_setup.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

// 事件处理程序
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI("wifi", "Wi-Fi STA 启动，尝试连接...");
        esp_wifi_connect();
        
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI("wifi", "Wi-Fi STA 断开连接，尝试重新连接...");
        esp_wifi_connect();
        ESP_LOGI("wifi", "重试连接到 AP");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("wifi", "获取到 IP:" IPSTR, IP2STR(&event->ip_info.ip));
    }
}

// Wi-Fi 设置
void wifi_setup()
{
    // 初始化 TCP/IP 网络接口
    ESP_ERROR_CHECK(esp_netif_init());

    // 初始化 Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 创建默认的 Wi-Fi station
    // Wi-Fi station 是指设备作为客户端连接到 Wi-Fi 网络
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // 设置主机名
    ESP_ERROR_CHECK(esp_netif_set_hostname(sta_netif, "7#-Robot"));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    // 注册 Wi-Fi 事件的事件处理程序
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
            .sta = {
                    .ssid = "Xiaomi_73A0_Wi-Fi5",
                    .password = "03031902",
            },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}