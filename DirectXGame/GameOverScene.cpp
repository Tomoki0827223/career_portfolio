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
	whiteTexture_ = KamataEngine::TextureManager::Load("gameOver.png");
	optionTexture_ = KamataEngine::TextureManager::Load("sample.png");


	// 1. 全画面背景スプライトの生成 (背景一枚の代わり)
	backgroundSprite_ = KamataEngine::Sprite::Create(whiteTexture_, {0, 0});

	// ★修正: 生成失敗時のnullptrチェックを追加★
	if (backgroundSprite_) {
		backgroundSprite_->SetSize({kWindowWidth, kWindowHeight});
		backgroundSprite_->SetColor({0.1f, 0.1f, 0.1f, 0.8f}); // 暗い半透明
	}

	// ★追加: GAME OVER テキストスプライトの生成 (適当なサイズとテクスチャで仮置き) ★
	uint32_t gameOverTexture = KamataEngine::TextureManager::Load("gameOver.png"); // ※テクスチャ名は適宜修正してください
	gameOverTextSprite_ = KamataEngine::Sprite::Create(gameOverTexture, {0, 0});
	gameOverTextSprite_->SetSize({600.0f, 100.0f}); // サイズも適宜修正してください

	// 2. 選択肢スプライトの生成 (ボタンの代わり)
	const Vector2 kOptionSize = {300.0f, 60.0f};
	const Vector2 kCenterPos = {kWindowWidth / 2.0f, kWindowHeight / 2.0f};

	// ★ モーション制御の初期化と初期位置の設定 (前回の修正コード) ★
	motionTimer_ = 0; // タイマーをリセット

	// ★修正: 目標Y座標の計算と格納 ★
	targetCenterY_ = kWindowHeight / 2.0f;         // 画面中央Y座標
	targetRetryY_ = kWindowHeight / 2.0f + 100.0f; // リトライの目標Y座標 (中央から下に100)
	targetTitleY_ = kWindowHeight / 2.0f + 220.0f; // タイトルの目標Y座標 (中央から下に220)

	// 初期位置を設定 (画面外)
	if (gameOverTextSprite_) {
		// X座標は中心に配置し、Y座標は画面外上部から開始
		float centerX = (kWindowWidth - gameOverTextSprite_->GetSize().x) / 2.0f;
		gameOverTextSprite_->SetPosition({centerX, targetCenterY_ + kInitialOffsetY});
	}

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

	// ★ モーション制御の初期化 (追加) ★
	motionTimer_ = 0; // タイマーをリセット

	// UIの初期位置を画面外の上部に設定

	// 各スプライトの目標Y座標を覚えておく
	targetCenterY_ = kWindowHeight / 2.0f;         // 画面中央Y座標 360.0f
	targetRetryY_ = kWindowHeight / 2.0f + 100.0f; // リトライの目標Y座標
	targetTitleY_ = kWindowHeight / 2.0f + 220.0f; // タイトルの目標Y座標

	// 初期位置を設定 (画面外)
	if (gameOverTextSprite_) {
		gameOverTextSprite_->SetPosition({(kWindowWidth - gameOverTextSprite_->GetSize().x) / 2.0f, targetCenterY_ + kInitialOffsetY});
	}
	if (retrySprite_) {
		retrySprite_->SetPosition({kWindowWidth / 2.0f - retrySprite_->GetSize().x / 2.0f, targetRetryY_ + kInitialOffsetY});
	}
	if (titleSprite_) {
		titleSprite_->SetPosition({kWindowWidth / 2.0f - titleSprite_->GetSize().x / 2.0f, targetTitleY_ + kInitialOffsetY});
	}
}

void GameOverScene::Update() {
	// ------------------------------------
	// ★ モーションの進行 (追加) ★
	// ------------------------------------
	if (motionTimer_ < kMotionDuration_) {
		motionTimer_++;
	}

	// アニメーションの進行度 (0.0f -> 1.0f)
	float t = static_cast<float>(motionTimer_) / kMotionDuration_;
	if (t > 1.0f)
		t = 1.0f; // 1.0fを超えないようにクランプ

	// easeOutBounceを適用
	float easeT = easeOutBounce(t);

	float currentOffsetY = kInitialOffsetY * (1.0f - easeT);

	// 各スプライトの位置を更新
	if (gameOverTextSprite_) {
		Vector2 pos = gameOverTextSprite_->GetPosition();
		gameOverTextSprite_->SetPosition({pos.x, targetCenterY_ + currentOffsetY});
	}
	if (retrySprite_) {
		Vector2 pos = retrySprite_->GetPosition();
		retrySprite_->SetPosition({pos.x, targetRetryY_ + currentOffsetY});
	}
	if (titleSprite_) {
		Vector2 pos = titleSprite_->GetPosition();
		titleSprite_->SetPosition({pos.x, targetTitleY_ + currentOffsetY});
	}
	// ------------------------------------

	// ★ モーション中は選択肢の入力を受け付けないようにする ★
	if (motionTimer_ < kMotionDuration_) {
		return;
	}


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