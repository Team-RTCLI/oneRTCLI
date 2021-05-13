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
    struct VMStackFrame create_vm_stackframe(VMInterpreterMethod* method_info, rtcli_byte* args,
        rtcli_byte* frame_memory, rtcli_usize lss_alloc_size)
    {
        rtcli_byte* frame_mem_cursor = frame_memory;
        rtcli_byte* initlss = NULL;
        
        if(lss_alloc_size > 0)
        {
            initlss = (rtcli_byte*)malloc(lss_alloc_size);
        }

        VMStackFrame stackframe = {0};
        {
            stackframe.locals_size = method_info->LocalsMemorySize();
            stackframe.local_var_memory = frame_mem_cursor;
            frame_mem_cursor += stackframe.locals_size;

            stackframe.method = method_info;
            stackframe.ops = (struct VMStackOp*)frame_mem_cursor;
            stackframe.ops_ht = 0;
            stackframe.args = args;
            stackframe.lss = initlss;
            stackframe.lss_alloc_size = lss_alloc_size;
        }
        return stackframe;
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


void VMInterpreter::Exec(struct VMStackFrame* stack, const struct MIL_IL il)
{
    switch(il.code)
    {
    case MIL_Nop: return;

    case MIL_Ldc_I4: return vm_exec_ldc_i4(stack, static_cast<rtcli_i32>(il.arg));
    case MIL_Ldloc: return vm_exec_ldloc(stack, il.arg);
    case MIL_Stloc: return vm_exec_stloc(stack, il.arg);
    case MIL_Add: return vm_exec_add(stack);
    default:
        assert(0 && "not implemented!");    
    }
}

void VMInterpreter::Exec(struct VMStackFrame* stack, const struct CIL_IL il)
{
    switch(il.code)
    {
    case CIL_Nop: return;
    
    case CIL_Ldc_I4: return vm_exec_ldc_i4(stack, static_cast<rtcli_i32>(il.arg));
    case CIL_Ldc_I4_0: return vm_exec_ldc_i4(stack, 0);
    case CIL_Ldc_I4_1: return vm_exec_ldc_i4(stack, 1);
    case CIL_Ldc_I4_2: return vm_exec_ldc_i4(stack, 2);
    case CIL_Ldc_I4_3: return vm_exec_ldc_i4(stack, 3);
    case CIL_Ldc_I4_4: return vm_exec_ldc_i4(stack, 4);
    case CIL_Ldc_I4_5: return vm_exec_ldc_i4(stack, 5);
    case CIL_Ldc_I4_6: return vm_exec_ldc_i4(stack, 6);
    case CIL_Ldc_I4_7: return vm_exec_ldc_i4(stack, 7);
    case CIL_Ldc_I4_8: return vm_exec_ldc_i4(stack, 8);

    case CIL_Ldloc: return vm_exec_ldloc(stack, il.arg);
    case CIL_Ldloc_0: return vm_exec_ldloc(stack, 0);
    case CIL_Ldloc_1: return vm_exec_ldloc(stack, 1);
    case CIL_Ldloc_2: return vm_exec_ldloc(stack, 2);
    case CIL_Ldloc_3: return vm_exec_ldloc(stack, 3);

    case CIL_Stloc: return vm_exec_stloc(stack, il.arg);
    case CIL_Stloc_0: return vm_exec_stloc(stack, 0);
    case CIL_Stloc_1: return vm_exec_stloc(stack, 1);
    case CIL_Stloc_2: return vm_exec_stloc(stack, 2);
    case CIL_Stloc_3: return vm_exec_stloc(stack, 3);
    
    case CIL_Add: return vm_exec_add(stack);
    default:
        assert(0 && "not implemented!");    
    }
}

void VMInterpreter::Exec(struct VMStackFrame* stack, struct VMInterpreterMethod* method)
{
    if(method->optimized_dynamic_method != NULL)
    {
        const auto& optimized = *(method->optimized_dynamic_method);
        for(auto index = 0; index < optimized.ILs_count; index++)
        {
            const auto& IL = optimized.ILs[index];
            Exec(stack, IL);
        }
    }
    else
    {
        const auto& dynamic_method = *(method->method.dynamic_method);
        for(auto index = 0; index < dynamic_method.ILs_count; index++)
        {
            const auto& IL = dynamic_method.ILs[index];
            Exec(stack, IL);
        }
    }
    return;
}

extern "C"
{
    void interpreter_exec(struct VMInterpreter* interpreter, struct VMInterpreterMethod* method)
    {
        assert(0 && "not implemented!");
    }

    void interpreter_exec_at_stackframe(
        struct VMInterpreter* interpreter, struct VMInterpreterMethod* method, struct VMStackFrame* stackframe)
    {
        interpreter->Exec(stackframe, method);
    }
}