#pragma once
#include <Windows.h>
#include <wrl.h>
#include <string>
#include <dxcapi.h>
#include <d3d12.h>

#include <DirectXTex.h>

#include "DirectXCommon.h"

class SpriteCommon
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public:
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	void SpritePreDraw();

	//Getter
	ID3D12RootSignature* GetRootSignature() { return rootSignature.Get(); }
	ID3D12PipelineState* GetPipelineState() { return pipelineState.Get(); }

	
	
private:

	IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* dxcIncludeHandler
	);

private:
	DirectXCommon* dxCommon_ = nullptr;

	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState{};

};