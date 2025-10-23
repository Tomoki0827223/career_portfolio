#pragma once
#include "TitleScnce.h"
#include "input/Input.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>
#include <cmath> // std::powfを使うために必要です
#include "math/Vector2.h"

// --- イージング関数のインライン定義 ---
// easeOutBounce (xは0.0fから1.0fの進行度)
inline float easeOutBounce(float x) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1.0f / d1) {
		return n1 * x * x;
	} else if (x < 2.0f / d1) {
		x -= 1.5f / d1;
		return n1 * x * x + 0.75f;
	} else if (x < 2.5f / d1) {
		x -= 2.25f / d1;
		return n1 * x * x + 0.9375f;
	} else {
		x -= 2.625f / d1;
		return n1 * x * x + 0.984375f;
	}
}
// ------------------------------------

/// <summary>
/// ゲーム説明 (チュートリアル) シーン
/// </summary>
class TutorialScene {

public:
	~TutorialScene();

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

	// シーンが終了したか (ゲームシーンへ移行する準備ができたか)
	bool IsFinished() const { return isFinished_; }

	// ★追加: タイトルシーンへ戻るフラグ
	bool IsBackToTitle() const { return isBackToTitle_; }

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;

	bool isFinished_ = false;
	bool isBackToTitle_ = false; // ★追加: タイトルシーンへ戻る

	// ゲーム説明用のスプライト
	uint32_t textureHandle_ = 0;
	KamataEngine::Sprite* tutorialSprite_ = nullptr;

	// ★追加: 背景用のテクスチャハンドルとスプライト
	uint32_t backgroundTextureHandle_ = 0;
	KamataEngine::Sprite* backgroundSprite_ = nullptr;


	// ★追加: 暗転用スプライト
	uint32_t fadeTextureHandle_ = 0;
	KamataEngine::Sprite* fadeOutSprite_ = nullptr; // ★追加: 暗転用スプライト

	// ★追加: ロード画面用のテクスチャハンドルとスプライト
	uint32_t loadingTextureHandle_ = 0;
	KamataEngine::Sprite* loadingSprite_ = nullptr;

	// ★追加: ロード演出用のスプライト（画面を覆う暗幕/スライド用）
	KamataEngine::Sprite* slideSprite_ = nullptr;
	uint32_t slideTextureHandle_ = 0; // スプライト用のテクスチャハンドル

	// ★追加: シーンの実行状態
	enum class State {
		FadeIn,     // フェードイン演出中
		Active,     // 操作受付中
		Transition, // ★修正: 次のシーンへの移行演出中 (フェードアウト + ロード演出)
		Finished,   // ★追加: 移行完了（次のシーンへの移行直前）
	};
	State state_ = State::FadeIn; // 初期状態をFadeInにする

	// ★追加: 演出関連
	static inline const float kFadeInDuration = 60.0f; // フェードイン演出時間（フレーム数、例: 1秒）
	float fadeInTimer_ = 0.0f;                         // フェードインタイマー

	// ★追加: フェードアウト演出関連
	static inline const float kFadeOutDuration = 30.0f; // ★追加: フェードアウト時間（フレーム数、例: 0.5秒）
	float fadeOutTimer_ = 0.0f;                         // ★追加: フェードアウトタイマー

	// ★追加: ロード画面の演出時間 (黒画面維持時間)
	static inline const float kLoadingHoldDuration = 240.0f; // ロード演出（暗転維持）の時間 (例: 1秒)
	// ★追加：ロード画面表示/退場の演出時間
	static inline const float kSlideDuration = 30.0f; // ロード画面が画面外から出てくる/消える時間 (例: 0.5秒)

	float loadingTimer_ = 0.0f; // ロードタイマー

	float timer_ = 0.0f;
};