#pragma once
#include "BIt_Map_Font.h"
#include "Graph.h"
#include "KamataEngine.h"
#include "Player.h"
#include "Stage.h"
#include "Experience.h" 
#include "Enemy.h"       // 追記
#include <vector>       
#include <random>       
#include <algorithm>
#include "math/MathUtility.h" // ★ これを追加する ★

using namespace KamataEngine;

class GameScene {
public:
	~GameScene();

	void Initialize();

	void Update();

	void Draw();

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

	// 敵を格納するベクトル (追加)
	std::vector<Enemy*> enemies_;
	// 敵の生成間隔タイマーと最大数 (追加)
	int enemySpawnTimer_ = 0;
	const int kEnemySpawnInterval = 120; // 120フレーム(2秒)に1回
	const int kMaxEnemies = 30;          // 最大敵数

	// HPバー用のスプライト (追加)
	KamataEngine::Sprite* hpBarBase_ = nullptr;
	KamataEngine::Sprite* hpBar_ = nullptr;
	uint32_t hpBarBaseTexture_ = 0;
	uint32_t hpBarTexture_ = 0;

	// ゲームオーバーフラグ (追加)
	bool isGameOver_ = false;

	// 衝突判定関数 (追加)
	void CheckAllCollisions();

	// HPバー描画関数 (追加)
	void DrawHPBar();

	// 敵のランダム生成関数 (追加)
	void SpawnEnemy();
};