#include "rtcli/vm/interpreter/interpreter.h"
#include "rtcli/vm/interpreter/stack.h"

#include <algorithm>
#include <cassert>

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
}

extern "C"
{
    struct VMStackFrame create_vmstack(VMInterpreterMethod* method_info, rtcli_byte* args,
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
            //LargeStructOperandStackPop(sz, srcAddr);
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