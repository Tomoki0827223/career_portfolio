#pragma once
#include "input/Input.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <KamataEngine.h>

using namespace KamataEngine;

/// <summary>
/// ゲームオーバーシーン
/// </summary>
class GameOverScene {

public:
	~GameOverScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// リトライ or タイトルへ戻る選択が完了したか
	bool IsFinished() const { return isFinished_; }
	// リトライが選択されたか (true: GameSceneへ, false: TitleSceneへ)
	bool IsRetrySelected() const { return isRetrySelected_; }


private:
	KamataEngine::Input* input_ = nullptr;

	bool isFinished_ = false;      // シーン遷移フラグ
	bool isRetrySelected_ = false; // リトライ選択フラグ
	int selectedOption_ = 0;       // 0: リトライ, 1: タイトルへ

	uint32_t whiteTexture_ = 0;
	uint32_t optionTexture_ = 0;

	// ★ UI要素のスプライトポインタ (不足していたものを追加) ★
	KamataEngine::Sprite* backgroundSprite_ = nullptr;
	KamataEngine::Sprite* gameOverTextSprite_ = nullptr; // ★追加: GAME OVERの文字 ★
	KamataEngine::Sprite* retrySprite_ = nullptr;
	KamataEngine::Sprite* titleSprite_ = nullptr;
	KamataEngine::Sprite* cursorSprite_ = nullptr;

	// ★ モーション制御用のメンバー (前回の修正で追加済みと仮定) ★
	int motionTimer_ = 0;            // モーションの経過フレーム
	const int kMotionDuration_ = 60; // モーションの総フレーム数 (例: 60フレーム = 1秒)

	// 画面サイズを 1280x720 と仮定し、画面外上部からスタートさせる
	const float kInitialOffsetY = -400.0f; // 画面外に隠すためのオフセット量

	// ★ モーションの目標Y座標 (不足していたものを追加) ★
	float targetCenterY_ = 0.0f; // ★追加: GameOver文字の最終Y座標 ★
	float targetRetryY_ = 0.0f;  // ★追加: リトライボタンの最終Y座標 ★
	float targetTitleY_ = 0.0f;  // ★追加: タイトルボタンの最終Y座標 ★

	// ★ easeOutBounce 関数 (インラインで定義) ★
	float easeOutBounce(float x) {
		const float n1 = 7.5625f;
		const float d1 = 2.75f;

		if (x < 1.0f / d1) {
			return n1 * x * x;
		} else if (x < 2.0f / d1) {
			return n1 * (x - 1.5f / d1) * (x - 1.5f / d1) + 0.75f;
		} else if (x < 2.5f / d1) {
			return n1 * (x - 2.25f / d1) * (x - 2.25f / d1) + 0.9375f;
		} else {
			return n1 * (x - 2.625f / d1) * (x - 2.625f / d1) + 0.984375f;
		}
	}
};