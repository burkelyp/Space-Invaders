#include "initcpu.h"
#include "access_mmap.h"

#ifdef PLATFORM_WINDOWS
    #include <Windows.h>
    #include <tchar.h>

    int init_mmap(State8080* state, memory_map* mem_map) {
        mem_map->h_map_file = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAPPED_NAME);

        if (mem_map->h_map_file == NULL) {
            _tprintf(TEXT("Could not open file mapping object (%d).\n"), GetLastError());
            return 1;
        }

        mem_map->buffer_ptr = (LPVOID)MapViewOfFile(mem_map->h_map_file, FILE_MAP_ALL_ACCESS, 0, 0, MEMORY_SIZE);
        if (mem_map->buffer_ptr == NULL) {
            _tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
            CloseHandle(mem_map->h_map_file);
            return 1;
        }

        initCPU(state, mem_map->buffer_ptr);
        return 0;
    }

#else // Linux / POSIX

    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <cstring>
    #include <iostream>

    int init_mmap(State8080* state, memory_map* mem_map) {
        mem_map->shm_fd = shm_open(MAPPED_NAME, O_RDWR, 0666);
        if (mem_map->shm_fd == -1) {
            std::cerr << "Failed to open shared memory object.\n";
            return 1;
        }

        mem_map->buffer_ptr = (uint8_t*)mmap(nullptr, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_map->shm_fd, 0);
        if (mem_map->buffer_ptr == MAP_FAILED) {
            std::cerr << "Failed to map shared memory.\n";
            close(mem_map->shm_fd);
            return 1;
        }

        initCPU(state, mem_map->buffer_ptr);
        return 0;
    }
#endif