#pragma once
#include "TitleScnce.h"
#include "input/Input.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>
#include <cmath> // ★追加: EaseOutCubicのために必要

using namespace KamataEngine; // ★追加: SelectSceneに合わせて追加

// イージング関数（EaseOutCubic）
float EaseOutCubic(float t); // ★追加: SelectSceneに合わせて宣言を追加

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

	// ★追加: 設定シーンへ移行するフラグ
	bool IsToSetting() const { return isToSetting_; } // ★追加

	// ★追加: 遷移フラグをリセットするメソッド
	void ResetFlags() {
		isFinished_ = false;
		isBackToTitle_ = false;
		isToSetting_ = false; // ★追加: フラグをリセット
		// ★追加: 選択インデックスもリセット
		currentSelectIndex_ = 0;
	}

private:
	/// <summary>
	/// 矢印の移動を開始する
	/// </summary>
	void StartArrowMovement(); // ★追加: SelectSceneに合わせて追加

	/// <summary>
	/// 矢印のアニメーション更新
	/// </summary>
	void UpdateArrowAnimation(); // ★追加: SelectSceneに合わせて追加

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;

	bool isFinished_ = false;
	bool isBackToTitle_ = false; // ★追加: タイトルシーンへ戻る
	bool isToSetting_ = false;   // ★追加: 設定シーンへ移行するフラグ

	// ゲーム説明用のスプライト (ページ0)
	uint32_t textureHandle_ = 0;
	KamataEngine::Sprite* tutorialSprite_ = nullptr;

	// ★追加: 複数ページ用のスプライト (ページ1, 2)
	uint32_t textureHandle_1 = 0;
	KamataEngine::Sprite* tutorialSprite_1 = nullptr;
	uint32_t textureHandle_2 = 0;
	KamataEngine::Sprite* tutorialSprite_2 = nullptr;

	// ★追加: 背景用のテクスチャハンドルとスプライト
	uint32_t backgroundTextureHandle_ = 0;
	KamataEngine::Sprite* backgroundSprite_ = nullptr;

	// ★追加: 暗転用スプライト
	uint32_t fadeTextureHandle_ = 0;
	KamataEngine::Sprite* fadeOutSprite_ = nullptr;

	// ★追加: ロード画面用のテクスチャハンドルとスプライト
	uint32_t loadingTextureHandle_ = 0;
	KamataEngine::Sprite* loadingSprite_ = nullptr;

	// ★追加: 矢印スプライト (SelectSceneから移植)
	uint32_t arrowTextureHandle_ = 0;
	KamataEngine::Sprite* arrowSprite_ = nullptr;

	// ★追加: 矢印の移動 (SelectSceneから移植)
	int currentSelectIndex_ = 0;
	float currentArrowX_ = 0.0f;
	float currentArrowY_ = 0.0f;
	float targetArrowX_ = 0.0f;
	float targetArrowY_ = 0.0f;
	bool isArrowMoving_ = false;

	// ★追加: 矢印の浮遊アニメーション用 (SelectSceneから移植)
	float arrowVerticalOffset_ = 0.0f; // 垂直方向のオフセット
	float animationTimer_ = 0.0f;      // アニメーションタイマー

	// ★追加: シーンの実行状態
	enum class State {
		FadeIn,     // フェードイン演出中
		Active,     // 操作受付中
		Transition, // 次のシーンへの移行演出中 (フェードアウト)
		Finished,   // 移行完了（次のシーンへの移行直前）
	};
	State state_ = State::FadeIn; // 初期状態をFadeInにする

	// ★追加: 演出関連
	static inline const float kFadeInDuration = 60.0f; // フェードイン演出時間（フレーム数、例: 1秒）
	float fadeInTimer_ = 0.0f;                         // フェードインタイマー

	// ★追加: フェードアウト演出関連
	static inline const float kFadeOutDuration = 30.0f; // フェードアウト時間（フレーム数、例: 0.5秒）
	float fadeOutTimer_ = 0.0f;                         // フェードアウトタイマー

	// ★追加: ロード画面の演出時間 (黒画面維持時間)
	static inline const float kLoadingHoldDuration = 120.0f; // 例: 2秒間
	float loadingTimer_ = 0.0f;                              // ロードタイマー
};