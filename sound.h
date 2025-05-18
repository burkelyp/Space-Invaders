#ifndef SOUND_H
#define SOUND_H

#include <cstdint>
#include "miniaudio.h"

enum SoundEffect {
    SOUND_SHOOT,
    SOUND_INVADER_KILLED,
    SOUND_FAST_INVADER_1,
    SOUND_FAST_INVADER_2,
    SOUND_FAST_INVADER_3,
    SOUND_FAST_INVADER_4,
    SOUND_UFO_HIGH,
    SOUND_UFO_LOW,
    SOUND_EXPLOSION,
    SOUND_EXTENDED_PLAY,
    SOUND_COUNT
};

extern ma_engine engine;
extern ma_sound sounds[SOUND_COUNT];

bool initSoundSystem();
void playSound(SoundEffect effect);
void stopSound(SoundEffect effect);
void shutdownSoundSystem();

#endif // SOUND_H
