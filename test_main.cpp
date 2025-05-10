#include <iostream>
#include <thread>
#include <chrono>
#include "sound_manager.h"

// Helper to wait after playing each sound
void wait() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

int main() {
    std::cout << "Starting Space Invaders audio test..." << std::endl;

    SoundManager sound;
    if (!sound.init()) {
        std::cerr << "Audio init failed." << std::endl;
        return 1;
    }

    // Play each sound sequentially
    std::cout << "Playing: shoot" << std::endl;
    sound.play_sound("shoot");
    wait();

    std::cout << "Playing: invader_killed" << std::endl;
    sound.play_sound("invader_killed");
    wait();

    std::cout << "Playing: player_die" << std::endl;
    sound.play_sound("player_die");
    wait();

    std::cout << "Playing: invader_step1" << std::endl;
    sound.play_sound("invader_step1");
    wait();

    std::cout << "Playing: invader_step2" << std::endl;
    sound.play_sound("invader_step2");
    wait();

    std::cout << "Playing: invader_step3" << std::endl;
    sound.play_sound("invader_step3");
    wait();

    std::cout << "Playing: invader_step4" << std::endl;
    sound.play_sound("invader_step4");
    wait();

    std::cout << "Playing: ufo_low" << std::endl;
    sound.play_sound("ufo_low");
    wait();

    std::cout << "Playing: ufo_high" << std::endl;
    sound.play_sound("ufo_high");
    wait();

    sound.shutdown();
    std::cout << "Audio test complete." << std::endl;
    return 0;
}
