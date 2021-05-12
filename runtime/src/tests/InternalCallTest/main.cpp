#include <iostream>
#include "rtcli/vm/string.h"
#include "rtcli/vm/array.h"
#include "rtcli/vm/gc.h"
#include "rtcli/vm/interop.h"

#ifdef __cplusplus
typedef struct VMStringEditable : public RTCLI::System::__object
{
#else
typedef struct VMStringEditable
{
    rtcli_object object;
#endif
    rtcli_i32 length;
    const rtcli_char* chars;
} VMStringEditable;

int main()
{
    size_t bufferLength = 10;
    int size = (offsetof(VMString, chars) + (((size_t)bufferLength + 1) * 2));
	VMString* string = rtcli_gc_alloc_string(nullptr, size, bufferLength);
    auto str = L"0123456789";
    VMStringEditable* hack = reinterpret_cast<VMStringEditable*>(string);
    hack->chars = str;

    /*
    C =>
    INTERNAL_CALL
    RTCLI_EXTERN_C RTCLI_API void rtcli_interop_beep(rtcli_i32 frequency, rtcli_i32 duration);

    C++ => 
    INTERNAL_CALL
    RTCLI_API RTCLI::Interop::Beep(rtcli_i32 frequency, rtcli_i32 duration);
    
    C# =>
    namespace Interop 
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static void beep(int frequency, int duration);
    }
    */
    RTCLI::Interop::Beep(800, 200);
    rtcli_interop_beep(800, 200);
    rtcli_interop_beep(10, 10);
    rtcli_interop_beep(100000, 10);
    rtcli_interop_beep(100000, -10);

    VMArray* array = nullptr;
    RTCLI::InternalCall::String::Constructor(*string, *array);

    return 0;
}