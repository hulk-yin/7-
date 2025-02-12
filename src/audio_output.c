#include "audio_output.h"
#include "driver/dac.h"
#include "esp_log.h"

#define AUDIO_OUTPUT_DAC_CHANNEL DAC_CHANNEL_1  // GPIO25 对应 DAC1

void audio_output_init()
{
    // 初始化 DAC 通道
    dac_output_enable(AUDIO_OUTPUT_DAC_CHANNEL);
}

void write_audio_output(int audio_signal)
{
    // 将音频信号写入 DAC 输出
    dac_output_voltage(AUDIO_OUTPUT_DAC_CHANNEL, audio_signal);
}