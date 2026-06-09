#pragma once
#include "BaseScene.h"
#include "TitleScnce.h"
#include "input/Input.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>

/// <summary>
/// ゲーム説明 (チュートリアル) シーン
/// </summary>
class TutorialScene : public BaseScene {

public:
	~TutorialScene() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	// シーンが終了したか (ゲームシーンへ移行する準備ができたか)
	bool IsFinished() const { return isFinished_; }

	// ★追加: タイトルシーンへ戻るフラグ
	bool IsBackToTitle() const { return isBackToTitle_; }

	// ★追加: 遷移フラグをリセットするメソッド
	void ResetFlags() {
		isFinished_ = false;
		isBackToTitle_ = false;
	}

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

	// ★追加: シーンの実行状態
	enum class State {
		FadeIn,     // フェードイン演出中
		Active,     // 操作受付中
		Transition, // ★修正: 次のシーンへの移行演出中 (フェードアウト)
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
	static inline const float kLoadingHoldDuration = 120.0f; // 例: 1秒間を2秒間 (120フレーム) に変更
	float loadingTimer_ = 0.0f;                              // ロードタイマー

	float timer_ = 0.0f;
};
// 最後にあった余分な '}' は削除しました