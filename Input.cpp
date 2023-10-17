#include "Input.h"

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {
	HRESULT result;
	
	//DirectInput�̃C���X�^���X����
	
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	//�L�[�{�[�h�f�o�C�X����
	//ComPtr<IDirectInputDevice8> keyboard;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
	
}
void Input::Update() {
	HRESULT result;
	memcpy(KeyPre, Key, sizeof(Key));
	//�L�[�{�[�h���̎擾�J�n
	result = keyboard->Acquire();
	//�S�L�[�̓��͏����擾����
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
