#include <stdio.h>
#include <stdlib.h>
#include "rtcli/vm/class.h"
#include "rtcli/vm/object.h"
#include "rtcli/vm/type.h"
#include "rtcli/vm/method.h"
#include "rtcli/vm/gc.h"
#include "rtcli/detail/log.h"

typedef struct VMStack
{
    void* datas[100];
    int current;    
} VMStack;

void ldarg0(struct VMStack* stack, rtcli_object* this)
{
    stack->current += 1;
    stack->datas[stack->current - 1] = malloc(sizeof(rtcli_object*));
    *(rtcli_object**)stack->datas[stack->current - 1] = this;
}

void* get_at_stack(struct VMStack* stack, rtcli_u32 index)
{
    stack->current -= 1;
    return stack->datas[index];
}

void RuntimeType_GetType(struct VMStack* stack)
{
    rtcli_object* this = *(rtcli_object**)get_at_stack(stack, 0);
    ldarg0(stack, this);
}

void RuntimeType_Ctor(struct VMStack* stack)
{
    VMType* this = *(VMType**)get_at_stack(stack, 0);
    this->name = "Internal_RuntimeType";
    this->namespaze = "#";
    rtcli_info("RuntimeType ctor, rc %d", this->object.m_rc);
}

void invoke(VMMethodInfo* method, struct VMStack* stack)
{
    if(method->flags && METHOD_FLAG_NATIVE)
    {
        return method->method_pointer(stack);
    }
}

int main()
{
    // System.Type
    rtcli_vtable vtable = {};
    vtable.name = "Type";
    vtable.namespaze = "System";
    vtable.fields = NULL;
    vtable.field_count = 0;
    VMType* runtimeTypeInstance = NULL;

    // Type GetType()
    VMMethodInfo GetType = {
        .method_pointer = &RuntimeType_GetType,
        .name = "GetType",
        .klass = NULL,
        .return_type = runtimeTypeInstance,
        .parameters = NULL,
        .parameters_count = 0,
        .flags = METHOD_FLAG_NATIVE
    };
    VMMethodInfo Ctor = {
        .method_pointer = &RuntimeType_Ctor,
        .name = ".ctor",
        .klass = NULL,
        .return_type = NULL,
        .parameters = NULL,
        .parameters_count = 0,
        .flags = METHOD_FLAG_NATIVE
    };
    VMMethodInfo methods[] = {
        GetType, Ctor
    };
    vtable.methods = methods;
    vtable.method_count = sizeof(methods) / sizeof(VMMethodInfo);
    vtable.actual_size = -1;

    // System.RuntimeType
    runtimeTypeInstance = (VMType*)rtcli_gc_alloc_obj(&vtable, 50);
    runtimeTypeInstance->object.m_rc = 1;
    if(!runtimeTypeInstance)
    {
        rtcli_error("gc_alloc failed!");
    } else
    {
        rtcli_info("gc_alloc succeed!");
    }

    VMStack stack = {
        .current = 0
    };
    ldarg0(&stack, (rtcli_object*)runtimeTypeInstance);
    invoke(&Ctor, &stack);
    ldarg0(&stack, (rtcli_object*)runtimeTypeInstance);
    invoke(&GetType, &stack);
    VMType* result = *(VMType**)get_at_stack(&stack, 0);
    rtcli_info(result->name);

    return 0;
}