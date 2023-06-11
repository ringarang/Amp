#include "DXHook.hpp"
#include "Windows.h"
#include <fstream>
#include <mutex>


#include "VMTHook.hpp"
#include "Amp.hpp"
#include "ConsoleHandler.hpp"

VMTHook* VTHook;
int s_w, s_h;
bool renderInterface = true;

ImFont* IFont;
HWND window;
RECT rc;
DXGI_SWAP_CHAIN_DESC sd;
ID3D11RenderTargetView* mainRenderTargetView;
WNDPROC oWndProc;

HWND GlobalHWND = nullptr;
ID3D11Device* Globald3Device = nullptr;
ID3D11DeviceContext* Globald3Context = nullptr;
IDXGISwapChain* GlobalSwapChain = nullptr;
std::once_flag IsInit;

ID3D11Texture2D* pBackBuffer = nullptr;
D3D11PresentHook phookD3D11Present = nullptr;
tResizeBuffers oResizeBuffers = nullptr;

unsigned long* pSwapChainVTable = nullptr;

void InitImGui(ID3D11Device* device, ID3D11DeviceContext* ctx)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	IFont = io.Fonts->AddFontDefault();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = NULL;	
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, ctx);
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT Msg, WPARAM w_param, LPARAM l_param) {
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_WndProcHandler(hWnd, Msg, w_param, l_param);

	if (renderInterface) ImGui::GetIO().MouseDrawCursor = true;
	else ImGui::GetIO().MouseDrawCursor = false;

	switch (Msg)
	{
	case WH_CBT:
		window = sd.OutputWindow;
		GlobalHWND = hWnd;
		break;
	case WM_KEYDOWN:
		if (w_param == VK_INSERT) renderInterface = !renderInterface;
		break;
	case 522:
	case 513:
	case 533:
	case 514:
	case 134:
	case 516:
	case 517:
	case 258:
	case 257:
	case 512:
	case 132:
	case 127:
	case 32:
	case 255:
    case 523:
	case 524:
	case 793:
		if (renderInterface) return TRUE; //block basically all messages we don't want roblox to receive
		break;
	}	
	return CallWindowProc(oWndProc, hWnd, Msg, w_param, l_param);
}


Amp::Interfaces interfs;


HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	std::call_once(IsInit, [&]()
		{
			pSwapChain->GetDesc(&sd);
			pSwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&Globald3Device));
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
			window = sd.OutputWindow;
			oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));
			ImGui::CreateContext();
			Globald3Device->GetImmediateContext(&Globald3Context);
			Globald3Device->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView); pBackBuffer->Release();
			InitImGui(Globald3Device, Globald3Context);		
		});

	if (mainRenderTargetView == nullptr)
	{
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
		Globald3Device->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
		pBackBuffer->Release();
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	ImDrawList* bgDrawList = ImGui::GetBackgroundDrawList();
	ImDrawList* ovDrawList = ImGui::GetOverlayDrawList();

	if (Amp::getLoadingState() == Amp::LoadingState::LOADING)
		ovDrawList->AddText(ImVec2(20, s_h-70), ImColor(1.0f, 1.0f, 0.0f), "amp is loading...");
	else if (Amp::getLoadingState() == Amp::LoadingState::READY)
		ovDrawList->AddText(ImVec2(20, s_h-70), ImColor(0.0f, 1.0f, 0.0f), "amp is loaded!");

	if (renderInterface) {
		/* Darken the scene */
		bgDrawList->AddRectFilled(ImVec2(0, 0), ImVec2(s_w, s_h), ImColor(0.0f, 0.0f, 0.0f, 0.5f));

		/* Run our interfaces */
		Amp::Interfaces::iterator it = interfs.begin();
		while (it != interfs.end()) it++->second();
	}

	ImGui::Render();
	Globald3Context->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return phookD3D11Present(pSwapChain, SyncInterval, Flags);
}

HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	if (mainRenderTargetView)
	{
		mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
	}
	window = sd.OutputWindow;
	GetWindowRect(GlobalHWND, &rc);
	s_w = rc.right - rc.left;
	s_h = rc.bottom - rc.top;
	return oResizeBuffers(pThis, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

void Start()
{
	D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D_FEATURE_LEVEL obtainedLevel;
	DXGI_SWAP_CHAIN_DESC sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.OutputWindow = GlobalHWND;
		sd.SampleDesc.Count = 1;
		sd.Windowed = ((GetWindowLongPtrA(GlobalHWND, GWL_STYLE) & WS_POPUP) != 0) ? false : true;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.BufferDesc.Width = 1;
		sd.BufferDesc.Height = 1;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, levels, sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &sd, &GlobalSwapChain, &Globald3Device, &obtainedLevel, &Globald3Context);

    unsigned int* VTSwapChain;
	memcpy(&VTSwapChain, reinterpret_cast<LPVOID>(GlobalSwapChain), sizeof(uintptr_t));
	phookD3D11Present = reinterpret_cast<D3D11PresentHook>(VTHook->ReplaceVtablePointer(VTSwapChain, 8, reinterpret_cast<unsigned int>(&PresentHook)));
	oResizeBuffers = reinterpret_cast<tResizeBuffers>(VTHook->ReplaceVtablePointer(VTSwapChain, 13, reinterpret_cast<unsigned int>(&hkResizeBuffers)));
}

int Amp::DXHook::getScreenWidth()
{
	return s_w;
}

int Amp::DXHook::getScreenHeight()
{
	return s_h;
}

bool* Amp::DXHook::InterfaceOpened()
{
	return &renderInterface;
}

void Amp::DXHook::HookInit()
{
	SetForegroundWindow(Amp::getRobloxWindowHwnd()); //Refocuses window
	GlobalHWND = Amp::getCachedWindowHwnd();
	GetWindowRect(Amp::getRobloxWindowHwnd(), &rc);
	s_w = rc.right - rc.left;
	s_h = rc.bottom - rc.top;
	Start();
}

void Amp::DXHook::PushInterface(const char* name, Amp::InterfaceFunc interf)
{
	interfs.insert(Interface(name, interf));
}

void Amp::DXHook::PopInterfaceByName(const char* name)
{
	interfs.erase(name);
}

Amp::InterfaceFunc Amp::DXHook::GetInterfaceByName(const char* name)
{
	return interfs[name];
}
