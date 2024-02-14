#include "Sprite.h"
#include "BufferResource.h"

#include <DirectXMath.h>
#include "externals/imgui/imgui.h"
#include "TextureManager.h"
using namespace Microsoft::WRL;
using namespace DirectX;

void Sprite::Initialize(DirectXCommon* dxCommon, SpriteCommon* common, std::wstring textureFilePath)
{
	dxCommon_ = dxCommon;
	common_ = common;
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);
	
	// ���_���
	CreateVertex();
	//�C���f�b�N�X
	CreateIndex();
	// �F
	CreateMaterial();
	// �s��
	CreateWVP();

}

void Sprite::Updete()
{
	transform.translate = { position.x,position.y,0.0f };
	transform.rotation = { 0.0f,0.0f,rotation };
	materialData->color = color_;
	transform.scale = { size.x,size.y,1.0f };

	vertexData[0].position = { 0.0f, 1.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//vertexData[0].normal = { 0.0f,0.0f,-1.0 };


	vertexData[1].position = { 0.0f, 0.0f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.0f, 0.0f };
	//vertexData[1].normal = { 0.0f,0.0f,-1.0 };

	vertexData[2].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f, 1.0f };
	//vertexData[2].normal = { 0.0f,0.0f,-1.0 };

	vertexData[3].position = { 1.0f, 0.0f,0.0f,1.0f };
	vertexData[3].texcoord = { 1.0f, 0.0f };
	//vertexData[3].normal = { 0.0f,0.0f,-1.0 };



	ImGui::Begin("Texture");
	ImGui::DragFloat3("Pos", &position.x, 0.1f);
	ImGui::DragFloat3("UV-Pos", &uvTransform.translate.x, 0.01f,-10.0f,10.0f);
	ImGui::SliderAngle("UV-Rot", &uvTransform.rotation.z);
	ImGui::DragFloat3("UV-Scale", &uvTransform.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat3("Size", &size.x, 0.01f, -10.0f, 10.0f);
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
	/*XMMATRIX proj = XMMatrixPerspectiveFovLH
	(XMConvertToRadians(45.0f),
		(float)WinApp::window_width / (float)WinApp::window_height,
		0.1f,
		100.0f
	);*/

	XMMATRIX proj = XMMatrixOrthographicOffCenterLH(0,
		WinApp::window_width,
		WinApp::window_height,
		0, 0.1f, 100.0f);

	// WVP
	XMMATRIX worldViewProjectionMatrix = worldMatrix * (view * proj);

	//�s��̑��
	*wvpData = worldViewProjectionMatrix;

	XMMATRIX uvScaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&uvTransform.scale));
	XMMATRIX uvRotateMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&uvTransform.rotation));
	XMMATRIX uvTranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&uvTransform.translate));

	XMMATRIX uvRotationAndScaleMatrix = XMMatrixMultiply(uvRotateMatrix, uvScaleMatrix);

	XMMATRIX uvWorldMatrix = XMMatrixMultiply(uvRotationAndScaleMatrix, uvTranslationMatrix);
	materialData->uvTransform = uvWorldMatrix;


	dxCommon_->GetCommandList()->SetGraphicsRootSignature(common_->GetRootSignature());
	dxCommon_->GetCommandList()->SetPipelineState(common_->GetPipelineState());

	// ���_���
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �F���
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	// �s��
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSvtHandleGPU(textureIndex_));

	//dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);

	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Sprite::SetTexture(std::wstring textureFilePath)
{
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);
}

void Sprite::CreateVertex()
{
	// VertexResource
	vertexResource = CreateBufferResouce(dxCommon_->GetDevice(), sizeof(VertexData) * 4);

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// ���_���
	
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	vertexData[0].position = { 0.0f, 1.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//vertexData[0].normal = { 0.0f,0.0f,-1.0 };


	vertexData[1].position = { 0.0f, 0.0f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.0f, 0.0f };
	//vertexData[1].normal = { 0.0f,0.0f,-1.0 };

	vertexData[2].position = { 1.0f,1.0f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f, 1.0f };
	//vertexData[2].normal = { 0.0f,0.0f,-1.0 };

	vertexData[3].position = { 1.0f, 0.0f,0.0f,1.0f };
	vertexData[3].texcoord = { 1.0f, 0.0f };
	//vertexData[3].normal = { 0.0f,0.0f,-1.0 };
}

void Sprite::CreateIndex()
{
	indexResource = CreateBufferResouce(dxCommon_->GetDevice(), sizeof(uint32_t) * 6);
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;

}

void Sprite::CreateMaterial()
{
	materialResource = CreateBufferResouce(dxCommon_->GetDevice(), sizeof(MaterialData));

	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	materialData->color = color_;
	materialData->uvTransform = XMMatrixIdentity();

}

void Sprite::CreateWVP()
{
	wvpResource = CreateBufferResouce(dxCommon_->GetDevice(), sizeof(XMMATRIX));

	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	*wvpData = XMMatrixIdentity();

}