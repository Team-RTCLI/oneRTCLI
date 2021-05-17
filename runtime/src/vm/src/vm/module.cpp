#define DLL_IMPLEMENTATION
#include "rtcli/metadata/module.h"
#include "rtcli/metadata/class.h"
#include <unordered_map>
#include <string_view>

struct VMClassHashTable : 
    public std::unordered_map<std::string_view, VMRuntimeTypeHandle>
{

};

VMModule::~VMModule()
{
    delete class_table;
}

VMModule::VMModule()
{
    class_table = new VMClassHashTable();
}

VMRuntimeTypeHandle VMModule::GetClass(const char* key)
{
    const auto res = class_table->find(key);
    if (res != class_table->end())
    {
        return res->second; 
    }
    return nullptr;
}

rtcli_bool VMModule::InsertClass(const VMRuntimeTypeHandle klass)
{
    const auto res = class_table->find(klass->fullname);
    if (res != class_table->end())
    {
        return false; 
    }
    else
    {
        const std::string_view name_view = klass->fullname;
        (*class_table)[name_view] = klass;
        return true;
    }
}

extern "C"
{
    void VMModule_Init(struct VMModule* vm_module)
    {
        new (vm_module) VMModule();
    }

    VMRuntimeTypeHandle VMModule_GetClass(
        struct VMModule* vm_module, const char* key)
    {
        return vm_module->GetClass(key);
    }

    rtcli_bool VMModule_InsertClass(
        struct VMModule* vm_module, const VMRuntimeTypeHandle klass)
    {
        return vm_module->InsertClass(klass);
    }

    void VMModule_Destroy(struct VMModule* vm_module)
    {
        return vm_module->~VMModule();
    }
}