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
	//初期化
	void Initialize(WinApp* winApp);

	void PreDraw();

	void PosDraw();

	//Getter
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }

	// スワップチェーン
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc; }

	// RTVディスク
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc; }

	// SRVディスク
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap.Get(); }

private:
	//デバイス
	void DeviceInitialize();
	//コマンド
	void CommandInitialize();
	//スワップチェーン
	void SwapChainInitialize();
	//レンダーターゲット
	void RenderTargetInitialize();
	//深度バッファ
	void DepthBufferInitialize();
	//フェンス
	void FenceInitialize();

	// ディスクリプタヒープ作成
	ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescripots, bool shaderVisible);

	//FPS固定初期化処理
	void InitializeFixFPS();
	//FPS固定更新
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

	// レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	// ディスクリプタヒープ
	// RTV(ゲーム画面を保存しておく)
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	// SRV(画像などを保存しておくもの)
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

	//記録用時間計測の変数
	std::chrono::steady_clock::time_point reference_;

};