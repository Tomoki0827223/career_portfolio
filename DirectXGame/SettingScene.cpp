#include "SettingScene.h"

SettingScene::~SettingScene() { delete backgroundSprite_; }

void SettingScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();

	// タイトルに戻るフラグをリセット
	isBackToTitle_ = false;

	// 例: 背景テクスチャのロード (適切な画像パスに修正してください)
	backgroundTextureHandle_ = KamataEngine::TextureManager::Load("Setting/SettingBackground.png");
	backgroundSprite_ = KamataEngine::Sprite::Create(backgroundTextureHandle_, {0, 0});
	backgroundSprite_->SetPosition({0, 0});
}

void SettingScene::Update() {
	// 例: Escキーが押されたらタイトルに戻る
	if (input_->TriggerKey(DIK_ESCAPE)) {
		isBackToTitle_ = true;
	}

	// TODO: ここに音量調整などの設定項目に対応するロジックを記述
}

void SettingScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 2D描画前準備
	KamataEngine::Sprite::PreDraw(commandList);

	// 背景スプライトを描画
	if (backgroundSprite_) {
		backgroundSprite_->Draw();
	}

	// 2D描画終了
	KamataEngine::Sprite::PostDraw();
}