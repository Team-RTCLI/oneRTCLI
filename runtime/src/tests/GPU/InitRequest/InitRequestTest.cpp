#include "gtest/gtest.h"
#include "cgpu/api.h"
#include <iostream>
#include <vector>
#ifdef CGPU_USE_VULKAN
#include "vulkan/vulkan_core.h"
#include "cgpu/backend/vulkan/cgpu_vulkan_exts.h"
#endif

CGpuInstanceId init_instance(ECGPUBackEnd backend)
{
    CGpuInstanceDescriptor desc;
    desc.backend = backend;
    CGpuInstanceId instance = cgpu_create_instance(&desc);
    return instance;
}

int enum_adapters(CGpuInstanceId instance)
{
    // enum adapters
    size_t adapters_count = 0;
    cgpu_enum_adapters(instance, nullptr, &adapters_count);
    std::vector<CGpuAdapterId> adapters; adapters.resize(adapters_count);
    cgpu_enum_adapters(instance, adapters.data(), &adapters_count);
    for(auto adapter : adapters)
    {
        auto prop = cgpu_query_adapter_detail(adapter);
        std::cout << "device id: " << prop.deviceId << "  vendor id: " << prop.vendorId << "\n";
        std::cout << "    name: " << prop.name << "\n";
        std::cout << "    description: " << prop.driverDescription << "\n";
    }
    //cgpu_destroy_instance(instance);
    return adapters_count;
}

class CGpuTest : public ::testing::Test
{
protected:
	void SetUp() override
	{}
	CGpuInstanceId webgpu_instance;
	CGpuInstanceId vulkan_instance;
};

TEST(CGpuTest, InstanceInit) 
{
    auto webgpu_instance = init_instance(ECGPUBackEnd_WEBGPU);
    auto vulkan_instance = init_instance(ECGPUBackEnd_VULKAN);

    EXPECT_TRUE( webgpu_instance != nullptr );
    EXPECT_TRUE( vulkan_instance != nullptr );
}

TEST(CGpuTest, EnumAdapters) 
{
    auto webgpu_instance = init_instance(ECGPUBackEnd_WEBGPU);
    auto vulkan_instance = init_instance(ECGPUBackEnd_VULKAN);

    EXPECT_TRUE( webgpu_instance != nullptr );
    EXPECT_TRUE( vulkan_instance != nullptr );

    auto webgpu_adapters = enum_adapters(webgpu_instance);
    auto vulkan_adapters = enum_adapters(vulkan_instance);
    EXPECT_TRUE( webgpu_adapters > 0 );
    EXPECT_TRUE( vulkan_adapters > 0 );
}
 
#ifdef CGPU_USE_VULKAN
TEST(CGpuTest, CreateVkInstance)
{
    CGpuVulkanInstanceDescriptor vkDesc = {};
    const char* exts[] = 
    {
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		VK_KHR_SURFACE_EXTENSION_NAME
    };
    vkDesc.mInstanceExtensionCount = 2;
    vkDesc.ppInstanceExtensions = exts;
    vkDesc.mRequestAllAvailableQueues = true;
    
    //auto vulkan_instance = cgpu_vulkan_create_instance(&vkDesc);
    //EXPECT_TRUE( vulkan_instance != nullptr );
}
#endif