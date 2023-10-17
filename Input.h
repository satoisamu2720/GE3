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
public:
	//初期化
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	//更新
	void Update();

	bool PushKey(BYTE keyNumber);

	
private:
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;
	//全キーの入力情報を取得する
	BYTE Key_[256] = {};
};
