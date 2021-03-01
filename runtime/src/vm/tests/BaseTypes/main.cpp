#include <iostream>
#include "rtcli/base-types.h"

int main()
{
    const rtcli_native_char* string = RTCLI_NATIVE_STRING("Test String");
    std::wcout << string << std::endl;
    return 0;
}