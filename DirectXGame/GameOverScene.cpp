#include "GameOverScene.h"
#include <KamataEngine.h> // DirectXCommon, TextureManager, Sprite
#include <cstdint>        // uint32_t

using namespace KamataEngine;

// 画面サイズ (仮定)
const float kWindowWidth = 1280.0f;
const float kWindowHeight = 720.0f;

GameOverScene::~GameOverScene() {
	delete backgroundSprite_;
	delete retrySprite_;
	delete titleSprite_;
	delete cursorSprite_;
}

void GameOverScene::Initialize() {
	input_ = KamataEngine::Input::GetInstance();
	isFinished_ = false;
	isRetrySelected_ = false;
	selectedOption_ = 0; // 初期選択はリトライ

	// 既存の sample.png をテクスチャとしてロード (UI用)
	backgroundTexture_ = KamataEngine::TextureManager::Load("sample.png");
	optionBaseTexture_ = backgroundTexture_; // 同じテクスチャを使用

	// 1. 全画面背景スプライトの生成
	backgroundSprite_ = KamataEngine::Sprite::Create(backgroundTexture_, {0, 0});
	backgroundSprite_->SetSize({kWindowWidth, kWindowHeight});
	// 黒に赤みを加えた半透明 (ゲームオーバーの雰囲気)
	backgroundSprite_->SetColor({0.3f, 0.0f, 0.0f, 0.9f});

	// 2. 選択肢スプライトの生成
	const Vector2 kOptionSize = {300.0f, 60.0f}; // 選択肢のサイズ
	// 画面中央付近に配置
	const Vector2 kBasePos = {kWindowWidth / 2.0f - kOptionSize.x / 2.0f, kWindowHeight / 2.0f + 50.0f};

	// リトライ (選択肢 0)
	retrySprite_ = KamataEngine::Sprite::Create(optionBaseTexture_, kBasePos);
	retrySprite_->SetSize(kOptionSize);
	retrySprite_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});

	// タイトルへ (選択肢 1)
	Vector2 titlePos = {kBasePos.x, kBasePos.y + kOptionSize.y + 20.0f};
	titleSprite_ = KamataEngine::Sprite::Create(optionBaseTexture_, titlePos);
	titleSprite_->SetSize(kOptionSize);
	titleSprite_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});

	// 3. 選択カーソル/ハイライトスプライトの生成
	cursorSprite_ = KamataEngine::Sprite::Create(optionBaseTexture_, {0, 0});
	cursorSprite_->SetSize({kOptionSize.x + 20.0f, kOptionSize.y + 10.0f});
	cursorSprite_->SetColor({1.0f, 1.0f, 0.0f, 0.5f}); // 黄色で半透明
}

void GameOverScene::Update() {
	if (isFinished_) {
		return;
	}

	// 上キー/下キーで選択肢を移動
	if (input_->TriggerKey(DIK_W) || input_->TriggerKey(DIK_UP)) {
		selectedOption_ = (selectedOption_ - 1 + 2) % 2;
	}
	if (input_->TriggerKey(DIK_S) || input_->TriggerKey(DIK_DOWN)) {
		selectedOption_ = (selectedOption_ + 1) % 2;
	}

	// 決定キー (スペースキーやエンターキー)
	if (input_->TriggerKey(DIK_SPACE) || input_->TriggerKey(DIK_RETURN)) {
		isFinished_ = true;
		isRetrySelected_ = (selectedOption_ == 0); // 0がリトライ
	}

	// カーソルの位置を更新
	Sprite* targetSprite = (selectedOption_ == 0) ? retrySprite_ : titleSprite_;
	Vector2 cursorPosition = targetSprite->GetPosition();
	Vector2 cursorSize = cursorSprite_->GetSize();
	Vector2 targetSize = targetSprite->GetSize();

	// 選択肢の左上の座標からカーソルの左上の座標を計算し、選択肢をハイライト
	Vector2 cursorDrawPos = {cursorPosition.x - (cursorSize.x - targetSize.x) / 2.0f, cursorPosition.y - (cursorSize.y - targetSize.y) / 2.0f};
	cursorSprite_->SetPosition(cursorDrawPos);
}

void GameOverScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// --- 2D描画 ---
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 1. 背景を描画
	backgroundSprite_->Draw();

	// 2. カーソルを描画
	cursorSprite_->Draw();

	// 3. 選択肢の背景を描画
	retrySprite_->Draw();
	titleSprite_->Draw();

	// ※TODO: ここにゲームオーバーの文字と、選択肢の「リトライ」「タイトルへ」の文字描画が必要です。
	//         BIt_Map_Fontは文字列描画に対応していないため、一旦スプライトのみとします。

	Sprite::PostDraw();
}