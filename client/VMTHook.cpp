#include "VMTHook.hpp"

#include <Windows.h>

VMTHook::VMTHook(void* Instance)
{
    VT_Backup = *reinterpret_cast<unsigned int**>(Instance);
    while (reinterpret_cast<unsigned int*>(*reinterpret_cast<unsigned int*>(Instance))[VT_Functions])
    {
        VT_Functions++;
    }
    const unsigned int VT_Size = ((VT_Functions * 0x4) + 0x4);
    VT_New = std::make_unique<unsigned int[]>(VT_Functions + 1);
    memcpy(VT_New.get(), &VT_Backup[-1], VT_Size);
    *reinterpret_cast<unsigned int**>(Instance) = &VT_New.get()[1];
}

void* VMTHook::GetDefaultFunc(const unsigned int Index)
{
    return reinterpret_cast<void*>(VT_Backup[Index]);
}

void VMTHook::HookFunc(void* New, const unsigned int Index)
{
    VT_New[Index + 1] = reinterpret_cast<unsigned int>(New);
}

void VMTHook::UnhookFunc(const unsigned int Index)
{
    VT_New[Index + 1] = VT_Backup[Index];
}

//Todo: Make actual RestoreVMT Function
void VMTHook::RestoreVMT()
{
    VT_New.get()[1] = VT_Backup[0];
    delete[] VT_Backup;
}

//A more simple VMTHook method. Will just replace the vtable pointer instead copying the whole vtable
void* VMTHook::ReplaceVtablePointer(unsigned int* VTable, int Index, unsigned int New)
{
	unsigned long oldProtection;
	VirtualProtect(&VTable[Index], sizeof(uintptr_t), 0x40, &oldProtection);
	void* oldFunc = reinterpret_cast<void*>(VTable[Index]);
	VTable[Index] = New;
	VirtualProtect(&VTable[Index], sizeof(uintptr_t), oldProtection, &oldProtection);
	return oldFunc;
}