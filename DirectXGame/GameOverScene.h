#pragma once
#include "BaseScene.h"
#include "input/Input.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <KamataEngine.h>
#include "BIt_Map_Font.h"

using namespace KamataEngine;

/// <summary>
/// ゲームオーバーシーン
/// </summary>
class GameOverScene : public BaseScene {

public:
	~GameOverScene() override;

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

	// リトライ or タイトルへ戻る選択が完了したか
	bool IsFinished() const { return isFinished_; }
	// リトライが選択されたか (true: GameSceneへ, false: TitleSceneへ)
	bool IsRetrySelected() const { return isRetrySelected_; }
	// ★追加: スコアをセットする関数
	void SetResultScore(int score) { resultScore_ = score; }

private:
	KamataEngine::Input* input_ = nullptr;

	bool isFinished_ = false;      // シーン遷移フラグ
	bool isRetrySelected_ = false; // リトライ選択フラグ
	int selectedOption_ = 0;       // 0: リトライ, 1: タイトルへ (上下キーの処理を削除したため、この値は常に0のままになります)
	int resultScore_ = 0;          // ★追加: 表示するスコア
	BIt_Map_Font* resultFont_ = nullptr;

	uint32_t whiteTexture_ = 0;
	uint32_t optionTexture_ = 0;

	// ★ UI要素を分離したスプライト ★
	KamataEngine::Sprite* backgroundSprite_ = nullptr; // 背景一枚
	KamataEngine::Sprite* retrySprite_ = nullptr;      // リトライボタン
	// KamataEngine::Sprite* titleSprite_ = nullptr;      // ★削除: タイトルへボタンの宣言を削除
	KamataEngine::Sprite* cursorSprite_ = nullptr; // 選択カーソル/ハイライト
};