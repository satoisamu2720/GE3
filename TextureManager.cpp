#include "TextureManager.h"
#include "BufferResource.h"


TextureManager* TextureManager::instance = nullptr;

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
	dxCommon_ = dxCommon_;
	textureDatas.reserve(DirectXCommon::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::wstring& filePath)
{
	//�e�N�X�`���t�@�C����ǂ�Ńv���O�����ň�����悤�ɂ���
	DirectX::ScratchImage image{};
	HRESULT result = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(result));

	//�~�b�v�}�b�v�̍쐬
	DirectX::ScratchImage mipImages{};
	result = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(result));

	textureDatas.resize(textureDatas.size() + 1);
	TextureData& data = textureDatas.back();

	data.filePath = filePath;
	data.metaData = mipImages.GetMetadata();
	data.resource = CreateTextureResource(dxCommon_->GetDevice(), data.metaData);
	UploadTextureData(data.resource.Get(), mipImages);

	//�摜���ۑ�����Ă��郁����
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1);
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	handleCPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * srvIndex;
	handleGPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * srvIndex;

	data.sevHandleCPU = handleCPU;
	data.sevHandleGPU = handleGPU;


}

void TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	//Meta�����擾
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//�SMipMap�ɂ���
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		//MipMapLevel���w�肵�ĊeImage���擾
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Texture�ɓ]��
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
