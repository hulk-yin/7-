#include "audio_stream.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"

#define TAG "AUDIO_STREAM"

void audio_stream_init()
{
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2s_stream_writer, mp3_decoder, http_stream_reader;

    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_decoder = mp3_decoder_init(&mp3_cfg);

    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.type = AUDIO_STREAM_READER;
    http_cfg.uri = "https://m801.music.126.net/20250212234933/5576c4f0bdc25302eae3b4235e13bf72/jdymusic/obj/wo3DlMOGwrbDjj7DisKw/57315487974/fb46/0c75/7898/537f7193c604462bdb82d531b583b9a1.mp3";
    http_stream_reader = http_stream_init(&http_cfg);

    audio_pipeline_register(pipeline, http_stream_reader, "http");
    audio_pipeline_register(pipeline, mp3_decoder, "mp3");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    const char *link_tag[3] = {"http", "mp3", "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 3);

    audio_pipeline_run(pipeline);
}