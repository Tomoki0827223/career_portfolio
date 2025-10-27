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


	// テクスチャのロード (ここでは既存のsample.pngを流用)
	backgroundTexture_ = KamataEngine::TextureManager::Load("white1x1.png"); // 真っ白なテクスチャを使用
	optionBaseTexture_ = KamataEngine::TextureManager::Load("sample.png");   // ボタンのベースに利用

	// 1. 全画面背景スプライトの生成 (暗い半透明)
	backgroundSprite_ = KamataEngine::Sprite::Create(backgroundTexture_, {0, 0});
	backgroundSprite_->SetSize({kWindowWidth, kWindowHeight});
	backgroundSprite_->SetColor({0.1f, 0.1f, 0.1f, 0.8f}); // 暗い赤の代わりに暗い半透明に

	// 2. 選択肢スプライトの生成
	const Vector2 kOptionSize = {300.0f, 60.0f};
	const Vector2 kCenterPos = {kWindowWidth / 2.0f, kWindowHeight / 2.0f};

	// リトライ (選択肢 0)
	Vector2 retryPos = {kCenterPos.x - kOptionSize.x / 2.0f, kCenterPos.y + 50.0f};
	retrySprite_ = KamataEngine::Sprite::Create(optionBaseTexture_, retryPos);
	retrySprite_->SetSize(kOptionSize);
	retrySprite_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});

	// タイトルへ (選択肢 1)
	Vector2 titlePos = {retryPos.x, retryPos.y + kOptionSize.y + 20.0f};
	titleSprite_ = KamataEngine::Sprite::Create(optionBaseTexture_, titlePos);
	titleSprite_->SetSize(kOptionSize);
	titleSprite_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});

	// 3. 選択カーソル/ハイライトスプライトの生成
	// カーソルの位置はUpdateで設定
	cursorSprite_ = KamataEngine::Sprite::Create(optionBaseTexture_, {0, 0});
	cursorSprite_->SetSize({kOptionSize.x + 20.0f, kOptionSize.y + 10.0f});
	cursorSprite_->SetColor({1.0f, 1.0f, 0.0f, 0.5f});
}


void GameOverScene::Update() {
	if (isFinished_) {
		return;
	}

	// 選択肢の移動
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
		return;                                    // シーン遷移フラグを立てたら、このフレームの残りの処理はスキップ
	}

	// カーソルの位置を更新
	Sprite* targetSprite = (selectedOption_ == 0) ? retrySprite_ : titleSprite_;
	Vector2 targetCenter = targetSprite->GetPosition(); // Positionは左上座標
	Vector2 targetSize = targetSprite->GetSize();
	Vector2 cursorSize = cursorSprite_->GetSize();

	// 中央座標を計算し、カーソルを中央に配置（ここでは左上に配置されているので微調整が必要）
	// targetCenterは左上座標なので、中心は targetCenter + targetSize/2
	// カーソルの左上は (targetCenter + targetSize/2) - cursorSize/2
	Vector2 targetCenterPos = {targetCenter.x + targetSize.x / 2.0f, targetCenter.y + targetSize.y / 2.0f};
	Vector2 cursorDrawPos = {targetCenterPos.x - cursorSize.x / 2.0f, targetCenterPos.y - cursorSize.y / 2.0f};

	cursorSprite_->SetPosition(cursorDrawPos);
}


void GameOverScene::Draw() {
	// ... (Draw関数内での描画処理)
	// 1. 背景を描画
	backgroundSprite_->Draw();

	// 2. カーソルを描画 (選択肢の上に重ねる)
	cursorSprite_->Draw();

	// 3. 選択肢の背景を描画
	retrySprite_->Draw();
	titleSprite_->Draw();

	// 4. 文字描画（DebugTextで代用）
	DebugText::GetInstance()->Print("GAME OVER", 500, 200, 3.0f); // 赤色
	DebugText::GetInstance()->Print("RETRY", 640 - 100, 360 + 20, 2.0f);
	DebugText::GetInstance()->Print("TITLE", 640 - 100, 440 + 20, 2.0f);
}