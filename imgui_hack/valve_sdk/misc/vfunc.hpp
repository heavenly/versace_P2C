#pragma once

template<typename FuncType>
__forceinline static FuncType call_vfunc(void* pp_class, int index)
{
    int* ptr_vtable = *(int**)pp_class;
    int dw_addr = ptr_vtable[index];
    return (FuncType)(dw_addr);
}
