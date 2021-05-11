#define DLL_IMPLEMENTATION
#include "rtcli/vm/gc.h"
#include "rtcli/vm/object.h"
#include "rtcli/vm/class.h"
#include "rtcli/vm/string.h"
#include "rtcli/vm/array.h"
#include "rtcli/PIL.h"

#ifdef RTCLI_NO_GC

RTCLI_EXTERN_C RTCLI_API
struct rtcli_object*
rtcli_gc_alloc_obj(rtcli_vtable* vtable, size_t size)
{
	rtcli_object *obj = rtcli_calloc(1, size);
	obj->p_vtable = vtable;
	return obj;
}

RTCLI_EXTERN_C RTCLI_API
struct VMString*
rtcli_gc_alloc_string(rtcli_vtable* vtable, size_t size, rtcli_i32 length)
{
	VMString *str = rtcli_calloc(1, size);
	
	str->object.p_vtable = vtable;
	str->length = length;
	str->chars [length] = 0;

	return str;
}

RTCLI_EXTERN_C RTCLI_API 
struct VMArray*
rtcli_gc_alloc_array(rtcli_vtable* vtable, rtcli_usize size, rtcli_uintptr max_length, rtcli_uintptr bounds_size)
{
	VMArray* obj = rtcli_calloc(1, size);

	obj->object.p_vtable = vtable;
	obj->max_length = max_length;

	if (bounds_size)
		obj->bounds = (VMArrayBounds*)((char *) obj + size - bounds_size);

	return obj;
}

#endif