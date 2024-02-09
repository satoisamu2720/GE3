#pragma once

#include <Windows.h>
class WinApp
{
public:
	//������
	void Initialize();
	//�X�V
	bool Update();

	void Finalize();

	// �E�B���h�E�v���V�[�W��
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Getter
	HWND GetHwnd() const { return hwnd; }
	HINSTANCE GetHinstance()const { return w.hInstance; }

public:
	// �E�B���h�E�T�C�Y
	static const int window_width = 1280;  // ����
	static const int window_height = 720;  // �c��

private:
	HWND hwnd;
	WNDCLASSEX w{};
	MSG msg{};  // ���b�Z�[�W
};