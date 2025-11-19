#pragma once
#include "input/Input.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <KamataEngine.h>

using namespace KamataEngine;

/// <summary>
/// ゲームクリアシーン
/// </summary>
class GameClearScene {

public:
	~GameClearScene();

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

	uint32_t whiteTexture_ = 0;
	uint32_t optionTexture_ = 0;

	// UI要素
	KamataEngine::Sprite* backgroundSprite_ = nullptr; // 背景
	KamataEngine::Sprite* retrySprite_ = nullptr;      // リトライボタン
	KamataEngine::Sprite* cursorSprite_ = nullptr;     // 選択カーソル/ハイライト

	// ★追加: ゲームクリア表示用のスプライト（オプション）
	KamataEngine::Sprite* gameClearTextSprite_ = nullptr;

	// ★★★ イージング演出のための追加メンバー変数 ★★★
	static const int kFadeInDuration = 60; // 60フレーム (1秒)
	int transitionTimer_ = 0;              // シーン開始からの経過フレーム

	// UIの目標座標
	Vector2 targetTextPos_;
	Vector2 targetRetryPos_;

	// ★★★ 追記: 3Dモデル演出用の変数 ★★★
	Model* playerModel_ = nullptr;       // プレイヤーモデル本体
	WorldTransform playerModelWtLeft_;   // 左側のモデルのワールド変換
	WorldTransform playerModelWtRight_;  // 右側のモデルのワールド変換
	Camera camera_;                      // 演出用のカメラ
	const float kModelScale_ = 3.0f;     // モデルの拡大率
	const float kModelDistance_ = 10.0f; // 画面中心からモデルまでの距離
	float rotationAngle_ = 0.0f;         // モデルのY軸回転角度
};