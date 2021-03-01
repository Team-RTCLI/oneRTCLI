#pragma once
#include "rtcli/config.h"

struct rtcli_object;
struct rtcli_vtable;

RTCLI_EXTERN_C RTCLI_API 
struct rtcli_object*
rtcli_gc_alloc_obj(struct rtcli_vtable* vtable, size_t size);

