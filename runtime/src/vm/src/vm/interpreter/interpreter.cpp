#include "rtcli/vm/interpreter/interpreter.h"
#include "rtcli/vm/interpreter/mcode.h"
#include "rtcli/vm/interpreter/stack.h"

#include <algorithm>
#include <cassert>
#include <cmath>

extern "C"
{
    rtcli_bool VMInterpreterType_isLargeStruct(VMInterpreterType type)
    {
        return type.isLargeStruct();
    }

    rtcli_usize VMInterpreterType_ActualSize(VMInterpreterType type)
    {
        return type.ActualSize();
    }

    void VMInterpreterType_InitFromInnerType(VMInnerActualType inner, VMInterpreterType* type)
    {
        new (type) VMInterpreterType(inner);
    }
}

extern "C"
{
    void vm_exec_nop(struct VMStackFrame* stack)
    {
        // Current DoNothing...
    }

    void vm_exec_ldarg(struct VMStackFrame* stack, int arg_index)
    {
        stack->LdFromMemAddr(stack->GetArgAddr(arg_index), stack->GetArgType(arg_index));
    }

    void vm_exec_ldc_i4(struct VMStackFrame* stack, rtcli_i32 value)
    {
        stack->OpStackSetType(stack->ops_ht, VMInterpreterType(VM_INNER_ACTUAL_TYPE_INT));
        stack->OpStackSetValue<rtcli_i32>(stack->ops_ht, value);
        stack->ops_ht++;
    }

    void vm_exec_stloc(struct VMStackFrame* stack, rtcli_i32 loc_index)
    {
        assert(stack->ops_ht >= 1);
    
        // Don't decrement "m_curStackHt" early -- if we do, then we'll have a potential GC hole, if
        // the top-of-stack value is a GC ref.
        const auto ind = stack->ops_ht - 1;
        auto target = stack->FixedSizeLocalSlot(loc_index);
        VMInterpreterType tp = stack->method->locals[loc_index].type;
        if(tp.isLargeStruct())
        {
            assert(0);
        }
        else
        {
            const auto value = stack->OpStackGetValue<rtcli_i64>(ind);
            *target = value;
        }
        stack->ops_ht = ind;
    }
    
    void vm_exec_ldloc(struct VMStackFrame* stack, rtcli_i32 loc_index)
    {
        const auto stackHt = stack->ops_ht;
        const auto slot = stack->FixedSizeLocalSlot(loc_index);
        stack->OpStackSetValue<rtcli_i64>(stackHt, *slot);
        VMInterpreterType tp = stack->method->locals[loc_index].type;
        stack->OpStackSetType(stackHt, tp);
        stack->ops_ht = stackHt + 1;
    }

    void vm_exec_add(struct VMStackFrame* stack)
    {
        stack->DoOp<AOp_Add>();
    }

}

void VMStackFrame::LargeStructStackCanPush(rtcli_usize sz)
{
    rtcli_usize remaining = lss_alloc_size - lss_ht;
    if (remaining < sz)
    {
        rtcli_usize newAllocSize = std::max(lss_alloc_size + sz * 4, lss_alloc_size * 2);
        rtcli_byte* newStack = new rtcli_byte[newAllocSize];
        lss_alloc_size = newAllocSize;
        if(lss != nullptr)
        {
            memcpy(newStack, lss, lss_ht);
            delete[] lss;
        } 
        lss = newStack;
    }
}

void* VMStackFrame::LargeStructStackPush(rtcli_usize sz)
{
    // ensure stack memory
    LargeStructStackCanPush(sz);
    assert(lss_alloc_size >= lss_ht + sz);
    void* res = &lss[lss_ht];
    lss_ht += sz;
    return res;
}

void VMStackFrame::LdFromMemAddr(void* addr, struct VMInterpreterType type)
{
    auto stackHt = ops_ht;
    rtcli_usize sz = type.ActualSize();

    OpStackSetType(stackHt, type);

    // User Struct: 0x1 & 0x3
    if(type.isStruct())
    {
        if(type.isLargeStruct())
        {
            void* ptr = LargeStructStackPush(sz);
            memcpy(ptr, addr, sz);
            OpStackSetValue<void*>(stackHt, ptr);
        }
        else
        {
            OpStackSetValue<rtcli_i64>(stackHt, GetSmallStructValue(addr, sz));
        }
        ops_ht = stackHt + 1;
        return;
    }

    // Otherwise...
    if (sz == 4)
    {
        OpStackSetValue<rtcli_i32>(stackHt, *reinterpret_cast<rtcli_i32*>(addr));
    }
    else if (sz == 1)
    {
        VMInnerActualType it = type.InnerActualType();
        if (VMInnerActualType_isUnsined(it))
        {
            OpStackSetValue<rtcli_u32>(stackHt, *reinterpret_cast<rtcli_u8*>(addr));
        }
        else
        {
            OpStackSetValue<rtcli_i32>(stackHt, *reinterpret_cast<rtcli_i8*>(addr));
        }
    }
    else if (sz == 8)
    {
        OpStackSetValue<rtcli_i64>(stackHt, *reinterpret_cast<rtcli_i64*>(addr));
    }
    else
    {
        assert(sz == 2); // only remaining case.
        VMInnerActualType it = type.InnerActualType();
        if (VMInnerActualType_isUnsined(it))
        {
            OpStackSetValue<rtcli_u32>(stackHt, *reinterpret_cast<rtcli_u16*>(addr));
        }
        else
        {
            OpStackSetValue<rtcli_i32>(stackHt, *reinterpret_cast<rtcli_i16*>(addr));
        }
    }
    ops_ht = stackHt + 1;
}

