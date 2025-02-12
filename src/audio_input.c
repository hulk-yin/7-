#include "audio_input.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define AUDIO_INPUT_GPIO GPIO_NUM_34 // 定义音频输入的GPIO引脚

void audio_input_init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE; // 禁用中断
    io_conf.mode = GPIO_MODE_INPUT; // 设置为输入模式
    io_conf.pin_bit_mask = (1ULL << AUDIO_INPUT_GPIO); // 配置引脚掩码
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // 禁用下拉
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE; // 启用上拉
    gpio_config(&io_conf); // 应用配置
}

int read_audio_input()
{
    return gpio_get_level(AUDIO_INPUT_GPIO); // 读取音频输入引脚的电平
}