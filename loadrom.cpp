#include "loadrom.h"
#pragma warning(disable:4996)

/**
* Loads the ROM into the emulated Intel8080 CPU.
*
* @param path C-style string representing the path to the ROM file.
* @param state Pointer to a State8080 struct holding the CPU state and memory.
*/
void loadROM(const char* path, State8080* state) {
    FILE* ptr = std::fopen(path, "rb");
    if (ptr == nullptr) {
        std::cerr << "File pointer is null." << std::endl;
        exit(1);
    }

    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        std::cerr << "Failed to get size of ROM." << std::endl;
        exit(1);
    }

    size_t result = fread(state->memory, 1, statbuf.st_size, ptr);
    if (result != statbuf.st_size) {
        std::cerr << "Failed to read file. Reading error or EOF." << path << std::endl;
        exit(1);
    }
    fclose(ptr);
}