void VMStackFrame::StToMemAddr(void* addr, struct VMInterpreterType type)
{
    assert(ops_ht >= 1);
    ops_ht--;

    rtcli_usize sz = type.SizeOnStack();
    if (type.isStruct())
    {
        if (type.isLargeStruct())
        {
            // Large struct case.
            void* srcAddr = OpStackGetValue<void*>(ops_ht);
            memcpy(addr, srcAddr, sz);
            LargeStructOperandStackPop(sz, srcAddr);
        }
        else
        {
            memcpy(addr, OpStackGetValueAddr(ops_ht), sz);
        }
        return;
    }

    // Note: this implementation assumes a little-endian architecture.
    if (sz == 4)
    {
        *reinterpret_cast<rtcli_i32*>(addr) = OpStackGetValue<rtcli_i32>(ops_ht);
    }
    else if (sz == 1)
    {
        *reinterpret_cast<rtcli_i8*>(addr) = OpStackGetValue<rtcli_i8>(ops_ht);
    }
    else if (sz == 8)
    {
        *reinterpret_cast<rtcli_i64*>(addr) = OpStackGetValue<rtcli_i64>(ops_ht);
    }
    else
    {
        assert(sz == 2);
        *reinterpret_cast<rtcli_i16*>(addr) = OpStackGetValue<rtcli_i16>(ops_ht);
    }
}

struct VMStackFrame* VMInterpreter::DiscardStackFrame()
{
    VMStackFrame* now = sfs + sf_size - 1;
    auto* fmem_start = now->local_var_memory;
    auto* fmem_end = now->ops + now->ops_ht;
#if false
    //set 0xcdcd of old memory
    memset(fmem_start, 0, (rtcli_byte*)fmem_end - fmem_start);
#endif
    opstack_current = fmem_start; // start of frame_memeory
    sf_size--;
    VMStackFrame* last = sfs + sf_size - 1;
    return last;
}

struct VMStackFrame* VMInterpreter::NextStackFrame()
{
    // Root
    VMStackFrame* now = sfs + sf_size - 1;
    opstack_current = (rtcli_byte*)(now->ops + now->ops_ht);
    sf_size++;
    VMStackFrame* next = sfs + sf_size - 1;
    return next;
}

struct VMStackFrame* VMInterpreter::CurrentStackFrame()
{
    return &sfs[sf_size - 1];
}

void VMInterpreter::Exec(struct VMStackFrame* stack, const struct MIL_IL il)
{
    switch(il.code)
    {
    case MIL_Nop: vm_exec_nop(stack); break;

    case MIL_Ldc_I4: 
    vm_exec_ldc_i4(stack, static_cast<rtcli_i32>(il.arg));
    break;
    case MIL_Ldloc: vm_exec_ldloc(stack, il.arg);break;
    case MIL_Stloc: vm_exec_stloc(stack, il.arg);break;
    case MIL_Add: vm_exec_add(stack);break;
    case MIL_Call: 
    {
        ExecNextStack((VMInterpreterMethod*)il.arg, NULL);
        break;
    }
    default:
        assert(0 && "not implemented!");
        break;
    }
}

