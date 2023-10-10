#pragma once
#include <Windows.h>
#include <cassert>


#define DIRECTNPUT_VERESON     0x0800
#include <dinput.h>
#include<wrl.h>


#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
class Input
{
public:
	//namespace
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//初期化
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	//更新
	void Update();


	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;
private:
	
};
