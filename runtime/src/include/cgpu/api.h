#pragma once
#include "cgpu_config.h"
#include "stdbool.h"
#define CGPU_ARRAY_LEN(array) ((sizeof(array) / sizeof(array[0])))

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
#include <stdint.h>

typedef int CGpuVersion;
typedef enum ECGPUBackEnd
{
#ifdef CGPU_USE_WEBGPU
    ECGPUBackEnd_WEBGPU = 0,
#endif
#ifdef CGPU_USE_VULKAN
    ECGPUBackEnd_VULKAN = 1,
#endif
#ifdef CGPU_USE_D3D12
    ECGPUBackEnd_D3D12 = 2,
#ifdef XBOX
    ECGPUBackEnd_XBOX_D3D12 = 3,
#endif
#endif
#ifdef CGPU_USE_AGC
    ECGPUBackEnd_AGC = 4,
#endif
#ifdef CGPU_USE_METAL
    ECGPUBackEnd_METAL = 5,
#endif
    ECGPUBackEnd_COUNT
} ECGPUBackEnd;

typedef struct CGpuInstanceDescriptor {
    ECGPUBackEnd backend;
    bool         enableDebugLayer;
    bool         enableGpuBasedValidation;
} CGpuInstanceDescriptor;

typedef struct CGpuAdapterDetail {
    uint32_t deviceId;
    uint32_t vendorId;
    const char* name;
} CGpuAdapterDetail;

typedef uint32_t CGpuQueueIndex;
typedef enum ECGpuQueueType{
    ECGpuQueueType_Graphics = 0,
    ECGpuQueueType_Compute = 1,
    ECGpuQueueType_Transfer = 2,
    ECGpuQueueType_Count
} ECGpuQueueType;

typedef struct CGpuQueueGroupDescriptor {
    ECGpuQueueType queueType;
    uint32_t queueCount;
} CGpuQueueGroupDescriptor;

typedef struct CGpuDeviceDescriptor {
    CGpuQueueGroupDescriptor* queueGroups;
    uint32_t                  queueGroupCount;
} CGpuDeviceDescriptor;

typedef struct CGpuCommandEncoderDescriptor {
    uint32_t ___nothing_and_useless__;
} CGpuCommandEncoderDescriptor;

typedef struct CGpuSurface {
    void* ptr;
} CGpuSurface;
typedef CGpuSurface* CGpuSurfaceId;

typedef struct CGpuSwapChainDescriptor {
    CGpuSurface surface;
    /// Number of backbuffers in this swapchain
	uint32_t imageCount;
	/// Width of the swapchain
	uint32_t width;
	/// Height of the swapchain
	uint32_t height;
    /// Set whether swap chain will be presented using vsync
	bool enableVsync;
	/// We can toggle to using FLIP model if app desires.
	bool useFlipSwapEffect;
    /// Clear Value.
    float clearValue[4];
} CGpuSwapChainDescriptor;


typedef struct CGpuInstance* CGpuInstanceId;
typedef struct CGpuAdapter {
    const struct CGpuInstance* instance;
} CGpuAdapter;
typedef CGpuAdapter* CGpuAdapterId;

typedef struct CGpuDevice {
    const CGpuAdapterId adapter;
#ifdef __cplusplus
    CGpuDevice() :adapter(CGPU_NULLPTR) {}
#endif
} CGpuDevice;
typedef CGpuDevice* CGpuDeviceId;

typedef struct CGpuQueue {
    CGpuDeviceId   device;    
    ECGpuQueueType type; 
    CGpuQueueIndex index;
} CGpuQueue;
typedef CGpuQueue* CGpuQueueId;

typedef struct CGpuCommandEncoder {
    CGpuQueueId queue;
} CGpuCommandEncoder;
typedef CGpuCommandEncoder* CGpuCommandEncoderId;

typedef struct CGpuCommandBuffer {
    CGpuCommandEncoderId pool;
} CGpuCommandBuffer;
typedef CGpuCommandBuffer* CGpuCommandBufferId;

// Device APIs
CGPU_API CGpuInstanceId cgpu_create_instance(const CGpuInstanceDescriptor* desc);
typedef CGpuInstanceId (*CGPUProcCreateInstance)(const CGpuInstanceDescriptor * descriptor);
CGPU_API void cgpu_free_instance(CGpuInstanceId instance);
typedef void (*CGPUProcFreeInstance)(CGpuInstanceId instance);

CGPU_API void cgpu_enum_adapters(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);
typedef void (*CGPUProcEnumAdapters)(CGpuInstanceId instance, CGpuAdapterId* const adapters, size_t* adapters_num);

CGPU_API CGpuAdapterDetail cgpu_query_adapter_detail(const CGpuAdapterId adapter);
typedef CGpuAdapterDetail (*CGPUProcQueryAdapterDetail)(const CGpuAdapterId instance);
CGPU_API uint32_t cgpu_query_queue_count(const CGpuAdapterId adapter, const ECGpuQueueType type);
typedef uint32_t (*CGPUProcQueryQueueCount)(const CGpuAdapterId adapter, const ECGpuQueueType type);

