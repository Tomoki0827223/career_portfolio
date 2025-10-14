#pragma once
#include "input/Input.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>
#include "TitleScnce.h"

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

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;

	bool isFinished_ = false;

	// ゲーム説明用のスプライト
	uint32_t textureHandle_ = 0;
	KamataEngine::Sprite* tutorialSprite_ = nullptr;

	// ★追加: 背景用のテクスチャハンドルとスプライト
	uint32_t backgroundTextureHandle_ = 0;
	KamataEngine::Sprite* backgroundSprite_ = nullptr;

	// ★追加: シーンの実行状態
	enum class State {
		FadeIn,     // フェードイン演出中
		Active,     // 操作受付中
		Transition, // 次のシーンへの移行演出中 (今回はEnter入力で即終了と想定)
	};
	State state_ = State::FadeIn; // 初期状態をFadeInにする

	// ★追加: 演出関連
	static inline const float kFadeInDuration = 60.0f; // フェードイン演出時間（フレーム数、例: 1秒）
	float fadeInTimer_ = 0.0f;                         // フェードインタイマー

	float timer_ = 0.0f;
};