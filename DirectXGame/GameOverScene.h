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

	uint32_t backgroundTexture_ = 0;
	KamataEngine::Sprite* backgroundSprite_ = nullptr;
	uint32_t optionBaseTexture_ = 0;
	KamataEngine::Sprite* retrySprite_ = nullptr;
	KamataEngine::Sprite* titleSprite_ = nullptr;
	KamataEngine::Sprite* cursorSprite_ = nullptr;
};