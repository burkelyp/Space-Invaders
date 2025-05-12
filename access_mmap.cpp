#include "access_mmap.h"

int init_mmap(State8080* state) {
    memory_map mem_map;

#if defined(_WIN32) || defined(_WIN64)
    mem_map.h_map_file = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAPPED_NAME);

    if (mem_map.h_map_file == NULL) {
        return 1;
    }

    mem_map.buffer_ptr = MapViewOfFile(mem_map.h_map_file, FILE_MAP_ALL_ACCESS, 0, 0, MEMORY_SIZE);
    if (mem_map.buffer_ptr == NULL) {
        CloseHandle(mem_map.h_map_file);
        return 1;
    }

#else
    mem_map.fd = shm_open(MAPPED_NAME, O_RDWR, 0666);
    if (mem_map.fd == -1) {
        return 1;
    }

    mem_map.buffer_ptr = mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_map.fd, 0);
    if (mem_map.buffer_ptr == MAP_FAILED) {
        close(mem_map.fd);
        return 1;
    }
#endif

    // buffer_ptr for initcpu
    initCPU(state, mem_map.buffer_ptr);

    return 0;
}
