#include "AudioProcessor.h"

AudioProcessor::AudioProcessor(int sckPin, int wsPin, int sdPin, int bufferSize)
    : bclkPin(sckPin), lrclkPin(wsPin), dataPin(sdPin), bufferSize(bufferSize), 
      gainFactor(1.0f), volumeMultiplier(10.0f), filterMode(FILTER_NONE), 
      noiseGateThreshold(500.0f), filterAlpha(0.2f), prevFiltered(0)
{
  // 分配内存
  audioBuffer = new int16_t[bufferSize];
  waveformBuffer = new uint8_t[bufferSize];
  
  // 清空缓冲区
  memset(audioBuffer, 0, bufferSize * sizeof(int16_t));
  memset(waveformBuffer, 0, bufferSize * sizeof(uint8_t));
  
  // 配置I2S
  i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t)I2S_SAMPLE_BITS,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = I2S_BUFFER_COUNT,
    .dma_buf_len = I2S_BUFFER_SIZE,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  
  i2s_pin_config = {
    .bck_io_num = bclkPin,
    .ws_io_num = lrclkPin,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = dataPin
  };
}

void AudioProcessor::init()
{
  // 确保Serial已经准备好
  if (!Serial) {
    Serial.begin(115200);
    delay(100); // 等待Serial稳定
  }
  
  // 安装I2S驱动
  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &i2s_pin_config);
  
  // 启动I2S
  i2s_start(I2S_NUM);
  
  // 发送更详细的启动信息
  Serial.print("\n\n");
  Serial.println("==================================");
  Serial.println("I2S麦克风初始化完成");
  Serial.print("采样率: ");
  Serial.print(I2S_SAMPLE_RATE);
  Serial.println(" Hz");
  Serial.print("缓冲区大小: ");
  Serial.println(bufferSize);
  Serial.print("增益系数: ");
  Serial.println(gainFactor);
  Serial.print("音量放大系数: ");
  Serial.println(volumeMultiplier);
  Serial.print("噪声过滤模式: ");
  Serial.println(filterMode);
  Serial.println("==================================");
  Serial.flush(); // 确保消息被完全发送
  delay(50);      // 短暂延迟，确保显示完成
}

void AudioProcessor::captureAudio()
{
  size_t bytes_read = 0;
  
  // 从I2S接口读取数据
  i2s_read(I2S_NUM, audioBuffer, bufferSize * sizeof(int16_t), &bytes_read, portMAX_DELAY);
  
  // 处理数据 - INMP441的数据需要右移
  for (int i = 0; i < bufferSize; i++) {
    // 将24位数据转换为16位
    audioBuffer[i] = audioBuffer[i] >> 8;
  }
}

int16_t* AudioProcessor::getAudioBuffer()
{
  return audioBuffer;
}

size_t AudioProcessor::getBufferSize()
{
  return bufferSize;
}

void AudioProcessor::processAudio()
{
  // 应用噪声过滤
  applyNoiseFilter();
  
  // 可以在这里添加噪声过滤、增益控制等处理
  // 简单示例：应用简单的移动平均滤波
  int16_t prevSample = audioBuffer[0];
  for (int i = 1; i < bufferSize - 1; i++) {
    int16_t temp = audioBuffer[i];
    audioBuffer[i] = (prevSample + audioBuffer[i] + audioBuffer[i + 1]) / 3;
    prevSample = temp;
  }
  
  // 应用增益
  applyGain();
}

void AudioProcessor::applyGain()
{
  // 对每个样本应用增益
  for (int i = 0; i < bufferSize; i++) {
    int32_t amplifiedSample = (int32_t)(audioBuffer[i] * gainFactor);
    // 防止溢出
    if (amplifiedSample > 32767) {
      audioBuffer[i] = 32767;
    } else if (amplifiedSample < -32768) {
      audioBuffer[i] = -32768;
    } else {
      audioBuffer[i] = (int16_t)amplifiedSample;
    }
  }
}

void AudioProcessor::setGain(float gain)
{
  if (gain < 0.0f) gain = 0.0f;
  if (gain > 10.0f) gain = 10.0f; // 设置一个合理的上限
  gainFactor = gain;
}

