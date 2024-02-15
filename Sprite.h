#pragma once
#include "DirectXCommon.h"
#include <d3d12.h>
#include <wrl.h>

#include <DirectXMath.h>

#include "SpriteCommon.h"

class Sprite
{

private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	struct Transform {
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT3 translate;
	};
	struct VertexData {
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT2 texcoord;
	};
	struct MaterialData {
		DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX uvTransform;
	};
public:
	//初期化
	void Initialize(DirectXCommon* dxCommon, SpriteCommon* common,std::wstring textureFilePath);
	void Updete();
	void Draw();
	void AdjustTextureSize();
	
	//Getter/Setter
	DirectX::XMFLOAT2 GetPosition() { return position; }
	float GetRotation() { return rotation; }
	DirectX::XMFLOAT4 GetColor() { return color_; }
	DirectX::XMFLOAT2 GetSize() { return size; }

	void SetPosition(DirectX::XMFLOAT2 pos) { position = pos; }
	void SetRotation(float rot) { rotation = rot; }
	void SetColor(DirectX::XMFLOAT4 color) { color_ = color; }
	void SetSize(DirectX::XMFLOAT2 size) { this->size = size; }

	void SetTexture(std::wstring textureFilePath);

	const DirectX::XMFLOAT2& GetAnchorPoint() const { return anchorPoint; }

	void SetAnchorPoint(const DirectX::XMFLOAT2& anchorPoint) { this->anchorPoint = anchorPoint; }

	bool SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX;}
	bool SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY;}

	bool GetIsFlipX(bool isFlipX) { return isFlipX_;}
	bool GetIsFlipY(bool isFlipY) { return isFlipY_;}

	void SetTextureLeftTop(DirectX::XMFLOAT2 value) { textureLeftTop = value; }
	void SetTextureSize(DirectX::XMFLOAT2 size) { textureSize = size; }

	DirectX::XMFLOAT2 GetTextureLeftTop() { return textureLeftTop; }
	DirectX::XMFLOAT2 GetTextureSize() { return textureSize; }

private:
	//頂点情報作成
	void CreateVertex();
	//	インデックス情報生成
	void CreateIndex();
	// マテリアル情報作成
	void CreateMaterial();
	// 行列情報作成	
	void CreateWVP();

private:
	DirectXCommon* dxCommon_ = nullptr;
	SpriteCommon* common_ = nullptr;

	ComPtr<ID3D12Resource> vertexResource;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	VertexData* vertexData = nullptr;
	//インデックス
	ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	
	// マテリアル情報
	ComPtr<ID3D12Resource> materialResource;
	MaterialData* materialData = nullptr;
	// 行列情報
	ComPtr<ID3D12Resource> wvpResource;
	DirectX::XMMATRIX* wvpData = nullptr;


	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

	// パラメータ
	DirectX::XMFLOAT4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	Transform uvTransform = { {0.5f,0.5f,0.5f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//scale			//Rotate		 //Translate
	Transform transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	DirectX::XMFLOAT2 position = { 0.0f,0.0f };
	float rotation = 0;
	DirectX::XMFLOAT2 size = { 128,128 };

	DirectX::XMFLOAT2 anchorPoint = { 0.0f,0.0f };

	DirectX::XMFLOAT2 textureLeftTop = { 0.0f,0.0f };
	DirectX::XMFLOAT2 textureSize = { 100.0f,100.0f };
	// カメラ
	Transform cameraTransform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };

	uint32_t textureIndex_ = 0;

	bool isFlipX_ = false;

	bool isFlipY_ = false;


};