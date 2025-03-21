#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H

#include <Arduino.h>
#include <driver/i2s.h>
#include "ASRClient.h"  // 添加ASRClient头文件

// I2S配置
#define I2S_NUM           I2S_NUM_0    // I2S接口号
#define I2S_SAMPLE_RATE   16000        // 采样率 16KHz
#define I2S_SAMPLE_BITS   16           // 样本位数
#define I2S_BUFFER_SIZE   512          // 缓冲区大小
#define I2S_BUFFER_COUNT  8            // DMA缓冲区数量

// 噪声过滤模式
enum NoiseFilterMode {
  FILTER_NONE = 0,    // 不过滤
  FILTER_LOWPASS,     // 低通滤波(去除高频噪声)
  FILTER_HIGHPASS,    // 高通滤波(去除低频噪声)
  FILTER_BANDPASS,    // 带通滤波(保留特定频率)
  FILTER_NOISE_GATE   // 噪声门限(抑制低于阈值的噪声)
};

class AudioProcessor {
public:
    AudioProcessor(int sckPin, int wsPin, int sdPin, int bufferSize = I2S_BUFFER_SIZE);
    void init();
    void captureAudio();
    
    // 获取音频数据
    int16_t* getAudioBuffer();
    size_t getBufferSize();
    
    // 音频处理功能
    void processAudio();
    uint8_t* getWaveformData(int width, int height);
    
    // 音量检测
    float getVolume();
    bool isAudioActive(float threshold = 500.0);
    
    // 新增增益控制方法
    void setGain(float gain);
    float getGain();
    
    // 音量显示放大系数控制
    void setVolumeMultiplier(float multiplier);
    float getVolumeMultiplier();
    
    // 噪声过滤控制
    void setNoiseFilter(NoiseFilterMode mode);
    NoiseFilterMode getNoiseFilterMode();
    void setNoiseGateThreshold(float threshold);
    float getNoiseGateThreshold();
    void setFilterAlpha(float alpha);
    float getFilterAlpha();
    
    // 添加ASR相关方法
    void setASRClient(ASRClient* client);
    void startSpeechRecognition();
    void stopSpeechRecognition();
    void sendAudioToASR();
    void updateASR();
    
    // 语音识别结果相关方法
    String getRecognizedText();
    void clearRecognizedText();
    void setRecognizedText(const String& text);
    bool isRecognizing() const;
    
private:
    // I2S引脚配置
    int bclkPin;  // 位时钟 (SCK)
    int lrclkPin; // 左右声道时钟 (WS)
    int dataPin;  // 数据线 (SD)
    
    // 数据缓冲区
    int bufferSize;
    int16_t* audioBuffer;
    uint8_t* waveformBuffer;
    
    // I2S配置
    i2s_config_t i2s_config;
    i2s_pin_config_t i2s_pin_config;
    
    // 计算音频波形
    void calculateWaveform(int width, int height);
    void applyGain(); // 新增方法用于应用增益
    
    float gainFactor; // 增益系数
    float volumeMultiplier; // 音量显示放大系数
    
    // 噪声过滤
    NoiseFilterMode filterMode;
    float noiseGateThreshold;  // 噪声门限阈值
    float filterAlpha;         // 滤波器系数 (0.0-1.0)
    int16_t prevFiltered;      // 前一个滤波输出值
    
    // 噪声过滤方法
    void applyNoiseFilter();
    void applyLowPassFilter();
    void applyHighPassFilter();
    void applyNoiseGate();
    
    // ASR相关属性
    ASRClient* asrClient;
    String recognizedText;
    bool recognitionActive;
    unsigned long lastASRUpdate;
    unsigned long asrUpdateInterval;
    
    // 预处理音频数据用于ASR
    void prepareAudioForASR();
    uint8_t* asrAudioBuffer;
    size_t asrBufferSize;
};

#endif // AUDIO_PROCESSOR_H
