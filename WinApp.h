#pragma once
//#include "externals/imgui/imgui.h"
#include <wtypes.h>
#include <Windows.h>
#include <cstdint>
class WinApp
{
public://静的メンバ関数
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();

	// ウィンドウサイズ
	static const int32_t kClientWidth = 1280;  // 横幅
	static const int32_t kClientHeight = 720;  // 縦幅
	HWND GetHwnd() const { return hwnd; }
	HINSTANCE GetHInstance() const { return w.hInstance; }
private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;
	WNDCLASSEX w{};
};

