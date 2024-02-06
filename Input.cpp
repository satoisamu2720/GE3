#include "Input.h"

void Input::Initialize(WinApp* winApp) {
	winApp_ = winApp;
	HRESULT result;

	result = DirectInput8Create(
		winApp->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));


	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	// ���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // �W���`��
	assert(SUCCEEDED(result));
	// �r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
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
