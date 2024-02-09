#pragma once
#include "DirectXCommon.h"
class ImGuiManager
{
public:
	// ������
	static void Initialize(HWND hwnd, DirectXCommon* dxCommon);

	// �X�V����
	static void NewFrame();

	// �X�V�I��
	static void CreateCommand();

	// �R�}���h��ς�
	static void CommandsExcute(ID3D12GraphicsCommandList* commandList);

	// new����
	static ImGuiManager* Create();

public:
	// �f�X�g���N�^
	~ImGuiManager();

public:
	void ShowDemo();


};