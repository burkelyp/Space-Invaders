#define MINIAUDIO_IMPLEMENTATION
#include "sound_manager.h"
#include <iostream>

bool SoundManager::init() {
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        std::cerr << "Failed to initialize miniaudio engine.\n";
        return false;
    }

    sound_map["shoot"] = "sounds/shoot.wav";
    sound_map["invader_killed"] = "sounds/invaderkilled.wav";
    sound_map["player_die"] = "sounds/explosion.wav";
    sound_map["invader_step1"] = "sounds/fastinvader1.wav";
    sound_map["invader_step2"] = "sounds/fastinvader2.wav";
    sound_map["invader_step3"] = "sounds/fastinvader3.wav";
    sound_map["invader_step4"] = "sounds/fastinvader4.wav";
    sound_map["ufo_low"] = "sounds/ufo_lowpitch.wav";
    sound_map["ufo_high"] = "sounds/ufo_highpitch.wav";

    return true;
}

void SoundManager::play_sound(const std::string& name) {
    auto it = sound_map.find(name);
    if (it != sound_map.end()) {
        if (ma_engine_play_sound(&engine, it->second.c_str(), NULL) != MA_SUCCESS) {
            std::cerr << "Failed to play sound: " << name << "\n";
        }
    } else {
        std::cerr << "Sound not found in map: " << name << "\n";
    }
}

void SoundManager::shutdown() {
    ma_engine_uninit(&engine);
}