float AudioProcessor::getGain()
{
  return gainFactor;
}

uint8_t* AudioProcessor::getWaveformData(int width, int height)
{
  calculateWaveform(width, height);
  return waveformBuffer;
}

void AudioProcessor::calculateWaveform(int width, int height)
{
  // 计算波形显示数据
  // 清空波形缓冲区
  memset(waveformBuffer, 0, bufferSize * sizeof(uint8_t));
  
  // 找出最大振幅以进行归一化
  int16_t maxAmplitude = 0;
  for (int i = 0; i < bufferSize; i++) {
    int16_t absValue = abs(audioBuffer[i]);
    if (absValue > maxAmplitude) {
      maxAmplitude = absValue;
    }
  }
  
  // 防止除以零
  if (maxAmplitude < 1) maxAmplitude = 1;
  
  // 计算每个点的位置
  int pointsPerPixel = bufferSize / width;
  if (pointsPerPixel < 1) pointsPerPixel = 1;
  
  for (int x = 0; x < width; x++) {
    // 计算这个像素位置的平均振幅
    int32_t sum = 0;
    for (int j = 0; j < pointsPerPixel; j++) {
      int idx = x * pointsPerPixel + j;
      if (idx < bufferSize) {
        sum += abs(audioBuffer[idx]);
      }
    }
    int16_t avg = sum / pointsPerPixel;
    
    // 归一化到显示高度
    uint8_t yPos = map(avg, 0, maxAmplitude, 0, height - 1);
    waveformBuffer[x] = yPos;
  }
}

float AudioProcessor::getVolume()
{
  // 计算当前音量水平 (RMS)
  float sum = 0;
  for (int i = 0; i < bufferSize; i++) {
    sum += audioBuffer[i] * audioBuffer[i];
  }
  // 应用音量放大系数
  return sqrt(sum / bufferSize) * volumeMultiplier;
}

void AudioProcessor::setVolumeMultiplier(float multiplier)
{
  if (multiplier < 1.0f) multiplier = 1.0f;
  if (multiplier > 100.0f) multiplier = 100.0f; // 设置合理上限
  volumeMultiplier = multiplier;
}

float AudioProcessor::getVolumeMultiplier()
{
  return volumeMultiplier;
}

bool AudioProcessor::isAudioActive(float threshold)
{
  // 注意：阈值需要考虑放大系数的影响
  return getVolume() > threshold;
}

void AudioProcessor::applyNoiseFilter()
{
  switch (filterMode) {
    case FILTER_LOWPASS:
      applyLowPassFilter();
      break;
    case FILTER_HIGHPASS:
      applyHighPassFilter();
      break;
    case FILTER_BANDPASS:
      // 先应用高通后应用低通实现带通
      applyHighPassFilter();
      applyLowPassFilter();
      break;
    case FILTER_NOISE_GATE:
      applyNoiseGate();
      break;
    case FILTER_NONE:
    default:
      // 不应用过滤
      break;
  }
}

void AudioProcessor::applyLowPassFilter()
{
  // 简单的一阶IIR低通滤波器: y[n] = α·x[n] + (1-α)·y[n-1]
  // α是平滑系数(0.0-1.0)，越小滤波效果越强，但可能导致信号延迟
  for (int i = 0; i < bufferSize; i++) {
    int16_t filtered = filterAlpha * audioBuffer[i] + (1.0f - filterAlpha) * prevFiltered;
    prevFiltered = filtered;
    audioBuffer[i] = filtered;
  }
}

void AudioProcessor::applyHighPassFilter()
{
  // 简单的一阶IIR高通滤波器: y[n] = α·(y[n-1] + x[n] - x[n-1])
  // 去除低频噪声和直流偏移
  static int16_t prevInput = 0;
  
  for (int i = 0; i < bufferSize; i++) {
    int16_t currentInput = audioBuffer[i];
    int16_t filtered = filterAlpha * (prevFiltered + currentInput - prevInput);
    prevInput = currentInput;
    prevFiltered = filtered;
    audioBuffer[i] = filtered;
  }
}

