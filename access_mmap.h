#pragma once

#ifndef MEMORY_MAP
#define MEMORY_MAP

#include "initcpu.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
const char MAPPED_NAME[] = "/SpaceInvaders";
#else
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
    #define MAPPED_NAME "/SpaceInvaders"
#endif

struct memory_map {
#if defined(_WIN32) || defined(_WIN64)
    HANDLE h_map_file;
    LPVOID buffer_ptr;
#else
    int fd;
    void* buffer_ptr;
#endif
};

int init_mmap(State8080* state);

#endif
