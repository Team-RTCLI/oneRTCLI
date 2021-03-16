#include "cgpu/cgpu_config.h"
#if defined(CGPU_USE_VULKAN) || defined(DAWN_ENABLE_BACKEND_VULKAN)

#define VOLK_IMPLEMENTATION
#include "cgpu/backend/vulkan/cgpu_vulkan.h"

#endif