#define DLL_IMPLEMENTATION
#include "rtcli/vm/gc.h"
#include "rtcli/vm/object.h"
#include "rtcli/vm/vtable.h"
#include "rtcli/PIL.h"

#ifdef RTCLI_NO_GC

RTCLI_EXTERN_C RTCLI_API
struct rtcli_object*
rtcli_gc_alloc_obj(struct rtcli_vtable* vtable, size_t size)
{
	rtcli_object *obj = rtcli_calloc(1, size);
	obj->p_vtable = vtable;
	return obj;
}

#endif