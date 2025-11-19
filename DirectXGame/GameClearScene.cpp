#include "GameClearScene.h"
#include "math/MathUtility.h"
#include <KamataEngine.h>
#include <cstdint>
#include "EasingFunctions.h"


// 画面サイズ (仮定)
const float kWindowWidth = 1280.0f;
const float kWindowHeight = 720.0f;

GameClearScene::~GameClearScene() {
	if (backgroundSprite_) {
		delete backgroundSprite_;
	}
	if (retrySprite_) {
		delete retrySprite_;
	}
	if (cursorSprite_) {
		delete cursorSprite_;
	}
	// ★追加: ゲームクリアテキストスプライトの解放
	if (gameClearTextSprite_) {
		delete gameClearTextSprite_;
	}
	// ★追加: プレイヤーモデルの解放 ★
	if (playerModel_) {
		delete playerModel_;
	}
}

void GameClearScene::Initialize() {
	input_ = KamataEngine::Input::GetInstance();
	isFinished_ = false;
	isRetrySelected_ = false;

	// ★★★ 追記: 3Dモデルとカメラの初期化 ★★★
	// 既存のPlayer.hに依存してモデルを生成します (例: Playerクラスが"player"モデルを使っている場合)
	playerModel_ = Model::CreateFromOBJ("player");

	// カメラ設定
	camera_.Initialize();
	camera_.translation_ = {0.0f, 5.0f, -20.0f}; // 遠目から見下ろす位置
	camera_.rotation_ = {0.2f, 0.0f, 0.0f};      // 少し傾ける
	camera_.UpdateMatrix();

	// プレイヤーモデルのワールド変換の初期化
	playerModelWtLeft_.Initialize();
	playerModelWtRight_.Initialize();

	// 初期スケール設定
	playerModelWtLeft_.scale_ = {kModelScale_, kModelScale_, kModelScale_};
	playerModelWtRight_.scale_ = {kModelScale_, kModelScale_, kModelScale_};

	// 初期位置設定 (画面中心から左右に離す)
	// 左側: X=-kModelDistance_
	playerModelWtLeft_.translation_ = {-kModelDistance_, 0.0f, 0.0f};
	// 右側: X=+kModelDistance_
	playerModelWtRight_.translation_ = {kModelDistance_, 0.0f, 0.0f};

	playerModelWtLeft_.UpdateMatarix();
	playerModelWtRight_.UpdateMatarix();

	// 回転角度の初期化
	rotationAngle_ = 0.0f;
	// ---------------------------------------------
	// ★★★ 追記: 演出タイマーのリセット ★★★
	transitionTimer_ = 0;

	// 既存のテクスチャをロード
	whiteTexture_ = KamataEngine::TextureManager::Load("white1x1.png");
	optionTexture_ = KamataEngine::TextureManager::Load("R.png");

	// テクスチャ: ゲームクリアの文字画像 (適当な画像名)
	uint32_t gameClearTextTexture = KamataEngine::TextureManager::Load("GameClearText.png");

	// 1. 全画面背景スプライトの生成
	backgroundSprite_ = KamataEngine::Sprite::Create(whiteTexture_, {0, 0});
	if (backgroundSprite_) {
		backgroundSprite_->SetSize({kWindowWidth, kWindowHeight});
		backgroundSprite_->SetColor({0.1f, 0.2f, 0.1f, 0.8f}); // 少し緑がかった暗い半透明
	}

	// 2. 選択肢スプライトの生成 (GameOverSceneと同様にリトライのみ)
	const Vector2 kOptionSize = {300.0f, 60.0f};
	const Vector2 kCenterPos = {kWindowWidth / 2.0f, kWindowHeight / 2.0f};

	// --- リトライボタン ---
	// リトライボタンの目標位置を計算
	targetRetryPos_ = {kCenterPos.x - kOptionSize.x / 2.0f, kCenterPos.y + 50.0f};

	// 初期位置を画面外下部に設定（演出のため）
	Vector2 initialRetryPos = {targetRetryPos_.x, kWindowHeight + kOptionSize.y};

	retrySprite_ = KamataEngine::Sprite::Create(optionTexture_, initialRetryPos);
	if (retrySprite_) {
		retrySprite_->SetSize(kOptionSize);
		retrySprite_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});
	}

	// --- ゲームクリアテキスト ---
	// テキストスプライトの目標位置を計算
	gameClearTextSprite_ = KamataEngine::Sprite::Create(gameClearTextTexture, {0, 0});
	if (gameClearTextSprite_) {
		gameClearTextSprite_->SetSize({500.0f, 100.0f});
		Vector2 center = {kWindowWidth / 2.0f, kWindowHeight / 4.0f};
		Vector2 size = gameClearTextSprite_->GetSize();

		// 目標位置 (画面中央上部)
		targetTextPos_ = {center.x - size.x / 2.0f, center.y - size.y / 2.0f};

		// 初期位置を画面外上部に設定（演出のため）
		Vector2 initialTextPos = {targetTextPos_.x, -size.y};
		gameClearTextSprite_->SetPosition(initialTextPos);
	}

	// 3. 選択カーソル/ハイライトスプライトの生成
	cursorSprite_ = KamataEngine::Sprite::Create(whiteTexture_, {0, 0});
	if (cursorSprite_) {
		cursorSprite_->SetSize({kOptionSize.x + 20.0f, kOptionSize.y + 10.0f});
		cursorSprite_->SetColor({0.0f, 1.0f, 1.0f, 0.5f}); // 水色で半透明
	}
}

