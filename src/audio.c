/*
 * audio.c - generates the background music in real time
 *
 * SDL calls our callback whenever it needs more audio samples.
 * we figure out what beat we're on, look up the note, and
 * generate the waveform with basic trig.
 *
 * bass = square wave (that chunky 8bit sound)
 * lead = sine wave (smoother, less harsh)
 *
 * the patterns just loop forever. its not gonna win any grammys
 * but it works and sounds kinda retro
 */

#include "audio.h"
#include <math.h>

// note frequencies in hz (equal temperament, A4=440)
#define NOTE_C3  130.81f
#define NOTE_D3  146.83f
#define NOTE_E3  164.81f
#define NOTE_F3  174.61f
#define NOTE_G3  196.00f
#define NOTE_A3  220.00f

#define NOTE_C4  261.63f
#define NOTE_D4  293.66f
#define NOTE_E4  329.63f
#define NOTE_F4  349.23f
#define NOTE_G4  392.00f
#define NOTE_A4  440.00f
#define NOTE_B4  493.88f

#define NOTE_C5  523.25f
#define NOTE_D5  587.33f
#define NOTE_E5  659.25f

#define NOTE_REST 0.0f

// 8 beat bass loop. tried a few progressions, this one felt the most "driving"
static const float BASS_PATTERN[] = {
    NOTE_C3, NOTE_C3, NOTE_F3, NOTE_F3,
    NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3
};
#define BASS_LEN 8

// 16 beat melody that sits on top
static const float LEAD_PATTERN[] = {
    NOTE_C5,   NOTE_E5,   NOTE_G4,   NOTE_REST,
    NOTE_A4,   NOTE_G4,   NOTE_E4,   NOTE_REST,
    NOTE_F4,   NOTE_A4,   NOTE_C5,   NOTE_REST,
    NOTE_G4,   NOTE_B4,   NOTE_D5,   NOTE_C5
};
#define LEAD_LEN 16

static float square_wave(float phase)
{
    return phase < 0.5f ? 1.0f : -1.0f;
}

static float sine_wave(float phase)
{
    return sinf(phase * 2.0f * (float)M_PI);
}

// quick attack + gentle decay so notes have a bit of punch to em
static float envelope(float t)
{
    if (t < 0.05f)
        return t / 0.05f;           // ramp up fast
    return 1.0f - (t * 0.6f);       // fade out slowly
}

/*
 * this runs on a separate thread btw. SDL handles the threading
 * but we only touch our own fields here (phase, sample_pos)
 * and the main thread only ever pauses/unpauses us, so its fine
 */
static void audio_callback(void *userdata, Uint8 *stream, int len)
{
    Audio *audio = (Audio *)userdata;
    Sint16 *buf = (Sint16 *)stream;
    int num_samples = len / (int)sizeof(Sint16);

    for (int i = 0; i < num_samples; i++) {
        Uint32 pos = audio->sample_pos;
        int beat = (int)(pos / BEAT_SAMPLES);
        float t = (float)(pos % BEAT_SAMPLES) / (float)BEAT_SAMPLES;

        float bass_freq = BASS_PATTERN[beat % BASS_LEN];
        float lead_freq = LEAD_PATTERN[beat % LEAD_LEN];
        float env = envelope(t);

        // bass voice
        float bass = 0.0f;
        if (bass_freq > 0.0f) {
            bass = square_wave(audio->phase_bass) * env * 0.4f;
            audio->phase_bass += bass_freq / AUDIO_SAMPLE_RATE;
            if (audio->phase_bass >= 1.0f) audio->phase_bass -= 1.0f;
        }

        // lead voice
        float lead = 0.0f;
        if (lead_freq > 0.0f) {
            lead = sine_wave(audio->phase_lead) * env * 0.5f;
            audio->phase_lead += lead_freq / AUDIO_SAMPLE_RATE;
            if (audio->phase_lead >= 1.0f) audio->phase_lead -= 1.0f;
        }

        // mix and clamp
        float mixed = (bass + lead) * AUDIO_VOLUME;
        if (mixed >  1.0f) mixed =  1.0f;
        if (mixed < -1.0f) mixed = -1.0f;

        buf[i] = (Sint16)(mixed * 32767.0f);
        audio->sample_pos++;
    }
}

bool audio_init(Audio *audio)
{
    audio->phase_bass = 0.0f;
    audio->phase_lead = 0.0f;
    audio->sample_pos = 0;
    audio->playing = false;
    audio->device = 0;

    SDL_AudioSpec want, have;
    SDL_memset(&want, 0, sizeof(want));
    want.freq     = AUDIO_SAMPLE_RATE;
    want.format   = AUDIO_S16SYS;
    want.channels = AUDIO_CHANNELS;
    want.samples  = AUDIO_SAMPLES;
    want.callback = audio_callback;
    want.userdata = audio;

    audio->device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (audio->device == 0) {
        printf("WARNING: no audio device: %s\n", SDL_GetError());
        printf("game will run fine without music tho\n");
        return false;
    }

    SDL_PauseAudioDevice(audio->device, 0);   // 0 = unpause = play
    audio->playing = true;

    printf("audio: %dHz %dbit %dch\n",
           have.freq, SDL_AUDIO_BITSIZE(have.format), have.channels);
    return true;
}

void audio_set_playing(Audio *audio, bool play)
{
    if (audio->device == 0) return;
    SDL_PauseAudioDevice(audio->device, play ? 0 : 1);
    audio->playing = play;
}

void audio_cleanup(Audio *audio)
{
    if (audio->device != 0) {
        SDL_CloseAudioDevice(audio->device);
        audio->device = 0;
    }
    audio->playing = false;
}
