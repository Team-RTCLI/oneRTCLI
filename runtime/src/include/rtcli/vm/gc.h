#pragma once
#include "rtcli/base-types.h"

struct rtcli_object;
typedef struct VMClass VMClass;
typedef VMClass rtcli_vtable;
struct VMString;
struct VMArray;

RTCLI_EXTERN_C RTCLI_API 
struct rtcli_object*
rtcli_gc_alloc_obj(rtcli_vtable* rtcli_usize, size_t size);


RTCLI_EXTERN_C RTCLI_API 
struct VMString*
rtcli_gc_alloc_string(rtcli_vtable* vtable, rtcli_usize size, rtcli_i32 length);


RTCLI_EXTERN_C RTCLI_API 
struct VMArray*
rtcli_gc_alloc_array(rtcli_vtable* vtable, rtcli_usize size, rtcli_uintptr max_length, rtcli_uintptr bounds_size);

