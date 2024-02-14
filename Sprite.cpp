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
	//Textyreを読んだ転送する
	DirectX::ScratchImage mipImages = common->LoadTexture(L"Resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(dxCommon_->GetDevice(), metadata);
	common->UploadTextureData(textureResource, mipImages);

#pragma region ShadeResourceViewを生成
	//DepthStencilTextureをウィンドウのサイズで作成
	//ID3D12Resource* depthStencilResouce = CreateDepthStencilTextureResource(device, kClientWidht, kClientHeight);

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);


	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(); 
	textureSrvHandleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	//先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVの生成
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);
#pragma endregion
	// 頂点情報
	CreateVertex();
	//インデックス
	CreateIndex();
	// 色
	CreateMaterial();
	// 行列
	CreateWVP();
}

void Sprite::Updete()
{
	transform.translate = { position.x,position.y,0.0f };
	transform.rotation = { 0.0f,0.0f,rotation };
	materialData->color = color_;
	transform.scale = { size.x,size.y,1.0f };

	vertexData[0].position = { -1.0f, -1.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[0].normal = { 0.0f,0.0f,-1.0 };


	vertexData[1].position = { -1.0f, 1.0f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.0f, 0.0f };
	vertexData[1].normal = { 0.0f,0.0f,-1.0 };

	vertexData[2].position = { 1.0f,-1.0f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f, 1.0f };
	vertexData[2].normal = { 0.0f,0.0f,-1.0 };

	vertexData[3].position = { 1.0f, 1.0f,0.0f,1.0f };
	vertexData[3].texcoord = { 1.0f, 0.0f };
	vertexData[3].normal = { 0.0f,0.0f,-1.0 };



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
	
	// Y軸中心に回転
	//transform.rotation.y += 0.03f;
	// ワールド 
	XMMATRIX scaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&transform.scale));
	XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&transform.rotation));
	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&transform.translate));
	// 回転行列とスケール行列の掛け算
	XMMATRIX rotationAndScaleMatrix = XMMatrixMultiply(rotateMatrix, scaleMatrix);
	// 最終敵は行列変換
	XMMATRIX worldMatrix = XMMatrixMultiply(rotationAndScaleMatrix, translationMatrix);

	// カメラ
	XMMATRIX cameraScaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&cameraTransform.scale));
	XMMATRIX cameraRotateMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&cameraTransform.rotation));
	XMMATRIX cameraTranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&cameraTransform.translate));
	// 回転行列とスケール行列の掛け算
	XMMATRIX cameraRotateAndScaleMatrix = XMMatrixMultiply(cameraRotateMatrix, cameraScaleMatrix);
	// 最終敵は行列変換
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

	//行列の代入
	*wvpData = worldViewProjectionMatrix;

	// uvワールド 
	XMMATRIX uvScaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&uvTransform.scale));
	XMMATRIX uvRotateMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&uvTransform.rotation));
	XMMATRIX uvTranslationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&uvTransform.translate));
	// 回転行列とスケール行列の掛け算
	XMMATRIX uvRotationAndScaleMatrix = XMMatrixMultiply(uvRotateMatrix, uvScaleMatrix);
	// 最終敵は行列変換
	XMMATRIX uvWorldMatrix = XMMatrixMultiply(uvRotationAndScaleMatrix, uvTranslationMatrix);
	materialData->uvTransform = uvWorldMatrix;

	dxCommon_->GetCommandList()->SetGraphicsRootSignature(common_->GetRootSignature());
	dxCommon_->GetCommandList()->SetPipelineState(common_->GetPipelineState());

	// 頂点情報
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//インデックス情報
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 色情報
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	// 行列
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	//テクスチャ画像貼り付け
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);


	//dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);


}

void Sprite::CreateVertex()
{
	// VertexResource
	vertexResource = CreateBufferResouce(dxCommon_->GetDevice(), sizeof(VertexData) * 4);

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点情報
	
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
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