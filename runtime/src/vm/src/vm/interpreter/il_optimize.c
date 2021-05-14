#define DLL_IMPLEMENTATION
#include <stdlib.h>
#include "rtcli/vm/interpreter/mcode.h"
#include "rtcli/vm/interpreter/meta.h"

void CIL_Optim2MIL(const struct CIL_IL* cil, struct MIL_IL* mil)
{
    // Remove Index in OpCodes for better runtime switch performance
    switch(cil->code)
    {
    case CIL_Ldarg_0: mil->code = MIL_Ldarg; mil->arg = 0; break;
    case CIL_Ldarg_1: mil->code = MIL_Ldarg; mil->arg = 1; break;
    case CIL_Ldarg_2: mil->code = MIL_Ldarg; mil->arg = 2; break;
    case CIL_Ldarg_3: mil->code = MIL_Ldarg; mil->arg = 3; break;
    
    case CIL_Ldloc_0: mil->code = MIL_Ldloc; mil->arg = 0; break;
    case CIL_Ldloc_1: mil->code = MIL_Ldloc; mil->arg = 1; break;
    case CIL_Ldloc_2: mil->code = MIL_Ldloc; mil->arg = 2; break;
    case CIL_Ldloc_3: mil->code = MIL_Ldloc; mil->arg = 3; break;

    case CIL_Stloc_0: mil->code = MIL_Stloc; mil->arg = 0; break;
    case CIL_Stloc_1: mil->code = MIL_Stloc; mil->arg = 1; break;
    case CIL_Stloc_2: mil->code = MIL_Stloc; mil->arg = 2; break;
    case CIL_Stloc_3: mil->code = MIL_Stloc; mil->arg = 3; break;

    case CIL_Ldc_I4_0: mil->code = MIL_Ldc_I4; mil->arg = 0; break;
    case CIL_Ldc_I4_1: mil->code = MIL_Ldc_I4; mil->arg = 1; break;
    case CIL_Ldc_I4_2: mil->code = MIL_Ldc_I4; mil->arg = 2; break;
    case CIL_Ldc_I4_3: mil->code = MIL_Ldc_I4; mil->arg = 3; break;
    case CIL_Ldc_I4_4: mil->code = MIL_Ldc_I4; mil->arg = 4; break;
    case CIL_Ldc_I4_5: mil->code = MIL_Ldc_I4; mil->arg = 5; break;
    case CIL_Ldc_I4_6: mil->code = MIL_Ldc_I4; mil->arg = 6; break;
    case CIL_Ldc_I4_7: mil->code = MIL_Ldc_I4; mil->arg = 7; break;
    case CIL_Ldc_I4_8: mil->code = MIL_Ldc_I4; mil->arg = 8; break;

    default: 
        mil->code = (MIL_OpCode)cil->code; mil->arg = cil->arg;
        break;
    }
}

void MethodBody_Optim2MIL(const struct VMCILMethodBody* raw, struct VMMILMethodBody* optimized)
{
    // Generate Optimized ILs.
    optimized->ILs = (MIL_IL*)malloc(sizeof(MIL_IL) * raw->ILs_count);
    optimized->ILs_count = raw->ILs_count;
    for(rtcli_usize i = 0; i < optimized->ILs_count; i++)
    {
        CIL_Optim2MIL(&raw->ILs[i], &optimized->ILs[i]);
    }
    //...
    return;
}