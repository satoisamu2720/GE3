#include "Input.h"

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {
	HRESULT result;
	
	//DirectInputのインスタンス生成
	
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//キーボードデバイス生成
	//ComPtr<IDirectInputDevice8> keyboard;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
	
}
void Input::Update() {
	HRESULT result;
	memcpy(KeyPre, Key, sizeof(Key));
	//キーボード情報の取得開始
	result = keyboard->Acquire();
	//全キーの入力情報を取得する
	result = keyboard->GetDeviceState(sizeof(Key), Key);
	
}
bool Input::PushKey(BYTE keyNumber) {
	if (Key[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE triggerKey)
{
	if (KeyPre[triggerKey]) {
		return true;
	}
	return false;
}
