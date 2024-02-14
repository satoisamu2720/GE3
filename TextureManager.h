#pragma once
#include "DirectXCommon.h"
#include<string>
#include <DirectXTex.h>

class TextureManager
{
private:
	struct TextureData
	{
		std::wstring filePath;
		DirectX::TexMetadata metaData;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE sevHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE sevHandleGPU;
	};
public:
	static TextureManager* GetInstance();
	void Finalize();
	void Initialize(DirectXCommon* dxCommon);

	// �摜�ǂݍ���
    void  LoadTexture(const std::wstring& filePath);
	uint32_t GetTextureIndexByFilePath(const std::wstring& filePath);
	//GPU�n���h���擾
	D3D12_GPU_DESCRIPTOR_HANDLE GetSvtHandleGPU(uint32_t textureIndex);

	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

private:
	// �ǂݍ��񂾉摜GPU(�V�F�[�_�ɑ���)
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

private:
	static TextureManager* instance;
	static uint32_t kSRVIndexTop;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator = (TextureManager&) = delete;
private:
	DirectXCommon* dxCommon_ = nullptr;
	std::vector<TextureData> textureDatas;
};
