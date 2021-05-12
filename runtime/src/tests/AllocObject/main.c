#include <stdio.h>
#include <stdlib.h>
#include "rtcli/vm/class.h"
#include "rtcli/vm/object.h"
#include "rtcli/vm/method.h"
#include "rtcli/vm/gc.h"
#include "rtcli/vm/interpreter/interpreter.h"
#include "rtcli/detail/log.h"

typedef struct VMStackFrame
{
    void* datas[100];
    int current;    
} VMStackFrame;

void ldarg0(struct VMStackFrame* stack, rtcli_object* this)
{
    stack->current += 1;
    stack->datas[stack->current - 1] = malloc(sizeof(rtcli_object*));
    *(rtcli_object**)stack->datas[stack->current - 1] = this;
}

void* get_at_stack(struct VMStackFrame* stack, rtcli_u32 index)
{
    stack->current -= 1;
    return stack->datas[index];
}

void RuntimeType_GetType(struct VMStackFrame* stack)
{
    rtcli_object* this = *(rtcli_object**)get_at_stack(stack, 0);
    ldarg0(stack, this);
}

void RuntimeType_Ctor(struct VMStackFrame* stack)
{
    rtcli_object* this = *(rtcli_object**)get_at_stack(stack, 0);
    //this->name = "Internal_RuntimeType";
    //this->namespaze = "#";
    rtcli_info("RuntimeType ctor, rc %d", this->m_rc);
}

void call(VMMethodInfo* method, struct VMStackFrame* stack)
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
    struct rtcli_object* runtimeTypeInstance = NULL;

    // Type GetType()
    struct VMMethodInfo GetType = {
        .method_pointer = &RuntimeType_GetType,
        .name = "GetType",
        .klass = NULL,
        .return_type = NULL/*runtimeTypeInstance*/,
        .parameters = NULL,
        .parameters_count = 0,
        .flags = METHOD_FLAG_NATIVE
    };
    struct VMMethodInfo Ctor = {
        .method_pointer = &RuntimeType_Ctor,
        .name = ".ctor",
        .klass = NULL,
        .return_type = NULL,
        .parameters = NULL,
        .parameters_count = 0,
        .flags = METHOD_FLAG_NATIVE
    };
    struct VMMethodInfo methods[] = {
        GetType, Ctor
    };
    vtable.methods = methods;
    vtable.method_count = sizeof(methods) / sizeof(VMMethodInfo);
    vtable.actual_size = -1;

    // System.RuntimeType
    runtimeTypeInstance = (rtcli_object*)rtcli_gc_alloc_obj(&vtable, 50);
    runtimeTypeInstance->m_rc = 1;
    if(!runtimeTypeInstance)
    {
        rtcli_error("gc_alloc failed!");
    } else
    {
        rtcli_info("gc_alloc succeed!");
    }

    //   "IL_0000: nop"
    //   "IL_0001: newobj System.Void TestCase.TestInnerClass::.ctor()"
    //   "IL_0006: stloc.0"
    //   "IL_0007: ldloc.0"
    //   "IL_0008: callvirt System.Type System.Object::GetType()"
    //   "IL_000d: stloc.1"
    //   "IL_000e: ret"
    VMStackFrame stack = {
        .current = 0
    };
    ldarg0(&stack, (rtcli_object*)runtimeTypeInstance);
    call(&Ctor, &stack);
    ldarg0(&stack, (rtcli_object*)runtimeTypeInstance);
    call(&GetType, &stack);
    rtcli_object* result = *(rtcli_object**)get_at_stack(&stack, 0);
    rtcli_info("%d", result->m_rc);

    return 0;
}