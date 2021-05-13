#define DLL_IMPLEMENTATION
#include "rtcli/vm/gc.h"
#include "rtcli/vm/object.h"
#include "rtcli/metadata/class.h"

#ifdef RTCLI_BOEHM_GC

RTCLI_EXTERN_C RTCLI_API
rtcli_object*
rtcli_gc_alloc_obj(rtcli_vtable* vtable, size_t size)
{
	rtcli_object *obj = boehm_gc_alloc (1, size);
	obj->p_vtable = vtable;
	return obj;
}

#endif