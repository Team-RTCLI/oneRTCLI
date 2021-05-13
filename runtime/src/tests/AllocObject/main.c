#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "rtcli/base-types.h"
#include "rtcli/cil/opcode.h"
#include "rtcli/metadata/class.h"
#include "rtcli/vm/object.h"
#include "rtcli/metadata/method.h"
#include "rtcli/vm/gc.h"
#include "rtcli/vm/interpreter/interpreter.h"
#include "rtcli/vm/interpreter/stack.h"
#include "rtcli/detail/log.h"

void RuntimeType_GetType(struct VMStackFrame* stack)
{
    //rtcli_object* this = *(rtcli_object**)get_at_stack(stack, 0);
    //ldarg0(stack, this);
}

void RuntimeType_Ctor(struct VMStackFrame* stack)
{
    //rtcli_object* this = *(rtcli_object**)get_at_stack(stack, 0);
    //this->name = "Internal_RuntimeType";
    //this->namespaze = "#";
    //rtcli_info("RuntimeType ctor, rc %d", this->m_rc);
}

rtcli_byte opstack[4096 * 4];

int main()
{
    // System.Type
    rtcli_vtable vtable = {};
    vtable.name = "Type";
    vtable.namespaze = "System";
    vtable.fields = NULL;
    vtable.field_count = 0;
    vtable.actual_size = -1;

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


    // System.RuntimeType
    struct rtcli_object* runtimeTypeInstance = (rtcli_object*)rtcli_gc_alloc_obj(&vtable, 50);
    runtimeTypeInstance->m_rc = 1;
    if(!runtimeTypeInstance)
    {
        rtcli_error("gc_alloc failed!");
    } else
    {
        rtcli_info("gc_alloc succeed!");
    }

    /*
    static void Main()
    {
        int a = 5;
        int b = 512;
        a = a + b;
        System.Console.WriteLine(a);
    }
        =>
    .method private hidebysig static 
		void Main () cil managed 
	{
		// Method begins at RVA 0x2050
		// Code size 25 (0x19)
		.maxstack 2
		.entrypoint
		.locals init (
			[0] int32 a,
			[1] int32 b
		)
		// {
		IL_0000: nop                 ML_0000: nop
		// int num = 5;              
		IL_0001: ldc.i4.5            ML_0001: ldc_i4 5
		IL_0002: stloc.0             ML_0002: stloc 0
		// int num2 = 512;           
		IL_0003: ldc.i4 512          ML_0003: ldc_i4 512
		IL_0008: stloc.1             ML_0008: stloc 1
		// num += num2;              
		IL_0009: ldloc.0             ML_0009: ldloc 0
		IL_000a: ldloc.1             ML_000a: ldloc 1
		IL_000b: add                 ML_000b: add
		IL_000c: stloc.0             ML_000c: stloc 0
        // Console.WriteLine(num);
		IL_000d: ldloc.0
		IL_000e: call void [System.Console]System.Console::WriteLine(int32)
		// }
		IL_0017: nop
		IL_0018: ret
	} // end of method Program::Main
    */
    struct CIL_IL MainILs[] = {
        {.code = CIL_Nop, .arg = 0},      //IL_0000: nop                 ML_0000: nop
        {.code = CIL_Ldc_I4_5, .arg = 5}, //IL_0001: ldc.i4.5            ML_0001: ldc_i4 5
        {.code = CIL_Stloc_0, .arg = 0},  //IL_0002: stloc.0             ML_0002: stloc 0
        {.code = CIL_Ldc_I4, .arg = 512}, //IL_0003: ldc.i4 512          ML_0003: ldc_i4 512
        {.code = CIL_Stloc_1, .arg = 0},  //IL_0008: stloc.1             ML_0008: stloc 1
        {.code = CIL_Ldloc_0, .arg = 0},  //IL_0009: ldloc.0             ML_0009: ldloc 0
        {.code = CIL_Ldloc_1, .arg = 0},  //IL_000a: ldloc.1             ML_000a: ldloc 1
        {.code = CIL_Add, .arg = 0},      //IL_000b: add                 ML_000b: add
        {.code = CIL_Stloc_0, .arg = 0}   //IL_000c: stloc.0             ML_000c: stloc 0
        //,
        //{.code = CIL_Ldloc_0, .arg = 0},
        //{.code = CIL_Call, .arg = MethodHandle("Console.WriteLine(int32)")}
    };
    struct VMInterpreterType intType = {0};
    VMInterpreterType_InitFromInnerType(VM_INNER_ACTUAL_TYPE_INT, &intType);
    struct VMInterpreterLocal MainLocals[] = {
        { 
            .type = intType,
            .offset = 0
        },//[0] int32 a,
		{
            .type = intType,
            .offset = VMInnerActualType_StackSize(VM_INNER_ACTUAL_TYPE_INT)
        }//	[1] int32 b
    };
    struct VMDynamicMethodBody MainBody = {
        .ILs = MainILs,
        .ILs_count = sizeof(MainILs) / sizeof(CIL_IL)
    };
    struct VMInterpreterMethod method = {
        .method = {
            .name = "Main",
            .dynamic_method = &MainBody,
            .klass = NULL,
            .return_type = NULL,
            .parameters = NULL,
            .parameters_count = 0,
            .max_stack = 2,
            .flags = METHOD_FLAG_DYNAMIC
        },
        .locals = MainLocals,
        .locals_count = sizeof(MainLocals) / sizeof(*MainLocals),
        .arguments = NULL
    };
    void* args = NULL;
    struct VMStackFrame stackframe = create_vm_stackframe(&method, args, opstack, 4096);
    VMInterpreter interpreter = {0};
    interpreter_exec_at_stackframe(&interpreter, &method, &stackframe);
    rtcli_i64* calculated_ptr = (rtcli_i64*)stackframe.local_var_memory + 0/*slot*/;
    rtcli_i32 calculated_value = *(rtcli_i32*)calculated_ptr;
    assert(*calculated_ptr == 512 + 5);
    rtcli_info("test passed, calculated value: %d", calculated_value);    
    return 0;
}
