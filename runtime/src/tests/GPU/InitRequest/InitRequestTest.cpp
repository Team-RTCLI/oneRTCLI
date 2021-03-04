#include "cgpu/api.h"
#include <iostream>

int main()
{
    const CGpuInstanceDescriptor desc = 
    {
        .backend = ECGPUBackEnd_WEBGPU
    };
    CGpuInstanceId instance = cgpu_create_instance(&desc);

    cgpu_destroy_instance(instance);

    return 0;
}