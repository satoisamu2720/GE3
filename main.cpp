#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include <vector>
#include "ImGuiManager.h"
#include "TextureManager.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Input* input_ = nullptr;
	WinApp* winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	//WindowsAPI初期化処理
	winApp_ = new WinApp;
	winApp_->Initialize();

	//DirectX初期化処理
	dxCommon_ = new DirectXCommon;
	dxCommon_->Initialize(winApp_);

	//Input生成、初期化
	input_ = new Input();
	input_->Initialize(winApp_);

	ImGuiManager* imgui_ = ImGuiManager::Create();
	ImGuiManager::Initialize(winApp_->GetHwnd(), dxCommon_);

	SpriteCommon* spriteCommon_ = nullptr;
	//スプライトの共通部分の生成と初期化
	spriteCommon_ = new SpriteCommon;
	spriteCommon_->Initialize(dxCommon_);

	TextureManager::GetInstance()->Initialize(dxCommon_);
	TextureManager::GetInstance()->LoadTexture(L"Resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture(L"Resources/reimu.png");

	std::vector<Sprite*>sprite_;
	for (uint32_t i = 0; i < 5; i++) {
		//スプライトの生成と初期化
		Sprite* sprites = new Sprite();
		if (i%2 == 0) {
			sprites->Initialize(dxCommon_, spriteCommon_, L"Resources/uvChecker.png");
		}
		else if(i%2 == 1){
			sprites->Initialize(dxCommon_, spriteCommon_, L"Resources/reimu.png");
		}
			sprites->SetPosition({ (float)i * 256.0f,0.0f });
			sprite_.push_back(sprites);
	}


	// ゲームループ
	while (true) {
		if (winApp_->Update() == true) {
			break;
		}

		ImGuiManager::NewFrame();
		imgui_->ShowDemo();

		//入力
		input_->Update();

		//for (uint32_t i = 0; i < 5; i++) {
		//	//移動
		//	DirectX::XMFLOAT2 pos = sprite_[i]->GetPosition();
		//	//pos.x += 0.01;
		//	sprite_[i]->SetPosition(pos);
		//	//回転
		//	float rot = sprite_[i]->GetRotation();
		//	//rot += 0.005f;
		//	sprite_[i]->SetRotation(rot);
		//	//色
		//	DirectX::XMFLOAT4 color = sprite_[i]->GetColor();
		//	//color.x -= 0.01f;
		//	if (color.x < 0.0f) {
		//		color.x = 1.0f;
		//	}
		//	sprite_[i]->SetColor(color);
		//	//大きさ
		//	DirectX::XMFLOAT2 size = sprite_[i]->GetSize();
		//	/*size.x += 0.01;
		//	size.y += 0.01;*/
		//	sprite_[i]->SetSize(size);
		//}
			for (uint32_t i = 0; i < 5; i++) {
				sprite_[i]->Updete();
			}
			////更新前処理
			ImGuiManager::CreateCommand();
			dxCommon_->PreDraw();
			spriteCommon_->SpritePreDraw();
			for (uint32_t i = 0; i < 5; i++) {
				sprite_[i]->Draw();
			}
			//更新後処理
			ImGuiManager::CommandsExcute(dxCommon_->GetCommandList());
			dxCommon_->PosDraw();
		
		TextureManager::GetInstance()->Finalize();

		delete input_;
		delete dxCommon_;

		delete spriteCommon_;

		winApp_->Finalize();
		delete winApp_;

		for (uint32_t i = 0; i < 5; i++) {
			delete sprite_[i];
		}
		//delete imgui_;

		return 0;
	}
}