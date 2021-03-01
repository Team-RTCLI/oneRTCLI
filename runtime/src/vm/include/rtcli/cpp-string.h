#pragma once
#include "rtcli/base-types.h"
#ifdef __cplusplus
#include <string>
namespace RTCLI
{
    typedef std::basic_string<rtcli_char> UTF16String;
    typedef std::basic_string<rtcli_native_char> NativeString;
}
#endif