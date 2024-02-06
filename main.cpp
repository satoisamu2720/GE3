#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"



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

	SpriteCommon* spriteCommon_ = nullptr;
	//スプライトの共通部分の生成と初期化
	spriteCommon_ = new SpriteCommon;
	spriteCommon_->Initialize(dxCommon_);

	//スプライトの生成と初期化
	Sprite* sprite_ = new Sprite();
	sprite_->Initialize(dxCommon_, spriteCommon_);


	// ゲームループ
	while (true) {
		if (winApp_->Update() == true) {
			break;
		}
		//入力
		input_->Update();

		//更新前処理
		dxCommon_->PreDraw();

		sprite_->Draw();

		//更新後処理
		dxCommon_->PosDraw();
	}

	

	winApp_->Finalize();
	delete winApp_;
     delete input_;
	delete dxCommon_;

	delete spriteCommon_;
	delete sprite_;

	return 0;
}