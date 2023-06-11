#pragma once
#include <fstream>
#include <vector>

class VMTHook {
    std::unique_ptr<unsigned int[]> VT_New = nullptr;
    unsigned int* VT_Backup = nullptr;
    unsigned int VT_Functions = 0;

public:
    VMTHook(void* Instance);

    void* GetDefaultFunc(const unsigned int Index);
    void HookFunc(void* New, const unsigned int Index);
    void UnhookFunc(const unsigned int Index);
    void RestoreVMT();
	void* ReplaceVtablePointer(unsigned int* VTable, int Index, unsigned int New);
};