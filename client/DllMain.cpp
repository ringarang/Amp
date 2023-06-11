#include "Amp.hpp"

#include <cstdint>
#include <psapi.h>

void AnnihilateIsTainted() {
	const std::uintptr_t wtdll = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WINTRUST.dll"));
	const std::uintptr_t wvt = reinterpret_cast<DWORD>(GetProcAddress(reinterpret_cast<HMODULE>(wtdll), "WinVerifyTrust"));

	const std::uint8_t nBytes[] = { 0x31, 0xC0, 0x5D, 0xC2, 0x0C, 0x00 };
	const std::uint32_t nByteLength = sizeof(nBytes);

	DWORD oldProtection;
	VirtualProtect(reinterpret_cast<void*>(wvt), nByteLength, 0x40, &oldProtection);
	for (std::uint32_t x = 0; x < sizeof(nBytes); x++) reinterpret_cast<std::uint8_t*>(wvt + x)[3] = nBytes[x];
	VirtualProtect(reinterpret_cast<void*>(wvt), nByteLength, oldProtection, &oldProtection);
}

void AnnihilateSetUnhandledExceptionFilter()
{
    const std::uintptr_t Kernel = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("KERNEL32.dll"));
    const std::uintptr_t Filter = reinterpret_cast<std::uintptr_t>(GetProcAddress(reinterpret_cast<HMODULE>(Kernel), "SetUnhandledExceptionFilter"));

    const std::uint8_t Bytes[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

    DWORD ulOldProtect;
    VirtualProtect(reinterpret_cast<void*>(Filter), sizeof(Bytes), 0x40, &ulOldProtect);
    for (std::uint32_t Iteration = 0; Iteration < sizeof(Bytes); Iteration++)
    {
        reinterpret_cast<std::uint8_t*>(Filter)[Iteration] = Bytes[Iteration];
    }
    VirtualProtect(reinterpret_cast<void*>(Filter), sizeof(Bytes), ulOldProtect, &ulOldProtect);
}

void toClipboard(HWND hwnd, const char* s) {
    OpenClipboard(hwnd);
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, strlen(s) + 1);
    if (!hg) {
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(hg), s, strlen(s) + 1);
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
}


long __stdcall TopLevelExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    const int res = MessageBoxA(0, "Roblox crashed surely because of an issue from amp.\nCommon fixes:\n\t- If Roblox recently updated, wait for amp to update too.\n\nFor more support, ping the owners on the beta testing server. (remember to complete reports)\nDo you want to copy details to clipboard?", "amp crash handler", MB_YESNO);

    if (res == IDYES) {
        toClipboard(NULL, "todo: add traceback");
    }
    MessageBoxA(0, "Roblox will now be force closed.", "amp crash handler", MB_OK);
    exit(1);
    return EXCEPTION_EXECUTE_HANDLER;
}

int __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
		AnnihilateIsTainted();
        AnnihilateSetUnhandledExceptionFilter();

        SetUnhandledExceptionFilter(TopLevelExceptionHandler);
		DisableThreadLibraryCalls(hinstDLL);

        CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&Amp::Main), hinstDLL, NULL, NULL);
    }
    return 1;
}
