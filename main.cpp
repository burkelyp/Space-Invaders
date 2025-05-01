

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <typeinfo>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <map>
#include "emulator.h"



int main(int argc, char** argv) {

    State8080* cpu = new State8080();
    memset(cpu, 0, sizeof(State8080));

    // Open file
    FILE* f = fopen(argv[1], "rb");
    if (f == NULL)
    {
        printf("error: Couldn't open %s\n", argv[1]);
        exit(1);
    };

    // Get the file size and read it into a memory buffer
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char* buffer = (unsigned char*)malloc(fsize);

    fread(buffer, fsize, 1, f);
    fclose(f);

    // Load file into cpu memory and free buffer
    memcpy(cpu->memory, buffer, fsize);
    free(buffer);


    // Option debug more
    bool debug = false;
    if (argc >= 3) {
        if (std::string(argv[2]) == "--debug") {
            debug = true;
        }
    };


    // Main loop
    int i = 0;
    while ((i < 200) && (!cpu->halted)) {
        Emulate8080Op(cpu, debug);
        i++;
    }
    return 0;
};
