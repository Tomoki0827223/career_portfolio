#include "GameOverScene.h"
#include "2d/DebugText.h" // DebugTextの利用を追加
#include <KamataEngine.h>
#include <cstdint>

using namespace KamataEngine;

// 画面サイズ (仮定)
const float kWindowWidth = 1280.0f;
const float kWindowHeight = 720.0f;

GameOverScene::~GameOverScene() {
	// ★修正: deleteする前にnullptrチェックを追加★
	if (backgroundSprite_) {
		delete backgroundSprite_;
	}
	if (retrySprite_) {
		delete retrySprite_;
	}
	if (titleSprite_) {
		delete titleSprite_;
	}
	if (cursorSprite_) {
		delete cursorSprite_;
	}
}


void GameOverScene::Initialize() {
	input_ = KamataEngine::Input::GetInstance();
	isFinished_ = false;
	isRetrySelected_ = false;
	selectedOption_ = 0; // 初期選択はリトライ

	// 既存のテクスチャをロード
	// white1x1.pngは真っ白なテクスチャとして、sample.pngはUIのベースとして利用
	whiteTexture_ = KamataEngine::TextureManager::Load("white1x1.png");
	optionTexture_ = KamataEngine::TextureManager::Load("sample.png");


	// 1. 全画面背景スプライトの生成 (背景一枚の代わり)
	backgroundSprite_ = KamataEngine::Sprite::Create(whiteTexture_, {0, 0});

	// ★修正: 生成失敗時のnullptrチェックを追加★
	if (backgroundSprite_) {
		backgroundSprite_->SetSize({kWindowWidth, kWindowHeight});
		backgroundSprite_->SetColor({0.1f, 0.1f, 0.1f, 0.8f}); // 暗い半透明
	}

	// 2. 選択肢スプライトの生成 (ボタンの代わり)
	const Vector2 kOptionSize = {300.0f, 60.0f};
	const Vector2 kCenterPos = {kWindowWidth / 2.0f, kWindowHeight / 2.0f};

	// リトライ (選択肢 0)
	Vector2 retryPos = {kCenterPos.x - kOptionSize.x / 2.0f, kCenterPos.y + 50.0f};
	retrySprite_ = KamataEngine::Sprite::Create(optionTexture_, retryPos);
	// ★修正: 生成失敗時のnullptrチェックを追加★
	if (retrySprite_) {
		retrySprite_->SetSize(kOptionSize);
		retrySprite_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});
	}

	// タイトルへ (選択肢 1)
	Vector2 titlePos = {retryPos.x, retryPos.y + kOptionSize.y + 20.0f};
	titleSprite_ = KamataEngine::Sprite::Create(optionTexture_, titlePos);
	// ★修正: 生成失敗時のnullptrチェックを追加★
	if (titleSprite_) {
		titleSprite_->SetSize(kOptionSize);
		titleSprite_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});
	}

	// 3. 選択カーソル/ハイライトスプライトの生成
	cursorSprite_ = KamataEngine::Sprite::Create(whiteTexture_, {0, 0});
	// ★修正: 生成失敗時のnullptrチェックを追加★
	if (cursorSprite_) {
		cursorSprite_->SetSize({kOptionSize.x + 20.0f, kOptionSize.y + 10.0f});
		cursorSprite_->SetColor({1.0f, 1.0f, 0.0f, 0.5f}); // 黄色で半透明
	}
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
		return;
	}
	
	// カーソルの位置を更新
	// ★修正: ターゲットとカーソルが有効かチェック★
	Sprite* targetSprite = (selectedOption_ == 0) ? retrySprite_ : titleSprite_;

	if (targetSprite && cursorSprite_) {
		Vector2 targetCenter = targetSprite->GetPosition();
		Vector2 targetSize = targetSprite->GetSize();
		Vector2 cursorSize = cursorSprite_->GetSize();

		// 選択肢の中心にカーソルを配置するよう座標を計算
		Vector2 targetCenterPos = {targetCenter.x + targetSize.x / 2.0f, targetCenter.y + targetSize.y / 2.0f};
		Vector2 cursorDrawPos = {targetCenterPos.x - cursorSize.x / 2.0f, targetCenterPos.y - cursorSize.y / 2.0f};

		cursorSprite_->SetPosition(cursorDrawPos);
	}
}

void GameOverScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// --- 2D描画 ---
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 1. 背景を描画
	if (backgroundSprite_) {
		backgroundSprite_->Draw();
	}

	// 2. 選択肢を描画
	if (retrySprite_) {
		retrySprite_->Draw();
	}
	if (titleSprite_) {
		titleSprite_->Draw();
	}

	// 3. カーソルを描画 (ハイライト)
	if (cursorSprite_) {
		cursorSprite_->Draw();
	}
	
	Sprite::PostDraw();
}