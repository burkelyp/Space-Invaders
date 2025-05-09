#pragma once

#ifndef MEMORY_MAP
#define MEMORY_MAP

#include "initcpu.h"

<<<<<<< HEAD
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define MAPPED_NAME TEXT("SpaceInvaders")
#else
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
    #define MAPPED_NAME "/SpaceInvaders"
=======
#ifdef _WIN64

    #include <Windows.h>
>>>>>>> 084aae831329db070c19a00d98197ffff7e19a45
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

<<<<<<< HEAD
#endif
=======

#endif
>>>>>>> 084aae831329db070c19a00d98197ffff7e19a45
