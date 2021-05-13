template<enum VMInterpreterArithOp op, typename T, bool IsIntType, enum VMInnerActualType cit, bool TypeIsUnchanged>
RTCLI_FORCEINLINE void VMStackFrame::DoOpCalc(T val1, T val2)
{
    T res;
    if (op == AOp_Add)
    {
        res = val1 + val2;
    }
    else if (op == AOp_Sub)
    {
        res = val1 - val2;
    }
    else if (op == AOp_Mul)
    {
        res = val1 * val2;
    }
    else
    {
        assert(op == AOp_Div || op == AOp_Rem);
        if (IsIntType)
        {
            if (val2 == 0)
            {
                assert(0 && "Divide By Zero");
                //ThrowDivideByZero();
            }
            else if (val2 == -1 && val1 == static_cast<T>(((rtcli_i64)1) << (sizeof(T)*8 - 1)))
            // min int / -1 is not representable.
            {
                assert(0 && "SysArithException");
                //ThrowSysArithException();
            }
        }
        // Otherwise...
        if (op == AOp_Div)
        {
            res = val1 / val2;
        }
        else
        {
            res = RemFunc(val1, val2);
        }
    }

    unsigned residx = ops_ht - 2;
    OpStackSetValue<T>(residx, res);
    if (!TypeIsUnchanged)
    {
        OpStackSetType(residx, VMInterpreterType(cit));
    }
}

template<enum VMInterpreterArithOp op, bool bInterpreterLooseRules>
RTCLI_FORCEINLINE void VMStackFrame::DoOp()
{
    assert(ops_ht >= 2);
    const auto op1idx = ops_ht - 2;
    const auto op2idx = ops_ht - 1;
    VMInterpreterType t1 = OpStackGetType(op1idx);
    VMInterpreterType t2 = OpStackGetType(op2idx);
    VMInnerActualType t1i = t1.InnerActualType();
    VMInnerActualType t2i = t2.InnerActualType();
    //assert(IsStackNormalType(t1.ToCorInfoType()));
    //assert(IsStackNormalType(t2.ToCorInfoType()));
    switch(t1i)
    {
    case VM_INNER_ACTUAL_TYPE_INT:
    {
        if(t1i == t2i)
        {
            rtcli_i32 val1 = OpStackGetValue<rtcli_i32>(op1idx);
            rtcli_i32 val2 = OpStackGetValue<rtcli_i32>(op2idx);
            DoOpCalc<op, rtcli_i32, /*asInt*/true, VM_INNER_ACTUAL_TYPE_INT, /*TypeIsUnchanged*/true>(val1, val2);
        }
        else
        {
            if(t2i == VM_INNER_ACTUAL_TYPE_NATIVEINT)
            {
                rtcli_isize val1 = static_cast<rtcli_isize>(OpStackGetValue<rtcli_i32>(op1idx));
                rtcli_isize val2 = OpStackGetValue<rtcli_isize>(op2idx);
                DoOpCalc<op, rtcli_isize, /*asInt*/true, VM_INNER_ACTUAL_TYPE_NATIVEINT, /*TypeIsUnchanged*/false>(val1, val2);
            }
            else if (bInterpreterLooseRules && t2i == VM_INNER_ACTUAL_TYPE_LONG)
            {
                rtcli_i64 val1 = static_cast<rtcli_i64>(OpStackGetValue<rtcli_i32>(op1idx));
                rtcli_i64 val2 = OpStackGetValue<rtcli_i64>(op2idx);
                DoOpCalc<op, rtcli_i64, /*asInt*/true, VM_INNER_ACTUAL_TYPE_LONG, /*TypeIsUnchanged*/false>(val1, val2);
            }
            else if (t2i == VM_INNER_ACTUAL_TYPE_BYREF)
            {
                if (op == AOp_Add || (bInterpreterLooseRules && op == AOp_Sub))
                {
                    // Int + ByRef = ByRef
                    rtcli_isize val1 = static_cast<rtcli_isize>(OpStackGetValue<rtcli_i32>(op1idx));
                    rtcli_isize val2 = OpStackGetValue<rtcli_isize>(op2idx);
                    DoOpCalc<op, rtcli_isize, /*IsIntType*/true, VM_INNER_ACTUAL_TYPE_BYREF, /*TypeIsUnchanged*/false>(val1, val2);
                }
                else
                {
                    assert(0 && "Operation not permitted on int and managed pointer.");
                }
            }
            else
            {
                assert(0 && "Binary arithmetic operation type mismatch (int and ?)");
            }
        }
        break;
    }
    default: assert(0 && "not implemented!"); break;
    }
    ops_ht--;
}