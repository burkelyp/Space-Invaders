#include "initcpu.h"
#include "memory_map.h"

int init_mmap(State8080* state, memory_map* mmap) {
    //#ifdef Q_OS_WIN

    mmap->h_map_file = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MAPPED_NAME);

    if (mmap->h_map_file == NULL) {
        _tprintf(TEXT("Could not open file mapping object (%d).\n"),
            GetLastError());
        return 1;
    }
    mmap->buffer_ptr = (LPVOID)MapViewOfFile(mmap->h_map_file, FILE_MAP_ALL_ACCESS, 0, 0, MEMORY_SIZE);
    if (mmap->buffer_ptr == NULL) {
        _tprintf(TEXT("Could not map view of file (%d).\n"),
            GetLastError());

        CloseHandle(mmap->h_map_file);

        return 1;
    }

    // buffer_ptr for initcpu
    initCPU(state, mmap->buffer_ptr);
    //#endif
    return 0;
}