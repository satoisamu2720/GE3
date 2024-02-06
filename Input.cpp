#include "Input.h"

void Input::Initialize(WinApp* winApp) {
	winApp_ = winApp;
	HRESULT result;

	result = DirectInput8Create(
		winApp->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));


	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));
	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
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
