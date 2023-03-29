#include "ae/audio.h"
#include "ae/array.h"

#define DR_WAV_IMPLEMENTATION
#include "ae/libs/dr_wav.h"

static PaStream *_audio_stream = NULL;
static audio_channel_t **audio_channels = NULL;

static void free_audio_instance(audio_instance_t *inst) {
    if (inst->owns_sample)
        delete_audio_sample(&inst->sample);
    inst->sample = NULL;
    inst->user_data = NULL;
    inst->state = AU_STOPPED;
    inst->callback = NULL;
}

static int32_t audio_cb(
    const void *input_buffer,
    void *output_buffer,
    unsigned long frame_count,
    const PaStreamCallbackTimeInfo* time_info,
    PaStreamCallbackFlags status_flags,
    void *user_data
) {
    float *out = output_buffer;

    memset(out, 0, sizeof(float) * frame_count * 2);

    uint32_t f, ci, i;
    audio_instance_t *instance;
    for (f = 0; f < frame_count * 2; f++) {
        for (ci = 0; ci < arr_len(audio_channels); ci++) {
            if (!audio_channels[ci]) continue;
            if (audio_channels[ci]->state != AU_PLAYING) continue;
            for (i = 0; i < arr_len(audio_channels[ci]->instances); i++) {
                instance = &audio_channels[ci]->instances[i];
                if (instance->state == AU_STREAMING || instance->state == AU_PLAYING) {
                    if (instance->sample->channels == 1 && f % 2 == 1) {
                        out[f] = out[f - 1];
                        continue;
                    }
                    if (!instance->mix.pitch) {
                        free_audio_instance(instance);
                        continue;
                    }
                    if (instance->position < instance->sample->length) {
                        if (instance->mix.gain) {
                            out[f] += instance->sample->data[(unsigned long long)instance->position] * instance->mix.gain * audio_channels[ci]->gain;
                            if (f % 2 == 1) out[f] *= 0.5f + instance->mix.pan;
                            else out[f] *= 0.5f - instance->mix.pan;
                        }
                        instance->position += (instance->sample->sample_rate / (float)AUDIO_SAMPLE_RATE) * instance->mix.pitch;
                    }
                    else if (instance->state == AU_STREAMING && instance->callback) {
                        if (!instance->callback(instance->sample, &instance->mix, instance->user_data)) {
                            free_audio_instance(instance);
                            continue;
                        }
                        instance->position = 0;
                    }
                    else free_audio_instance(instance);
                }
            }
        }
    }

    return paContinue;
}

static bool _audio_init() {
    Pa_Initialize();
    Pa_OpenDefaultStream(
        &_audio_stream, 0, 2, paFloat32, AUDIO_SAMPLE_RATE, 512, audio_cb, NULL
    );
    Pa_StartStream(_audio_stream);
    return true;
}

static void _audio_shutdown() {
    Pa_AbortStream(_audio_stream);
    Pa_Terminate();
}

audio_sample_t* new_audio_sample(float *data, size_t length, uint32_t sample_rate) {
    audio_sample_t *sample = malloc(sizeof(audio_sample_t));
    if (data) {
        size_t len = sizeof(float) * length;
        sample->data = malloc(len);
        memcpy(sample->data, data, len);
    }
    else sample->data = NULL;
    sample->length = length;
    sample->sample_rate = sample_rate ? sample_rate : AUDIO_SAMPLE_RATE;
    sample->channels = 0;
    return sample;
}

void delete_audio_sample(audio_sample_t **sample) {
    if (!(sample && *sample)) return;
    if ((*sample)->data) free((*sample)->data);
    free(*sample);
    *sample = NULL;
}

bool load_audio_sample(audio_sample_t *sample, const char *path) {
    if (!sample) return false;
    if (!path) return false;
    //if (extcmp(path, "wav")) return false;

    float *data = drwav_open_file_and_read_pcm_frames_f32(path, (unsigned int*)&sample->channels, &sample->sample_rate, (drwav_uint64*)&sample->length);
    if (!data) {
        drwav_free(data);
        return false;
    }

    size_t len = sizeof(float) * sample->length;
    sample->data = realloc(sample->data, len);
    memcpy(sample->data, data, len);

    drwav_free(data);

    return true;
}

