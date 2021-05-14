#define DLL_IMPLEMENTATION
#include <iostream>
#include <type_traits>
#include "rtcli/icalls/Console.h"
#include "rtcli/vm/interpreter/stack.h"

const rtcli_char* nextline = RTCLI_STRING("\n");

void DoWrite(const rtcli_i32 i)
{
    std::wcout << i;
}

void DoWrite(const rtcli_char* s, rtcli_i32 length)
{
    if(length == 0 || s == nullptr)
    {
        return;
    }
    std::wstring_view output = std::wstring_view(s, length);
    std::wcout << output;
}

void RTCLI::Internal::Console::Write(rtcli_i32 i)
{
    DoWrite(i);
}

void RTCLI::Internal::Console::Write(VMString* s)
{
    if(s != nullptr)
    {
        DoWrite(s->chars, s->length);
    }
}

void RTCLI::Internal::Console::Write(VMString& s)
{
    DoWrite(s.chars, s.length);
}

void RTCLI::Internal::Console::WriteLine(VMString* s)
{
    if(s != nullptr)
    {
        DoWrite(s->chars, s->length);
    }
    DoWrite(nextline, 1);
}

void RTCLI::Internal::Console::WriteLine(VMString& s)
{
    DoWrite(s.chars, s.length);
    DoWrite(nextline, 1);
}

extern "C"
{
    void VM_Console_Write(VMString* s)
    {
        RTCLI::Internal::Console::Write(s);
    }
    void VM_Console_Write_I32(rtcli_i32 i)
    {
        RTCLI::Internal::Console::Write(i);
    }
    void VM_Console_WriteLine(VMString* s)
    {
        RTCLI::Internal::Console::WriteLine(s);
    }

    void VMInternal_Console_Write_I32(struct VMStackFrame* stackframe)
    {
        const auto opidx = stackframe->ops_ht--;
        auto i = stackframe->OpStackGetValue<rtcli_i32>(opidx);
        RTCLI::Internal::Console::Write(i);
    }
}
