#include "GameOverScene.h"
#include "2d/DebugText.h" // DebugTextの利用を追加
#include <KamataEngine.h>
#include <cstdint>

using namespace KamataEngine;

// 画面サイズ (仮定)
const float kWindowWidth = 1280.0f;
const float kWindowHeight = 720.0f;

GameOverScene::~GameOverScene() {
	// ★修正: deleteする前にnullptrチェックを追加★+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (backgroundSprite_) {
		delete backgroundSprite_;
	}
	if (retrySprite_) {
		delete retrySprite_;
	}
	// delete titleSprite_ は、Initialize()から titleSprite_ の生成を削除したため不要
	// delete titleSprite_; // ★削除: titleSpriteの解放を削除
	if (cursorSprite_) {
		delete cursorSprite_;
	}
}

void GameOverScene::Initialize() {
	input_ = KamataEngine::Input::GetInstance();
	isFinished_ = false;
	isRetrySelected_ = false;
	selectedOption_ = 0; // 初期選択はリトライ (この値は実質使われなくなる)

	// 既存のテクスチャをロード
	// white1x1.pngは真っ白なテクスチャとして、sample.pngはUIのベースとして利用
	whiteTexture_ = KamataEngine::TextureManager::Load("white1x1.png");
	optionTexture_ = KamataEngine::TextureManager::Load("R.png");

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
	// リトライボタンを画面中央付近に配置
	Vector2 retryPos = {kCenterPos.x - kOptionSize.x / 2.0f, kCenterPos.y + 50.0f};
	retrySprite_ = KamataEngine::Sprite::Create(optionTexture_, retryPos);
	// ★修正: 生成失敗時のnullptrチェックを追加★
	if (retrySprite_) {
		retrySprite_->SetSize(kOptionSize);
		retrySprite_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});
	}

	// タイトルへ (選択肢 1)
	// titleSprite_ の生成を削除し、リトライのみに固定
	/*
	Vector2 titlePos = {retryPos.x, retryPos.y + kOptionSize.y + 20.0f};
	titleSprite_ = KamataEngine::Sprite::Create(optionTexture_, titlePos);
	if (titleSprite_) {
	    titleSprite_->SetSize(kOptionSize);
	    titleSprite_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});
	}
	*/

	// 3. 選択カーソル/ハイライトスプライトの生成
	cursorSprite_ = KamataEngine::Sprite::Create(whiteTexture_, {0, 0});
	// ★修正: 生成失敗時のnullptrチェックを追加★
	if (cursorSprite_) {
		cursorSprite_->SetSize({kOptionSize.x + 20.0f, kOptionSize.y + 10.0f});
		cursorSprite_->SetColor({1.0f, 1.0f, 0.0f, 0.5f}); // 黄色で半透明
	}

	// まだ作られていない場合のみ作成する
	if (resultFont_ == nullptr) {
		resultFont_ = new BIt_Map_Font();
	}

	// ★重要: Initialize を呼んでから SetPosition を呼ぶ
	resultFont_->Initialize();

	// 中央に配置する計算
	float centerX = 640.0f - (32.0f * 5 / 2.0f);
	resultFont_->SetPosition({centerX, 300.0f});
}

void GameOverScene::Update() {
	if (isFinished_) {
		return;
	}

	// --- コントローラー情報の取得 ---
	XINPUT_STATE joyState;
	bool hasJoy = input_->GetJoystickState(0, joyState);

	// --- 決定操作 (スペース、エンター、またはコントローラーのAボタン) ---
	bool enterTrigger = input_->TriggerKey(DIK_SPACE) || input_->TriggerKey(DIK_RETURN);
	if (hasJoy && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
		enterTrigger = true;
	}

	if (enterTrigger) {
		isFinished_ = true;
		isRetrySelected_ = true; // 常にリトライを選択したことにする
		return;
	}

	// カーソルの位置を更新 (既存の処理)
	Sprite* targetSprite = retrySprite_;

	if (targetSprite && cursorSprite_) {
		Vector2 targetCenter = targetSprite->GetPosition();
		Vector2 targetSize = targetSprite->GetSize();
		Vector2 cursorSize = cursorSprite_->GetSize();

		Vector2 targetCenterPos = {targetCenter.x + targetSize.x / 2.0f, targetCenter.y + targetSize.y / 2.0f};
		Vector2 cursorDrawPos = {targetCenterPos.x - cursorSize.x / 2.0f, targetCenterPos.y - cursorSize.y / 2.0f};

		cursorSprite_->SetPosition(cursorDrawPos);
	}
}

void GameOverScene::Draw() {
	//DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// --- 2D描画 ---
	Sprite::PreDraw();

	// 1. 背景を描画
	if (backgroundSprite_) {
		backgroundSprite_->Draw();
	}

	// 2. 選択肢を描画 (リトライのみ)
	if (retrySprite_) {
		retrySprite_->Draw();
	}
	// titleSprite_ の描画を削除
	/*
	if (titleSprite_) {
	    titleSprite_->Draw();
	}
	*/

	// 3. カーソルを描画 (ハイライト)
	if (cursorSprite_) {
		cursorSprite_->Draw();
	}
	
	// 1. 背景やリトライボタンの描画 (既存処理)
	backgroundSprite_->Draw();
	retrySprite_->Draw();

	if (resultFont_) {
		// ★重要：ここで保持しているスコアをフォントにセット
		resultFont_->Set(resultScore_);
		resultFont_->Draw();
	}

	Sprite::PostDraw();
}