#pragma once
#include "BIt_Map_Font.h"
#include "Graph.h"
#include "KamataEngine.h"
#include "Player.h"
#include "Stage.h"
#include "Experience.h" 
#include "Enemy.h"       // 追記
#include "Enemy2.h"
#include <vector>       
#include <random>       
#include <algorithm>
#include "math/MathUtility.h" // ★ これを追加する ★
#include "GameOverScene.h"

using namespace KamataEngine;

// ★ レベルアップ後のスキル選択肢の定義 (追加) ★
enum class SkillType {
	kAttackUp,  // 攻撃力アップ
	kSpeedUp,   // 移動速度アップ
	kHeal,      // HP回復
	kSkillCount // スキル数のカウント用
};

class GameScene {
public:
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

	bool IsGameOver() const { return isGameOver_; }

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

	//Graph* graph_ = nullptr;

	BIt_Map_Font* font_ = nullptr;
	int score_ = 0;


	// 経験値アイテムを格納するベクトル
	std::vector<Experience*> experiences_;
	// 吸引範囲 (例: 10.0f)
	const float ATTRACTION_RADIUS = 10.0f;


	// 敵の管理
	std::vector<Enemy*> enemies_;
	const int kMaxEnemies = 20;
	int enemySpawnTimer_ = 0;
	const int kEnemySpawnInterval = 120; // 120フレームごとに生成 (2秒)

	std::vector<Enemy2*> enemies2_; // ★ Enemy2のリストを追加 ★
	const int kMaxEnemies2 = 5;     // ★ Enemy2の最大数を設定 (出現数を制限) ★

	// HPバー用のスプライト (追加)
	KamataEngine::Sprite* hpBarBase_ = nullptr;
	KamataEngine::Sprite* hpBar_ = nullptr;
	uint32_t hpBarBaseTexture_ = 0;
	uint32_t hpBarTexture_ = 0;

	// ゲームオーバーフラグ (追加)
	bool isGameOver_ = false;


	// ★ レベルアップシステム関連 (追加) ★
	int level_ = 1;               // 現在のレベル
	int currentExp_ = 0;          // 現在の経験値 (score_から加算)
	int requiredExp_ = 10;        // 次のレベルまでに必要な経験値
	const int kExpBase = 10;      // 最初の必要経験値
	const float kExpScale = 1.2f; // 必要経験値の増加率 (レベルが上がるごとに必要経験値が1.2倍になる例)

	bool isLevelUpPending_ = false;              // レベルアップ待ち状態 (スキル選択画面表示中)
	int selectedSkillIndex_ = 0;                 // 選択中のスキルインデックス (0, 1, 2)
	std::vector<SkillType> currentSkillOptions_; // 現在のスキル選択肢 (3つ)
	// ------------------------------------

	// ★ スキル選択画面用スプライト (追加) ★
	KamataEngine::Sprite* skillScreenBackground_ = nullptr; // 半透明の背景
	KamataEngine::Sprite* skillOptionSprites_[3] = {};      // 3つの選択肢の背景
	KamataEngine::Sprite* skillCursorSprite_ = nullptr;     // 選択カーソル

	uint32_t whiteTextureHandle_ = 0; // スプライトの色付けに使う1x1の白テクスチャ
	// ----------------------------------------

	// 衝突判定関数 (追加)
	void CheckAllCollisions();

	// HPバー描画関数 (追加)
	void DrawHPBar();

	// 敵のランダム生成関数 (追加)
	void SpawnEnemy();

	// ★ スキル関連関数 (追加) ★
	void StartLevelUp();              // レベルアップ開始
	void UpdateSkillSelection();      // スキル選択画面の更新
	void ApplySkill(SkillType skill); // 選択したスキルを適用
};