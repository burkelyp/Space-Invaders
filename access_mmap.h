#ifndef MEMORY_MAP
#define MEMORY_MAP
#define MAPPED_NAME TEXT("/SpaceInvaders")

#include "initcpu.h"

#ifdef PLATFORM_WINDOWS
    #include <Windows.h>
#endif

struct memory_map {

	HANDLE h_map_file;
	LPVOID buffer_ptr;
};

int init_mmap(State8080* state, memory_map* mmap);

#endif
