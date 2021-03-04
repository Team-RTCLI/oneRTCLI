#pragma once
#include <stdbool.h>
#if defined(_LINUX) || defined(_UNIX)
#include <dlfcn.h>
using NativeLibHandle = void*;
#elif defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
typedef HMODULE NativeLibHandle;
#endif

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct SharedLibrary {NativeLibHandle handle; void* last_error;} SharedLibrary;
    typedef SharedLibrary* SharedLibraryId;

    inline SharedLibraryId shared_library_load(const char* path);
    inline bool shared_library_unload(SharedLibraryId library);
    inline bool shared_library_has_symbol(const SharedLibraryId library, const char* symbol_name);
    inline void* shared_library_load_symbol(const SharedLibraryId library, const char* symbol_name);

#if defined(_LINUX) || defined(_UNIX)

#elif defined(_WIN32) 
    inline const char* __sl_getWindowsError()
    {
        DWORD lastError = GetLastError();
        TCHAR* buffer = (TCHAR*)(malloc(sizeof(TCHAR) * 256));
        if(lastError != 0)
        {
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                          NULL,
                          lastError,
                          MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                          buffer,
                          256-1,
                          NULL);

            return buffer;
        }
        return NULL;
    }
    inline SharedLibraryId shared_library_load(const char* path)
    {
        SharedLibraryId loaded = (SharedLibraryId)malloc(sizeof(SharedLibrary));
        loaded->handle = NULL;
        loaded->last_error = NULL;

        loaded->handle = LoadLibraryA(path);
        if(loaded->handle == NULL)
        {
            if(loaded->last_error != NULL)
            {
                free((void*)(loaded->last_error));
            }
            loaded->last_error = (void*)__sl_getWindowsError();
        }
        return loaded;
    }
#endif

#ifdef __cplusplus
}
#endif