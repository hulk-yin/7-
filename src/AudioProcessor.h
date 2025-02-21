#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H

#include <Arduino.h>

class AudioProcessor {
public:
    AudioProcessor(int pin, int bufferSize);
    void init();
    void captureAudio();
    uint8_t* getAudioBuffer();
    size_t getBufferSize();

private:
    int audioPin;
    int bufferSize;
    uint8_t* audioBuffer;
};

#endif // AUDIO_PROCESSOR_H