CGPU_API CGpuDeviceId cgpu_create_device(CGpuAdapterId adapter, const CGpuDeviceDescriptor* desc);
typedef CGpuDeviceId (*CGPUProcCreateDevice)(CGpuAdapterId adapter, const CGpuDeviceDescriptor* desc);
CGPU_API void cgpu_free_device(CGpuDeviceId device);
typedef void (*CGPUProcFreeDevice)(CGpuDeviceId device);

CGPU_API CGpuQueueId cgpu_get_queue(CGpuDeviceId device, ECGpuQueueType type, uint32_t index);
typedef CGpuQueueId (*CGPUProcGetQueue)(CGpuDeviceId device, ECGpuQueueType type, uint32_t index);
CGPU_API void cgpu_free_queue(CGpuQueueId queue);
typedef void (*CGPUProcFreeQueue)(CGpuQueueId queue);

CGPU_API CGpuCommandEncoderId cgpu_create_command_encoder(CGpuQueueId queue, const CGpuCommandEncoderDescriptor* desc);
typedef CGpuCommandEncoderId (*CGPUProcCreateCommandEncoder)(CGpuQueueId queue, const CGpuCommandEncoderDescriptor* desc);
CGPU_API void cgpu_free_command_encoder(CGpuCommandEncoderId encoder);
typedef void (*CGPUProcFreeCommandEncoder)(CGpuCommandEncoderId encoder);


// CMDs 
CGPU_API void cgpu_cmd_set_viewport(CGpuCommandBufferId cmd, float x, float y, float width, float height,
    float min_depth, float max_depth);
typedef void (*CGPUProcCmdSetViewport)(CGpuCommandBufferId cmd, float x, float y, float width, float height,
    float min_depth, float max_depth);

CGPU_API void cgpu_cmd_set_scissor(CGpuCommandBufferId cmd, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
typedef void (*CGPUProcCmdSetScissor)(CGpuCommandBufferId cmd, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

typedef struct CGpuBuffer {const CGpuDeviceId device;} CGpuBuffer;
typedef CGpuBuffer* CGpuBufferId;


typedef struct CGpuProcTable {
    CGPUProcCreateInstance     create_instance;
    CGPUProcFreeInstance       free_instance;

    CGPUProcEnumAdapters       enum_adapters;
    CGPUProcQueryAdapterDetail query_adapter_detail;
    CGPUProcQueryQueueCount    query_queue_count;

    CGPUProcCreateDevice       create_device;
    CGPUProcFreeDevice         free_device;
    
    CGPUProcGetQueue           get_queue;
    CGPUProcFreeQueue          free_queue;

    CGPUProcCreateCommandEncoder  create_command_encoder;
    CGPUProcFreeCommandEncoder    free_command_encoder;

    CGPUProcCmdSetViewport     cmd_set_viewport;
    CGPUProcCmdSetScissor      cmd_set_scissor;
} CGpuProcTable;

// surfaces
CGPU_API void cgpu_surface_free(CGpuInstanceId instance, CGpuSurfaceId surface);
typedef void (*CGPUSurfaceProc_Free)(CGpuInstanceId instance, CGpuSurfaceId surface);

#if defined(_WIN32) || defined(_WIN64)
typedef struct HWND__* HWND;
CGPU_API CGpuSurfaceId cgpu_surface_from_hwnd(CGpuInstanceId instance, HWND window);
typedef CGpuSurfaceId (*CGPUSurfaceProc_CreateFromHWND)(CGpuInstanceId instance, HWND window);
#endif
#ifdef __APPLE__
CGPU_API CGpuSurfaceId cgpu_surface_from_ui_view(CGpuInstanceId instance, UIView* window);
typedef CGpuSurfaceId (*CGPUSurfaceProc_CreateFromUIView)(CGpuInstanceId instance, UIView* window);

CGPU_API CGpuSurfaceId cgpu_surface_from_ns_view(CGpuInstanceId instance, NSView* window);
typedef CGpuSurfaceId (*CGPUSurfaceProc_CreateFromNSView)(CGpuInstanceId instance, NSView* window);
#endif
typedef struct CGpuSurfacesProcTable {
#if defined(_WIN32) || defined(_WIN64)
    CGPUSurfaceProc_CreateFromHWND from_hwnd;
#endif
#ifdef __APPLE__
    CGPUSurfaceProc_CreateFromUIView from_ui_view;
    CGPUSurfaceProc_CreateFromNSView from_ns_view;
#endif
    CGPUSurfaceProc_Free cgpu_surface_free;
} CGpuSurfacesProcTable;

typedef struct CGpuInstance {
    const CGpuProcTable* proc_table;
    const CGpuSurfacesProcTable* surfaces_table;
} CGpuInstance;

#ifdef __cplusplus
} // end extern "C"
#endif