audio_channel_t* new_audio_channel() {
    audio_channel_t *new = calloc(1, sizeof(audio_channel_t));
    new->gain = 1;
    new->state = AU_STOPPED;
    new->instances = arr_new(sizeof(audio_instance_t), 10, 1);
    if (!audio_channels) {
        audio_channels = arr_new(sizeof(audio_channel_t), 1, 1);
        _audio_init();
    }
    arr_add(audio_channels, new);
    return new;
}

void delete_channel(audio_channel_t **channel) {
    if (!(channel && *channel)) return;
    for (uint32_t i = 0; i < arr_len(audio_channels); i++)
        if (audio_channels[i] == *channel)
            arr_remove(audio_channels, i);
    if (arr_len(audio_channels)) {
        arr_delete(audio_channels);
        _audio_shutdown();
    }
    arr_delete((*channel)->instances);
    free(*channel);
    *channel = NULL;
}

audio_instance_t* get_audio_instance(audio_channel_t *channel) {
    if (!channel) return NULL;
    uint32_t i = 0;
    for (; i < arr_len(channel->instances); i++) {
        if (channel->instances[i].state == AU_STOPPED)
            return &channel->instances[i];
    }
    audio_instance_t inst = {};
    arr_add(channel->instances, inst);
    return &channel->instances[arr_len(channel->instances) - 1];
}

audio_instance_t* audio_play(audio_channel_t *channel, audio_sample_t *sample, float gain, float pitch, float pan) {
    if (!(sample && channel)) return NULL;

    audio_instance_t *inst = get_audio_instance(channel);
    if (!inst) return NULL;

    inst->sample = sample;
    inst->owns_sample = false;
    inst->callback = NULL;
    inst->position = 0;
    inst->mix = (audio_mix_t){ gain, pitch, pan };
    inst->state = AU_PLAYING;
    inst->user_data = NULL;

    if (channel->state == AU_STOPPED)
        channel->state = AU_PLAYING;
    
    return inst;
}

audio_instance_t* audio_stream(audio_channel_t *channel, audio_callback_t callback, float gain, float pitch, float pan, void *user_data) {
    if (!(callback && channel)) return NULL;

    audio_instance_t *inst = get_audio_instance(channel);
    if (!inst) return NULL;

    inst->sample = new_audio_sample(NULL, 0, AUDIO_SAMPLE_RATE);
    inst->owns_sample = true;
    inst->callback = callback;
    inst->position = 0;
    inst->mix = (audio_mix_t){ gain, pitch, pan };
    inst->state = AU_STREAMING;
    inst->user_data = user_data;

    if (channel->state == AU_STOPPED)
        channel->state = AU_PLAYING;
    
    return inst;
}

audio_instance_t* audio_stream_sample(audio_channel_t *channel, audio_sample_t *sample, audio_callback_t callback, float gain, float pitch, float pan, void *user_data) {
    if (!sample) return NULL;
    if (!callback) return NULL;

    audio_instance_t *inst = get_audio_instance(channel);
    if (!inst) return NULL;

    inst->sample = sample;
    inst->owns_sample = false;
    inst->callback = callback;
    inst->position = 0;
    inst->mix = (audio_mix_t){ gain, pitch, pan };
    inst->state = AU_STREAMING;
    inst->user_data = user_data;

    if (channel->state == AU_STOPPED)
        channel->state = AU_PLAYING;
    
    return inst;
}

bool set_audio_state(audio_channel_t *channel, audio_sample_t *sample, audio_state_t state) {
    if (!channel) return false;
    uint32_t i;
    if (!sample) {
        channel->state = state;
        if (state == AU_STOPPED)
            for (i = 0; i < arr_len(channel->instances); i++)
                free_audio_instance(&channel->instances[i]);
        return true;
    }
    bool success = false;
    for (i = 0; i < arr_len(channel->instances); i++) {
        if (channel->instances[i].sample == sample) {
            channel->instances[i].state = state;
            if (state == AU_STOPPED)
                free_audio_instance(&channel->instances[i]);
            success = true;
        }
    }
    return success;
}