#ifndef SOUND_H
#define SOUND_H

#include <cstdint>

/**
 * List of available sound effects
 */
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

/**
 * Initializes the MiniAudio sound system and loads all sounds
 * 
 * @return true if successful, false on error
 */
bool initSoundSystem();

/**
 * Plays a specific sound effect
 * 
 * @param effect - the sound to play
 */
void playSound(SoundEffect effect);

/**
 * Shuts down the sound system and frees memory
 */
void shutdownSoundSystem();

#endif // SOUND_H
