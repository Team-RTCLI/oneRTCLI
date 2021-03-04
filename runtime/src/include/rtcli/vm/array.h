#pragma once
#include "rtcli/base-types.h"
#include "rtcli/vm/object.h"

typedef rtcli_i32 rtcli_array_lower_bound;
typedef rtcli_u32 rtcli_array_size;
typedef struct VMArrayBounds
{
    rtcli_array_size length;
    rtcli_array_lower_bound lower_bound;
} VMArrayBounds;

#ifdef __cplusplus
typedef struct VMArray : public RTCLI::System::__object
{
#else
typedef struct VMArray
{
    rtcli_object object;
#endif //__cplusplus
    /* bounds is NULL for szarrays */
    VMArrayBounds* bounds;
    /* total number of elements of the array */
    rtcli_array_size max_length;
    /* we use mono_64bitaligned_t to ensure proper alignment on platforms that need it */
    rtcli_64bit_aligned vector[RTCLI_ZERO_LEN_ARRAY];
} VMArray;