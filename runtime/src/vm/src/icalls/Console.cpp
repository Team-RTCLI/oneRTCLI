#define DLL_IMPLEMENTATION
#include <iostream>
#include <type_traits>
#include "rtcli/icalls/Console.h"

const rtcli_char* nextline = RTCLI_STRING("\n");

void DoWrite(const rtcli_char* s, rtcli_i32 length)
{
    if(length == 0 || s == nullptr)
    {
        return;
    }
    std::wstring_view output = std::wstring_view(s, length);
    std::wcout << output;
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
    void VMInternal_Console_Write(VMString* s)
    {
        RTCLI::Internal::Console::Write(s);
    }
    void VMInternal_Console_WriteLine(VMString* s)
    {
        RTCLI::Internal::Console::WriteLine(s);
    }
}
