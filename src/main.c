#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "setup.h"
#include "wifi_setup.h"
#include "audio_input.h"
#include "audio_output.h"
#include "audio_stream.h"

void app_main()
{
  // 调用 setup_nvs_flash 函数
  setup_nvs_flash();

  // 创建默认事件循环
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // 调用 wifi_setup 函数
  wifi_setup();

  // 初始化音频输入
  audio_input_init();

  // 初始化音频输出
  audio_output_init();

  // 初始化音频流
  audio_stream_init();

  // while (1) {
    // 读取音频输入信号
    // int audio_signal = read_audio_input();
    // ESP_LOGI("audio", "Audio signal: %d", audio_signal);

    // 打印二进制流
    // ESP_LOG_BUFFER_HEXDUMP("audio_binary", &audio_signal, sizeof(audio_signal), ESP_LOG_INFO);

    // 输出音频信号
    // write_audio_output(audio_signal);

    // 延时一段时间，避免过多日志输出
    // vTaskDelay(pdMS_TO_TICKS(1000));
  // }
}
