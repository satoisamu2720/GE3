#pragma once
#include <Windows.h>
#include <cassert>


#define DIRECTNPUT_VERESON     0x0800
#include <dinput.h>
#include<wrl.h>
#include "WinApp.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
class Input
{
public:
	//namespace
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();

	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumBer);
	
private:
	WinApp* winApp_ = nullptr;
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;
	ComPtr<IDirectInput8>directInput;
	//全キーの入力情報を取得する
	BYTE Key[256] = {};
	BYTE KeyPre[256] = {};
};
