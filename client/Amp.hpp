#pragma once

#include <windows.h>

#define __AMP_ISDEBUG

namespace Amp
{
	constexpr auto time = __TIME__;
	constexpr auto seed = static_cast<int>(time[7]) + static_cast<int>(time[6]) * 10 + static_cast<int>(time[4]) * 60 + static_cast<int>(time[3]) * 600 + static_cast<int>(time[1]) * 3600 + static_cast<int>(time[0]) * 36000;

	enum struct LoadingState {
		LOADING,
		READY
	};

	void Main(HINSTANCE hinstDLL);
	HINSTANCE getHinstDLL();
	HWND getRobloxWindowHwnd();
	HWND getCachedWindowHwnd();
	LoadingState getLoadingState();
};