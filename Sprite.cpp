#include "Sprite.h"
#include "BufferResource.h"

#include <DirectXMath.h>
#include "externals/imgui/imgui.h"
using namespace Microsoft::WRL;
using namespace DirectX;

void Sprite::Initialize(DirectXCommon* dxCommon, SpriteCommon* common)
{

	dxCommon_ = dxCommon;
	common_ = common;
	//Textyre��ǂ񂾓]������
	DirectX::ScratchImage mipImages = common->LoadTexture(L"Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(dxCommon_->GetDevice(), metadata);
	common->UploadTextureData(textureResource, mipImages);

#pragma region ShadeResourceView�𐶐�
	//DepthStencilTexture���E�B���h�E�̃T�C�Y�ō쐬
	//ID3D12Resource* depthStencilResouce = CreateDepthStencilTextureResource(device, kClientWidht, kClientHeight);

	//metaData�����SRV�̐ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	////DSV�p�̃q�[�v�Ńf�B�X�N���v�^�̐���1�BDSV��Shader���ŐG����̂ł͂Ȃ��̂ŁAShederVisible��false
	//ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	////DSV�̐ݒ�
	//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//Format�B��{�I�ɂ�Resource�ɍ��킹��
	//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	//2dTexture
	////DSVHeap�̐擪��DSV�����
	//device->CreateDepthStencilView(depthStencilResouce, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//SRV���쐬����DescriptorHeap�̏ꏊ�����߂�
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(); 
	textureSrvHandleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	//�擪��ImGui���g���Ă���̂ł��̎����g��
	textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRV�̐���
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);
#pragma endregion
	// ���_���
	CreateVertex();
	// �F
	CreateMaterial();
	// �s��
	CreateWVP();
}

void Sprite::Updete()
{
	ImGui::Begin("Texture");
	ImGui::DragFloat3("Pos", &transform.translate.x, 0.1f);
	ImGui::End();
}

void Sprite::Draw()
{

	// Y�����S�ɉ�]
	//transform.rotation.y += 0.03f;
	// ���[���h 
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&transform.scale));
	XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&transform.rotation));
	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&transform.translate));
	// ��]�s��ƃX�P�[���s��̊|���Z
	XMMATRIX rotationAndScaleMatrix = XMMatrixMultiply(rotateMatrix, scaleMatrix);
	// �ŏI�G�͍s��ϊ�
	XMMATRIX worldMatrix = XMMatrixMultiply(rotationAndScaleMatrix, translationMatrix);

	// �J����
	XMMATRIX cameraScaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&cameraTransform.scale));
	XMMATRIX cameraRotateMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&cameraTransform.rotation));
	XMMATRIX cameraTranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&cameraTransform.translate));
	// ��]�s��ƃX�P�[���s��̊|���Z
	XMMATRIX cameraRotateAndScaleMatrix = XMMatrixMultiply(cameraRotateMatrix, cameraScaleMatrix);
	// �ŏI�G�͍s��ϊ�
	XMMATRIX cameraMatrix = XMMatrixMultiply(cameraRotateAndScaleMatrix, cameraTranslationMatrix);

	// view
	XMMATRIX view = XMMatrixInverse(nullptr, cameraMatrix);
	// Proj
	XMMATRIX proj = XMMatrixPerspectiveFovLH
	(XMConvertToRadians(45.0f),
		(float)WinApp::window_width / (float)WinApp::window_height,
		0.1f,
		100.0f
	);

	// WVP
	XMMATRIX worldViewProjectionMatrix = worldMatrix * (view * proj);

	//�s��̑��
	*wvpData = worldViewProjectionMatrix;


	dxCommon_->GetCommandList()->SetGraphicsRootSignature(common_->GetRootSignature());
	dxCommon_->GetCommandList()->SetPipelineState(common_->GetPipelineState());

	// ���_���
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �F���
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	// �s��
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	//�e�N�X�`���摜�\��t��
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);


	dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);


}

void Sprite::CreateVertex()
{
	// VertexResource
	vertexResource = CreateBufferResouce(dxCommon_->GetDevice(), sizeof(VertexData) * 6);

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// ���_���
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	vertexData[0].position = { -1.0f, -1.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };

	vertexData[1].position = {  -1.0f, 1.0f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.0f, 0.0f };

	vertexData[2].position = {  1.0f,-1.0f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f, 1.0f };


	vertexData[3].position = { -1.0f, 1.0f,0.0f,1.0f };
	vertexData[3].texcoord = { 0.0f,0.0f };

	vertexData[4].position = { 1.0f, 1.0f,0.0f,1.0f };
	vertexData[4].texcoord = { 1.0f, 0.0f };

	vertexData[5].position = { 1.0f,-1.0f,0.0f,1.0f };
	vertexData[5].texcoord = { 1.0f, 1.0f };
}

void Sprite::CreateMaterial()
{
	materialResource = CreateBufferResouce(dxCommon_->GetDevice(), sizeof(XMFLOAT4));

	XMFLOAT4* materialData = nullptr;
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	*materialData = color_;

}

void Sprite::CreateWVP()
{
	wvpResource = CreateBufferResouce(dxCommon_->GetDevice(), sizeof(XMMATRIX));

	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	*wvpData = XMMatrixIdentity();

}