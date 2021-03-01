#include <iostream>
#include "rtcli/vm/gc.h"
#include "rtcli/internal/log.h"

int main()
{
    rtcli_object* object = rtcli_gc_alloc_obj(nullptr, 50);

    if(!object)
    {
        rtcli_error("gc_alloc failed!");
    } else
    {
        rtcli_info("gc_alloc succeed!");
    }
    return 0;
}