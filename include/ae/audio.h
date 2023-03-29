#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdint.h>
#include <stdbool.h>

#include "libs/dr_wav.h"
#include "libs/portaudio.h"

#ifndef AUDIO_SAMPLE_RATE
#define AUDIO_SAMPLE_RATE 44100
#endif

typedef enum {
    AU_STOPPED = 0,
    AU_PLAYING,
    AU_STREAMING,
    AU_PAUSED,
} audio_state_t;

typedef struct {
    float *data;
    size_t length;
    uint32_t sample_rate;
    uint8_t channels;
} audio_sample_t;

typedef struct {
    float gain, pitch, pan;
} audio_mix_t;

typedef int (*audio_callback_t)(audio_sample_t *sample, audio_mix_t *mix, void *user_data);

typedef struct audio_instance {
    audio_mix_t mix;
    float position;
    audio_state_t state;
    audio_callback_t callback;
    bool owns_sample;
    audio_sample_t *sample;
    void *user_data;
} audio_instance_t;

typedef struct {
    uint32_t id;
    float gain;
    audio_state_t state;
    audio_instance_t *instances;
} audio_channel_t;

audio_sample_t* new_audio_sample(float *data, size_t length, uint32_t sample_rate);
void delete_audio_sample(audio_sample_t **sample);
bool load_audio_sample(audio_sample_t *sample, const char *path);
audio_channel_t* new_audio_channel();
void delete_channel(audio_channel_t **channel);
audio_instance_t* get_audio_instance(audio_channel_t *channel);
audio_instance_t* audio_play(audio_channel_t *channel, audio_sample_t *sample, float gain, float pitch, float pan);
audio_instance_t* audio_stream(audio_channel_t *channel, audio_callback_t callback, float gain, float pitch, float pan, void *user_data);
audio_instance_t* audio_stream_sample(audio_channel_t *channel, audio_sample_t *sample, audio_callback_t callback, float gain, float pitch, float pan, void *user_data);
bool set_audio_state(audio_channel_t *channel, audio_sample_t *sample, audio_state_t state);

#endif