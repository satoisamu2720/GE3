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
	//������
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	//�X�V
	void Update();

	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumBer);
	
private:
	//�L�[�{�[�h�̃f�o�C�X
	ComPtr<IDirectInputDevice8> keyboard;
	ComPtr<IDirectInput8>directInput;
	//�S�L�[�̓��͏����擾����
	BYTE Key[256] = {};
	BYTE KeyPre[256] = {};
};
