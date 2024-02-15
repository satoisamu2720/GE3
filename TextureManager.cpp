#include "TextureManager.h"
#include "BufferResource.h"


TextureManager* TextureManager::instance = nullptr;
uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new TextureManager();
	}
	return instance;
}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;
	textureDatas.reserve(DirectXCommon::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::wstring& filePath)
{
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);
	if (it != textureDatas.end()) {
		return;
	}
	assert(textureDatas.size() + kSRVIndexTop < DirectXCommon::kMaxSRVCount);

	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	HRESULT result = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(result));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	result = DirectX::GenerateMipMaps(
		image.GetImages(), image.GetImageCount(), 
		image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(result));

	textureDatas.resize(textureDatas.size() + 1);
	TextureData& data = textureDatas.back();

	data.filePath = filePath;
	data.metaData = mipImages.GetMetadata();
	data.resource = CreateTextureResource(
		dxCommon_->GetDevice(), data.metaData);
	UploadTextureData(data.resource.Get(), mipImages);

	//画像が保存されているメモリ 
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	handleCPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * srvIndex;
	handleGPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * srvIndex;

	data.sevHandleCPU = handleCPU;
	data.sevHandleGPU = handleGPU;

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = data.metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(data.metaData.mipLevels);

	//SRVの生成
	dxCommon_->GetDevice()->CreateShaderResourceView(data.resource.Get(), &srvDesc, data.sevHandleCPU);

}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::wstring& filePath)
{
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);
	if (it != textureDatas.end()) {
		uint32_t textureindex = static_cast<uint32_t>(std::distance(textureDatas.begin(), it));
		return textureindex;
	}
	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSvtHandleGPU(uint32_t textureIndex)
{
	assert(textureIndex < DirectXCommon::kMaxSRVCount);

	TextureData& data = textureDatas[textureIndex];
	return data.sevHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(uint32_t textureIndex)
{
	assert(textureIndex < DirectXCommon::kMaxSRVCount);

	TextureData& data = textureDatas[textureIndex];

	return data.metaData;
}


void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		//MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}
}
