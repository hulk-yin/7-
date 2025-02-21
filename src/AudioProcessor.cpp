#include "AudioProcessor.h"

AudioProcessor::AudioProcessor(int pin, int bufferSize)
    : audioPin(pin), bufferSize(bufferSize)
{
  audioBuffer = new uint8_t[bufferSize];
}

void AudioProcessor::init()
{
  pinMode(audioPin, INPUT);
}

void AudioProcessor::captureAudio()
{
  // 采样率为 8kHz 的单声道音频
  for (int i = 0; i < bufferSize; i++)
  {
    audioBuffer[i] = analogRead(audioPin) >> 4; // 将 12 位 ADC 值缩小到 8 位
    delayMicroseconds(125);                     // 确保采样率为 8kHz
  }
}

uint8_t *AudioProcessor::getAudioBuffer()
{
  return audioBuffer;
}

size_t AudioProcessor::getBufferSize()
{
  return bufferSize;
}
