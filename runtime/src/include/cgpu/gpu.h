#pragma once

#ifdef __cplusplus
extern "C" {
#ifndef CGPU_NULLPTR
#define CGPU_NULLPTR nullptr
#endif
#else
#ifndef CGPU_NULLPTR
#define CGPU_NULLPTR NULL
#endif
#endif

typedef struct GpuAdapter {const char* label = CGPU_NULLPTR;} GpuAdapter;
typedef GpuAdapter* GpuAdapterId;
typedef struct GpuDevice {const char* label = CGPU_NULLPTR;} GpuDevice;
typedef GpuDevice* GpuDeviceId;
typedef struct GpuBuffer {const char* label = CGPU_NULLPTR;} GpuBuffer;
typedef GpuBuffer* GpuBufferId;



#ifdef __cplusplus
} // end extern "C"
#endif