void GameClearScene::Update() {
	if (isFinished_) {
		return;
	}

	// 決定キー (スペースキーやエンターキー)
	if (input_->TriggerKey(DIK_SPACE) || input_->TriggerKey(DIK_RETURN)) {
		isFinished_ = true;
		isRetrySelected_ = true; // 常にリトライを選択
		return;
	}

	// ★★★ 追記: 3Dモデルの回転処理 ★★★
	// 毎フレームゆっくり角度を増やしていく
	const float kRotationSpeed = 0.01f; // 回転速度
	rotationAngle_ += kRotationSpeed;

	// 左側のモデル
	playerModelWtLeft_.rotation_.y = rotationAngle_;
	playerModelWtLeft_.UpdateMatarix();

	// 右側のモデル (逆回転させるか、少しずらしても良い)
	playerModelWtRight_.rotation_.y = -rotationAngle_; // 逆回転
	playerModelWtRight_.UpdateMatarix();
	// ----------------------------------------

	// ★★★ イージング演出の実行 ★★★
	if (transitionTimer_ < kFadeInDuration) {
		transitionTimer_++;

		// 0.0f から 1.0f へと変化する割合 (t)
		float t = static_cast<float>(transitionTimer_) / kFadeInDuration;

		// tにイージング関数を適用
		// Math::EaseOutQuart(t) ではなく、新しい関数 Math::easeOutQuint(t) を使う
		float et = MathEsing::easeInOutCirc(t); // ★修正: 新しいイージング関数へ変更 ★

		// 1. テキストスプライトの位置をイージング
		Vector2 initialTextPos = {kWindowWidth / 2.0f, -gameClearTextSprite_->GetSize().y};
		// 線形補間(Lerp)の式: initial * (1.0f - et) + target * et を直接使用
		Vector2 newTextPos = initialTextPos * (1.0f - et) + targetTextPos_ * et;
		gameClearTextSprite_->SetPosition(newTextPos);

		// 2. リトライスプライトの位置をイージング
		Vector2 initialRetryPos = {targetRetryPos_.x, kWindowHeight + retrySprite_->GetSize().y};
		// 線形補間(Lerp)の式: initial * (1.0f - et) + target * et を直接使用
		Vector2 newRetryPos = initialRetryPos * (1.0f - et) + targetRetryPos_ * et;
		retrySprite_->SetPosition(newRetryPos);

	} else {
		// アニメーション完了後、手動でのキー入力を受け付ける
		// 決定キー (スペースキーやエンターキー)
		if (input_->TriggerKey(DIK_SPACE) || input_->TriggerKey(DIK_RETURN)) {
			isFinished_ = true;
			isRetrySelected_ = true; // 常にリトライを選択
			return;
		}
	}
	// ★★★ イージング演出の実行ここまで ★★★

	// カーソルの位置を更新 (リトライに固定)
	// アニメーション完了後、またはアニメーション中に既に目標位置に到達している場合
	Sprite* targetSprite = retrySprite_;

	if (targetSprite && cursorSprite_) {
		// カーソルの位置は、常にリトライボタンの位置に追従させる
		Vector2 targetCenter = targetSprite->GetPosition();
		Vector2 targetSize = targetSprite->GetSize();
		Vector2 cursorSize = cursorSprite_->GetSize();

		Vector2 targetCenterPos = {targetCenter.x + targetSize.x / 2.0f, targetCenter.y + targetSize.y / 2.0f};
		Vector2 cursorDrawPos = {targetCenterPos.x - cursorSize.x / 2.0f, targetCenterPos.y - cursorSize.y / 2.0f};

		cursorSprite_->SetPosition(cursorDrawPos);
	}
}

void GameClearScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 1. 3D描画のセットアップ
	Model::PreDraw();

	// 2. 3Dオブジェクトの描画
	if (playerModel_) {
		// 左側のモデルを描画
		playerModel_->Draw(playerModelWtLeft_, camera_);
		// 右側のモデルを描画
		playerModel_->Draw(playerModelWtRight_, camera_);
	}

	// 3. 3D描画の終了
	Model::PostDraw();

	// --- ここから2D描画 ---

	// 4. 2D描画のセットアップ (コマンドリスト設定)
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 5. 2Dオブジェクトの描画
	// 背景
	if (backgroundSprite_) {
		backgroundSprite_->Draw();
	}
	// ゲームクリアテキスト
	if (gameClearTextSprite_) {
		gameClearTextSprite_->Draw();
	}
	// リトライ
	if (retrySprite_) {
		retrySprite_->Draw();
	}
	// カーソル
	if (cursorSprite_) {
		cursorSprite_->Draw();
	}

	// 6. 2D描画の終了
	Sprite::PostDraw();
}