#include "sound.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <iostream>

static ma_engine engine;
static ma_sound sounds[SOUND_COUNT];

/**
 * Maps a sound effect to its file path in the sounds directory
 * 
 * @param effect - the sound enum
 * @return const char* - file path string
 */
const char* getFilePath(SoundEffect effect) {
    switch (effect) {
        case SOUND_SHOOT:           return "sounds/shoot.wav";
        case SOUND_INVADER_KILLED:  return "sounds/invaderkilled.wav";
        case SOUND_FAST_INVADER_1:  return "sounds/fastinvader1.wav";
        case SOUND_FAST_INVADER_2:  return "sounds/fastinvader2.wav";
        case SOUND_FAST_INVADER_3:  return "sounds/fastinvader3.wav";
        case SOUND_FAST_INVADER_4:  return "sounds/fastinvader4.wav";
        case SOUND_UFO_HIGH:        return "sounds/ufo_highpitch.wav";
        case SOUND_UFO_LOW:         return "sounds/ufo_lowpitch.wav";
        case SOUND_EXPLOSION:       return "sounds/explosion.wav";
        case SOUND_EXTENDED_PLAY:   return "sounds/extendedplay.wav";
        default:                    return nullptr;
    }
}

/**
 * Initializes the MiniAudio engine and loads all sound effects
 * 
 * @return true if successful, false if initialization or loading fails
 */
bool initSoundSystem() {
    if (ma_engine_init(nullptr, &engine) != MA_SUCCESS) {
        std::cerr << "Failed to initialize MiniAudio engine.\n";
        return false;
    }

    for (int i = 0; i < SOUND_COUNT; ++i) {
        const char* path = getFilePath(static_cast<SoundEffect>(i));
        if (ma_sound_init_from_file(&engine, path, 0, nullptr, nullptr, &sounds[i]) != MA_SUCCESS) {
            std::cerr << "Failed to load sound: " << path << "\n";
            return false;
        }
    }

    return true;
}

/**
 * Plays a sound effect using the MiniAudio engine
 * 
 * @param effect - the sound effect to play
 */
void playSound(SoundEffect effect) {
    if (effect < 0 || effect >= SOUND_COUNT) return;
    ma_sound_stop(&sounds[effect]); // Stop any existing instance
    ma_sound_seek_to_pcm_frame(&sounds[effect], 0); // Rewind
    ma_sound_start(&sounds[effect]);
}

/**
 * Shuts down the MiniAudio engine and releases resources
 */
void shutdownSoundSystem() {
    for (int i = 0; i < SOUND_COUNT; ++i) {
        ma_sound_uninit(&sounds[i]);
    }
    ma_engine_uninit(&engine);
}
