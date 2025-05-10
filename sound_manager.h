#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <string>
#include <unordered_map>
#include "miniaudio.h"

class SoundManager {
public:
    bool init();
    void play_sound(const std::string& name);
    void shutdown();

private:
    ma_engine engine;
    std::unordered_map<std::string, std::string> sound_map;
};

#endif // SOUND_MANAGER_H
