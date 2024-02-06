#pragma once
//#include "externals/imgui/imgui.h"
#include <wtypes.h>
#include <Windows.h>
#include <cstdint>
class WinApp
{
public://�ÓI�����o�֐�
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public://�����o�֐�
	//������
	void Initialize();
	//�X�V
	void Update();

	// �E�B���h�E�T�C�Y
	static const int32_t kClientWidth = 1280;  // ����
	static const int32_t kClientHeight = 720;  // �c��
	HWND GetHwnd() const { return hwnd; }
	HINSTANCE GetHInstance() const { return w.hInstance; }
private:
	//�E�B���h�E�n���h��
	HWND hwnd = nullptr;
	WNDCLASSEX w{};
};

