#pragma once
#include "Book.h"
#include "Boomerang.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Enemy2.h"
#include "Experience.h"
#include "KamataEngine.h"
#include "Minion.h"
#include "Missile.h"
#include "Player.h"
#include "Wine.h"
#include "application/ui/BIt_Map_Font.h"
#include "math/MathUtility.h"
#include <algorithm>
#include <random>
#include <vector>
#include <string> // ★ 追加: std::string, std::to_string 用
#include <cmath>  // ★ 追加: std::pow 用
#include "2d/ImGuiManager.h"

using namespace KamataEngine;

// GameScene.h から移動: レベルアップ後のスキル選択肢の定義
enum class SkillType {
	kBook,      // Book: プレイヤーの周りを回る攻撃
	kBullet,    // Bullet: 自動で敵に撃つ
	kHeart,     // Heart: HP回復 (既存のkHealの代替)
	kWine,      // Wine: ランダムドロップアイテムの出現
	kBoomerang, // Boomerang: 投擲して戻ってくる攻撃
	kMinion,    // Minion: 味方を召喚
	kMissile,   // Missile: ホーミングミサイルを発射
	kSkillCount // スキル数のカウント用
};

class GameLogic {
public:
	// GameSceneから必要な依存オブジェクトを受け取る
	GameLogic(Player* player, BIt_Map_Font* font, KamataEngine::Sprite* hpBar, KamataEngine::Sprite* hpBarBase);
	~GameLogic();

	void Initialize();
	// メインの更新処理
	void Update();
	// 3Dオブジェクトの描画処理をGameSceneから呼び出すための関数
	void DrawObjects(const Camera& camera);

	// 外部（GameScene）から状態を取得するためのGetter
	bool IsLevelUpPending() const { return isLevelUpPending_; }

	// スキル選択画面の制御処理 (GameSceneからUI更新のため呼び出される)
	void UpdateSkillSelection();
	void DrawSkillSelectionUI(KamataEngine::Sprite* skillCursorSprite, KamataEngine::Sprite* skillOptionSprites[], KamataEngine::Sprite* skillScreenBackground);

#ifdef _DEBUG
	void DrawImGui();
#endif

	// 描画に必要なテクスチャハンドルをGameSceneへ渡すためのGetter
	// スキルアイコンのテクスチャハンドル配列を返す
	const uint32_t* GetSkillTextureHandles() const { return skillTextureHandles_; }


private:
	// GameSceneから移動したオブジェクト管理
	std::vector<Bullet*> bullets_;
	std::vector<Book*> books_;
	std::vector<Wine*> wines_;
	std::vector<Boomerang*> boomerangs_;
	std::vector<Minion*> minions_;
	std::vector<Missile*> missiles_;
	std::vector<Enemy*> enemies_;
	std::vector<Enemy2*> enemies2_;
	std::vector<Experience*> experiences_;

	// 依存オブジェクト
	Player* player_ = nullptr;
	BIt_Map_Font* font_ = nullptr;
	KamataEngine::Sprite* hpBar_ = nullptr;
	KamataEngine::Sprite* hpBarBase_ = nullptr;

	int score_ = 0;

	// タイマーと定数 (GameSceneから移動)
	const int kBulletSpawnInterval = 60;
	int bulletSpawnTimer_ = 0;
	const int kWineSpawnInterval = 600;
	int wineSpawnTimer_ = 0;
	const int kBoomerangSpawnInterval = 180;
	int boomerangSpawnTimer_ = 0;
	const int kMissileSpawnInterval = 90;
	int missileSpawnTimer_ = 0;

	const int kMaxEnemies = 20;
	int enemySpawnTimer_ = 0;
	const int kEnemySpawnInterval = 120;
	const int kMaxEnemies2 = 5;

	// レベルアップシステム関連 (GameSceneから移動)
	int level_ = 1;
	int currentExp_ = 0;
	int requiredExp_ = 150;
	const int kExpBase = 100;
	const float kExpScale = 1.2f;

	bool isLevelUpPending_ = false;
	int selectedSkillIndex_ = 0;
	std::vector<SkillType> currentSkillOptions_;

	// 内部処理 (GameSceneから移動)
	void CheckAllCollisions();
	void SpawnEnemy();
	void SpawnWine();
	void StartLevelUp();
	void ApplySkill(SkillType skill);

	uint32_t skillTextureHandles_[static_cast<int>(SkillType::kSkillCount)] = {};
	KamataEngine::Sprite* skillIconSprites_[3] = {};

#ifdef _DEBUG
	KamataEngine::Vector2 iconSize_ = {64.0f, 64.0f};
#endif
	
	// 新しいテクスチャのハンドル
	uint32_t boomerangTextureHandle_ = 0;
	uint32_t minionTextureHandle_ = 0;
	uint32_t missileTextureHandle_ = 0;
};