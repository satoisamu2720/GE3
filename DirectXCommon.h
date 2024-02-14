#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <chrono>

#include "WinApp.h"

class DirectXCommon
{

private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//������
	void Initialize(WinApp* winApp);

	void PreDraw();

	void PosDraw();

	//Getter
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }

	// �X���b�v�`�F�[��
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc; }

	// RTV�f�B�X�N
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc; }

	// SRV�f�B�X�N
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap.Get(); }

private:
	//�f�o�C�X
	void DeviceInitialize();
	//�R�}���h
	void CommandInitialize();
	//�X���b�v�`�F�[��
	void SwapChainInitialize();
	//�����_�[�^�[�Q�b�g
	void RenderTargetInitialize();
	//�[�x�o�b�t�@
	void DepthBufferInitialize();
	//�t�F���X
	void FenceInitialize();

	// �f�B�X�N���v�^�q�[�v�쐬
	ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescripots, bool shaderVisible);

	//FPS�Œ菉��������
	void InitializeFixFPS();
	//FPS�Œ�X�V
	void UpdateFixFPS();
public:
	static const uint32_t kMaxSRVCount;

private:
	WinApp* winApp_ = nullptr;

	ComPtr<ID3D12Device> device;
	ComPtr<IDXGIFactory7> dxgiFactory;

	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandQueue> commandQueue;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	ComPtr<IDXGISwapChain4> swapChain;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;

	ComPtr<ID3D12Resource> depthBuff;
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	ComPtr<ID3D12DescriptorHeap> dsvHeap;

	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;

	D3D12_RESOURCE_BARRIER barrierDesc{};

	// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	// �f�B�X�N���v�^�q�[�v
	// RTV(�Q�[����ʂ�ۑ����Ă���)
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	// SRV(�摜�Ȃǂ�ۑ����Ă�������)
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

	//�L�^�p���Ԍv���̕ϐ�
	std::chrono::steady_clock::time_point reference_;

};