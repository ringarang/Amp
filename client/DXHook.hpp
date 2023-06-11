#include <Windows.h>
#include <d3d11.h>
#include <functional>
#include <unordered_map>
#include <cstdint>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/TextEditor.h"
#include "imgui/imgui_internal.h"
#pragma comment(lib, "d3d11.lib")

typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(__stdcall* tResizeBuffers)(IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;

namespace Amp {
	using InterfaceFunc = std::function<void(void)>; /* Datatype we will use to store a void function (with void args) */
	using Interfaces = std::unordered_map<const char*, InterfaceFunc>; /* A map, with key being a integer (idx) and value InterfaceFunc. */
	using Interface = Interfaces::value_type; /* Datatype we will use to insert a new value in our map */

	class DXHook
	{
	public:
		static int getScreenWidth();
		static int getScreenHeight();

		static bool* InterfaceOpened();

		static void HookInit();

		static void PushInterface(const char* name, InterfaceFunc interf);
		static void PopInterfaceByName(const char* name);
		static InterfaceFunc GetInterfaceByName(const char* name);
	};
}