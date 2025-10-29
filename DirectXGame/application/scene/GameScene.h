#pragma once
#include "BIt_Map_Font.h"
#include "Book.h"   // ★追加
#include "Bullet.h" // ★追加
#include "Enemy.h"  // 追記
#include "Enemy2.h"
#include "Experience.h"
#include "GameOverScene.h"
#include "Graph.h"
#include "KamataEngine.h"
#include "Player.h"
#include "Stage.h"
#include "Wine.h"             // ★追加
#include "math/MathUtility.h" // ★ これを追加する ★
#include <algorithm>
#include <random>
#include <vector>
#include "SkillSelect.h"

using namespace KamataEngine;

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

	// ★追加: スキル関連のオブジェクト管理 ★
	std::vector<Bullet*> bullets_;
	const int kBulletSpawnInterval = 60; // 60フレームごとに発射
	int bulletSpawnTimer_ = 0;

	std::vector<Book*> books_;

	std::vector<Wine*> wines_;
	const int kWineSpawnInterval = 600; // 600フレーム (10秒) ごとに生成
	int wineSpawnTimer_ = 0;
	// ------------------------------------

	// HPバー用のスプライト (追加)
	KamataEngine::Sprite* hpBarBase_ = nullptr;
	KamataEngine::Sprite* hpBar_ = nullptr;
	uint32_t hpBarBaseTexture_ = 0;
	uint32_t hpBarTexture_ = 0;

	// ゲームオーバーフラグ (追加)
	bool isGameOver_ = false;

	// ★ 経験値アイテムの削除処理から経験値とスコア加算のロジックはGameSceneに残します ★
	int level_ = 1;               // 現在のレベル
	int currentExp_ = 0;          // 現在の経験値 (score_から加算)
	int requiredExp_ = 150;       // 次のレベルまでに必要な経験値
	const int kExpBase = 100;     // 最初の必要経験値
	const float kExpScale = 1.2f; // 必要経験値の増加率 (レベルが上がるごとに必要経験値が1.2倍になる例)

	// ★ 修正: isLevelUpPending_ のみを残し、UI関連の変数を削除 ★
	bool isLevelUpPending_ = false; // レベルアップ待ち状態 (スキル選択画面表示中)

	// 衝突判定関数 (追加)
	void CheckAllCollisions();

	// HPバー描画関数 (追加)
	void DrawHPBar();

	// 敵のランダム生成関数 (追加)
	void SpawnEnemy();

	// ★追加: Wineのランダム生成関数 ★
	void SpawnWine();

	// ★ 修正: スキル関連関数を削除・修正 ★
	void StartLevelUp(); // レベルアップ開始 (GameScene内に残す)
	                     // ★ 削除: UpdateSkillSelection は SkillSelect に移管します ★
	                     // void UpdateSkillSelection();
	                     // ★ 削除: ApplySkill は SkillSelect に移管します ★
	                     // void ApplySkill(SkillType skill);
};