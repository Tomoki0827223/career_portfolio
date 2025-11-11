#pragma once
#include "BIt_Map_Font.h"
#include "Book.h" // ★追加
#include "Boomerang.h"
#include "Bullet.h" // ★追加
#include "Enemy.h"  // 追記
#include "Enemy2.h"
#include "Experience.h"
#include "GameOverScene.h"
#include "Graph.h"
#include "KamataEngine.h"
#include "Minion.h"
#include "Missile.h"
#include "Player.h"
#include "Stage.h"
#include "Wine.h"
// ★★★ GameLogicの定義をインクルード ★★★
#include "GameLogic.h"
#include "math/MathUtility.h"
#include <algorithm>
#include <random>
#include <vector>


using namespace KamataEngine;

// ★ レベルアップ後のスキル選択肢の定義はGameLogic.hへ移動 ★
// enum class SkillType { ... }; // 削除

class GameScene {
public:
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

	bool IsGameOver() const { return isGameOver_; }
	// ★追加: ゲームオーバーフラグをリセットするためのpublicメソッド
	void ResetGameOverFlag() { isGameOver_ = false; }

private:
	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* sprite_ = nullptr;

	// 3Dモデルデータ
	Model* model_ = nullptr;

	Camera camera_;

	WorldTransform worldTransform;

	Stage* stage_ = nullptr;

	Player* player_ = nullptr;

	Model* playerModel_ = nullptr;

	// Graph* graph_ = nullptr;

	BIt_Map_Font* font_ = nullptr;

	// ★★★ GameLogic クラスのインスタンスを追加 ★★★
	GameLogic* gameLogic_ = nullptr;

	// 経験値アイテムを格納するベクトル (GameLogicへ移動)
	// const float ATTRACTION_RADIUS = 10.0f; // GameLogicへ移動

	// 敵の管理、スキル関連のオブジェクト管理、タイマー、レベルアップ関連はすべてGameLogicへ移動

	// HPバー用のスプライト (GameSceneに残す)
	KamataEngine::Sprite* hpBarBase_ = nullptr;
	KamataEngine::Sprite* hpBar_ = nullptr;
	uint32_t hpBarBaseTexture_ = 0;
	uint32_t hpBarTexture_ = 0;

	uint32_t sousaTextureHandle_ = 0;
	uint32_t sousaTextureHandle2_ = 0;
	KamataEngine::Sprite* sousaSprite_ = nullptr;
	KamataEngine::Sprite* sousaSprite2_ = nullptr;

	// ゲームオーバーフラグ (GameSceneに残す - シーン遷移制御のため)
	bool isGameOver_ = false;

	// スキル選択画面用スプライト (GameSceneに残す - 描画用UI要素)
	KamataEngine::Sprite* skillScreenBackground_ = nullptr; // 半透明の背景
	KamataEngine::Sprite* skillOptionSprites_[3] = {};      // 3つの選択肢の背景
	KamataEngine::Sprite* skillCursorSprite_ = nullptr;     // 選択カーソル

	uint32_t whiteTextureHandle_ = 0; // スプライトの色付けに使う1x1の白テクスチャ
	// ----------------------------------------

	// 衝突判定関数 (GameLogicへ移動したため削除)
	// void CheckAllCollisions();

	// HPバー描画関数 (GameSceneに残す - UI描画)
	void DrawHPBar();

	// 敵のランダム生成関数、Wineのランダム生成関数 (GameLogicへ移動したため削除)
	// スキル関連関数 (GameLogicへ移動したため削除)
};