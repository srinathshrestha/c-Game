/*
 * audio.h - procedural chiptune music
 * no wav/mp3 files, just math and sin waves
 */

#ifndef AUDIO_H
#define AUDIO_H

#include "defs.h"

#define AUDIO_SAMPLE_RATE  44100
#define AUDIO_CHANNELS     1
#define AUDIO_SAMPLES      1024       // buffer size per callback
#define AUDIO_VOLUME       0.15f      // keep it chill

#define BPM                140
#define BEAT_SAMPLES       (AUDIO_SAMPLE_RATE * 60 / BPM)

typedef struct {
    SDL_AudioDeviceID device;
    float phase_bass;     // oscillator phases (0..1 range)
    float phase_lead;
    Uint32 sample_pos;    // global sample counter for timing
    bool playing;
} Audio;

bool audio_init(Audio *audio);
void audio_cleanup(Audio *audio);
void audio_set_playing(Audio *audio, bool play);

#endif
