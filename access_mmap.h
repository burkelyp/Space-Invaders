#ifndef MEMORY_MAP
#define MEMORY_MAP

#include "initcpu.h"

#ifdef PLATFORM_WINDOWS
    #include <Windows.h>
    #define MAPPED_NAME TEXT("/SpaceInvaders")
    struct memory_map {
        HANDLE h_map_file;
        LPVOID buffer_ptr;
    };
#else
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <cstring>

    #define MAPPED_NAME "/SpaceInvaders"

    struct memory_map {
        int shm_fd;
        uint8_t* buffer_ptr;
    };
#endif

// struct memory_map {

// 	HANDLE h_map_file;
// 	LPVOID buffer_ptr;
// };

int init_mmap(State8080* state, memory_map* mmap);

#endif
