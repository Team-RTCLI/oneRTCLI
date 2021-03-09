#include "gtest/gtest.h"
#include "cgpu/api.h"

CGpuInstanceId init_instance(ECGPUBackEnd backend, bool enableDebugLayer, bool enableGPUValidation)
{
    CGpuInstanceDescriptor desc;
    desc.backend = backend;
    desc.enableDebugLayer = enableDebugLayer;
    desc.enableGpuBasedValidation = enableGPUValidation;
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
    }
    //cgpu_destroy_instance(instance);
    return adapters_count;
}

class CGpuTest : public::testing::TestWithParam<ECGPUBackEnd>
{
protected:
	void SetUp() override
	{

    }
	CGpuInstanceId instance;
};

TEST_P(CGpuTest, InstanceCreation)
{
    ECGPUBackEnd backend =  GetParam();
    auto inst = init_instance(backend, true, true);
    auto inst2 = init_instance(backend, true, false);
    auto inst3 = init_instance(backend, false, false);
    EXPECT_NE(inst, CGPU_NULLPTR);
    EXPECT_NE(inst2, CGPU_NULLPTR);
    EXPECT_NE(inst3, CGPU_NULLPTR);
    cgpu_destroy_instance(inst);
    cgpu_destroy_instance(inst2);
    cgpu_destroy_instance(inst3);
}

TEST_P(CGpuTest, AdapterEnum)
{
    ECGPUBackEnd backend = GetParam();
    instance = init_instance(backend, true, true);
    EXPECT_GT(enum_adapters(instance), 0);
}

static const auto allPlatforms = testing::Values(
#ifdef CGPU_USE_WEBGPU
    ECGPUBackEnd_WEBGPU
#endif
#ifdef CGPU_USE_VULKAN
    , ECGPUBackEnd_VULKAN
#endif
#ifdef CGPU_USE_D3D12
    ,ECGPUBackEnd_D3D12
#endif
);
INSTANTIATE_TEST_SUITE_P(DeviceInitialization, CGpuTest, allPlatforms);