void AudioProcessor::applyNoiseGate()
{
  // 噪声门限：当信号幅度低于阈值时将其归零
  for (int i = 0; i < bufferSize; i++) {
    if (abs(audioBuffer[i]) < noiseGateThreshold) {
      audioBuffer[i] = 0;
    }
  }
}

void AudioProcessor::setNoiseFilter(NoiseFilterMode mode)
{
  filterMode = mode;
  // 切换滤波器时重置状态变量
  prevFiltered = 0;
}

NoiseFilterMode AudioProcessor::getNoiseFilterMode()
{
  return filterMode;
}

void AudioProcessor::setNoiseGateThreshold(float threshold)
{
  if (threshold < 0.0f) threshold = 0.0f;
  if (threshold > 10000.0f) threshold = 10000.0f;
  noiseGateThreshold = threshold;
}

float AudioProcessor::getNoiseGateThreshold()
{
  return noiseGateThreshold;
}

void AudioProcessor::setFilterAlpha(float alpha)
{
  if (alpha < 0.0f) alpha = 0.0f;
  if (alpha > 1.0f) alpha = 1.0f;
  filterAlpha = alpha;
}

float AudioProcessor::getFilterAlpha()
{
  return filterAlpha;
}

// 初始化ASR相关属性
void AudioProcessor::setASRClient(ASRClient* client) {
    this->asrClient = client;
    this->recognitionActive = false;
    this->recognizedText = "";
    this->lastASRUpdate = 0;
    this->asrUpdateInterval = 100; // 100ms更新间隔
    
    // 初始化ASR音频缓冲区
    this->asrBufferSize = 1280; // 与ASRClient的AUDIO_CHUNK_SIZE保持一致
    this->asrAudioBuffer = new uint8_t[this->asrBufferSize];
}

// 启动语音识别
void AudioProcessor::startSpeechRecognition() {
    if (!asrClient) return;
    
    this->recognitionActive = true;
    this->recognizedText = "正在聆听...";
    Serial.println("开始语音识别");
}

// 停止语音识别
void AudioProcessor::stopSpeechRecognition() {
    if (!asrClient) return;
    
    this->recognitionActive = false;
    Serial.println("停止语音识别");
}

// 更新ASR状态，定期调用
void AudioProcessor::updateASR() {
    if (!asrClient || !recognitionActive) return;
    
    unsigned long currentTime = millis();
    if (currentTime - lastASRUpdate >= asrUpdateInterval) {
        lastASRUpdate = currentTime;
        
        // 如果检测到有效音频，发送到ASR
        if (isAudioActive()) {
            sendAudioToASR();
        }
    }
    
    // 让ASRClient处理它的循环任务
    asrClient->loop();
}

// 准备并发送音频数据到ASR服务
void AudioProcessor::sendAudioToASR() {
    if (!asrClient || !recognitionActive) return;
    
    // 1. 将16位音频数据转换为8位数据（如果需要）
    prepareAudioForASR();
    
    // 2. 发送到ASR客户端
    asrClient->sendData(asrAudioBuffer, asrBufferSize);
}

// 预处理音频数据用于ASR
void AudioProcessor::prepareAudioForASR() {
    // 将16位音频数据转换为8位，或者进行其他必要的预处理
    // 这里简化处理，实际可能需要根据ASRClient的需求调整
    for (int i = 0; i < asrBufferSize && i < bufferSize; i++) {
        // 将16位数据转换为8位
        asrAudioBuffer[i] = (uint8_t)((audioBuffer[i] >> 8) + 128);
    }
}

// 获取识别文本
String AudioProcessor::getRecognizedText() {
    return recognizedText;
}

// 清除识别文本
void AudioProcessor::clearRecognizedText() {
    recognizedText = "";
}

// 设置识别文本（由ASRClient回调设置）
void AudioProcessor::setRecognizedText(const String& text) {
    recognizedText = text;
    Serial.println("识别结果: " + recognizedText);
}

// 检查是否正在识别
bool AudioProcessor::isRecognizing() const {
    return recognitionActive;
}