void VMInterpreter::Exec(struct VMStackFrame* stack, const struct CIL_IL il)
{
    switch(il.code)
    {
    case CIL_Nop: vm_exec_nop(stack); break;
    
    case CIL_Ldc_I4: 
        vm_exec_ldc_i4(stack, static_cast<rtcli_i32>(il.arg));
        break;
    case CIL_Ldc_I4_0: vm_exec_ldc_i4(stack, 0);break;
    case CIL_Ldc_I4_1: vm_exec_ldc_i4(stack, 1);break;
    case CIL_Ldc_I4_2: vm_exec_ldc_i4(stack, 2);break;
    case CIL_Ldc_I4_3: vm_exec_ldc_i4(stack, 3);break;
    case CIL_Ldc_I4_4: vm_exec_ldc_i4(stack, 4);break;
    case CIL_Ldc_I4_5: vm_exec_ldc_i4(stack, 5);break;
    case CIL_Ldc_I4_6: vm_exec_ldc_i4(stack, 6);break;
    case CIL_Ldc_I4_7: vm_exec_ldc_i4(stack, 7);break;
    case CIL_Ldc_I4_8: vm_exec_ldc_i4(stack, 8);break;

    case CIL_Ldloc: vm_exec_ldloc(stack, il.arg);break;
    case CIL_Ldloc_0: vm_exec_ldloc(stack, 0);break;
    case CIL_Ldloc_1: vm_exec_ldloc(stack, 1);break;
    case CIL_Ldloc_2: vm_exec_ldloc(stack, 2);break;
    case CIL_Ldloc_3: vm_exec_ldloc(stack, 3);break;

    case CIL_Stloc: vm_exec_stloc(stack, il.arg);break;
    case CIL_Stloc_0: vm_exec_stloc(stack, 0);break;
    case CIL_Stloc_1: vm_exec_stloc(stack, 1);break;
    case CIL_Stloc_2: vm_exec_stloc(stack, 2);break;
    case CIL_Stloc_3: vm_exec_stloc(stack, 3);break;
    
    case CIL_Add: vm_exec_add(stack);break;
    
    default:
        assert(0 && "not implemented!");    
        break;
    }
}

void VMInterpreter::Exec(struct VMStackFrame* stack,
    struct VMInterpreterMethod* method, rtcli_arg_slot* args)
{
    // Interpreter Method Body
    if(method->optimized_dynamic_method != NULL)
    {
        const auto& optimized = *(method->optimized_dynamic_method);
        while(stack->ip < optimized.ILs_count)
        {
            const auto& IL = optimized.ILs[stack->ip];
            Exec(stack, IL);
            stack->ip++;
        }
    }
    else if ((method->method.flags & METHOD_FLAG_DYNAMIC) == METHOD_FLAG_DYNAMIC)
    {
        const auto& dynamic_method = *(method->method.dynamic_method);
        while(stack->ip < dynamic_method.ILs_count)
        {
            const auto& IL = dynamic_method.ILs[stack->ip];
            Exec(stack, IL);
            stack->ip++;
        }
    }
    else if ((method->method.flags & METHOD_FLAG_NATIVE) == METHOD_FLAG_NATIVE)
    {
        const auto& static_method = *(method->method.method_pointer);
        static_method(stack);
    }
    return;
}

void VMStackFrame_Init(
    VMStackFrame* stackframe, VMInterpreterMethod* method_info,
    rtcli_byte* frame_memory)
{
    rtcli_byte* frame_mem_cursor = frame_memory;

    {
        stackframe->ip = 0;

        stackframe->locals_size = method_info->LocalsMemorySize();
        stackframe->local_var_memory = frame_mem_cursor;
        frame_mem_cursor += stackframe->locals_size;

        stackframe->method = method_info;
        stackframe->ops = (struct VMStackOp*)frame_mem_cursor;
        stackframe->ops_ht = 0;

        stackframe->lss = NULL;
        stackframe->lss_alloc_size = 0;
    }
}

void VMStackFrame_LoadArgs(VMStackFrame* oldstack, VMStackFrame* sf)
{
    const auto argN = sf->method->method.parameters_count;
    oldstack->ops_ht -= argN;
    sf->args = oldstack->ops + oldstack->ops_ht - 1;
}

void VMInterpreter::ExecNextStack(
    struct VMInterpreterMethod* method, rtcli_arg_slot* args)
{
    struct VMStackFrame* oldstack = CurrentStackFrame();
    auto oldip = oldstack->ip;
    // Next Frame
    struct VMStackFrame* stack = NextStackFrame();
    // Setup
    VMStackFrame_Init(stack, method, opstack_current);
    VMStackFrame_LoadArgs(oldstack, stack);
    // Exec
    Exec(stack, method, args);
    // Back Frame
    stack = DiscardStackFrame();
}

VMInterpreter::~VMInterpreter()
{
    delete[] sfs;
}

VMInterpreter::VMInterpreter(rtcli_usize lss_alloc_size)
{
    sfs = new VMStackFrame[16]();
    sf_size = 1;
    sf_capacity = 16;
    opstack_current = &opstack[0];
    sfs->ops = (struct VMStackOp*)opstack_current;
    sfs->ops_ht = 0;
}

extern "C"
{
    void VMInterpreter_Init(
        struct VMInterpreter* interpreter, rtcli_usize lss_alloc_size)
    {
        new (interpreter) VMInterpreter(lss_alloc_size);
    }

    void VMInterpreter_Destroy(struct VMInterpreter* interpreter)
    {
        interpreter->~VMInterpreter();
    }

    void VMInterpreter_Exec(struct VMInterpreter* interpreter,
        struct VMInterpreterMethod* method)
    {
        // Setup
        auto* stack = &interpreter->sfs[interpreter->sf_size - 1];
        VMStackFrame_Init(stack, method, interpreter->opstack_current);
        interpreter->Exec(stack, method, NULL);
    }
}