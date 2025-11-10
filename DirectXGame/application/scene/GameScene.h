#pragma once
// ★修正: IScene.hのインクルードを削除 (ISceneを使わない方法へ変更) ★
// #include "External/KamataEngine/include/scene/IScene.h"

#include "2d/DebugText.h"
#include "2d/Sprite.h"
#include "3d/Camera.h" // unique_ptrで管理するためヘッダーを追加
#include "3d/PrimitiveDrawer.h"

#include "Book.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Enemy2.h"
#include "Experience.h"
#include "GameOverScene.h"
#include "Wine.h"

// 既存のヘッダーもインクルード
#include "application/3d/Player.h"
#include "application/3d/Stage.h"
#include "base/TextureManager.h"
#include "input/Input.h"
#include "math/MathUtility.h"

#include <algorithm>
#include <array>
#include <memory>
#include <random>
#include <vector>

using namespace KamataEngine;

// ★修正: ISceneの継承を削除し、純粋なクラスにする ★
class GameScene {
public:
	// スキル選択用の列挙型
	enum SkillType {
		kNone,
		kBook,      // Book.cpp/Book.h の効果
		kBullet,    // Bullet.cpp/Bullet.h の強化
		kHart,      // 回復 (Hart.pngに対応)
		kWine,      // Wine.cpp/Wine.h の効果
		kSkillCount // スキル総数（kNoneを除く）
	};

	// スキル情報構造体
	struct SkillInfo {
		SkillType type;
		std::string texturePath;         // スキルアイコンの画像パス
		std::string name;                // スキル名
		std::string description;         // スキルの説明
		Sprite* sprite = nullptr;        // スキルアイコン表示用Sprite
		Vector2 position = {0.0f, 0.0f}; // 表示位置
	};

	// コンストラクタ / デストラクタ
	GameScene() = default;
	~GameScene(); // ★修正: overrideを削除 ★

	// ISceneの仮想関数をオーバーライドしていた部分 (overrideを削除)
	void Initialize();
	void Update();
	void Draw();

	// ★main.cppで利用されるGetterを公開 ★
	bool IsGameOver() const { return isGameOver_; }
	void ResetGameOverFlag() { isGameOver_ = false; } // main.cppでフラグをリセットするために追加

	// スキル関連の関数プロトタイプ宣言
	void StartSkillSelection();
	void UpdateSkillSelection();
	void DrawSkillSelection();
	void ApplySkill(SkillType type);
	// ------------------------------------

private:
	// ------------------ ゲームオブジェクト管理 ------------------
	// ★ unique_ptr の vector はデストラクタで解放処理が不要になるため、GameScene.cppのデストラクタ修正と整合を取ります ★
	std::unique_ptr<Camera> camera_;
	Player* player_ = nullptr;
	std::unique_ptr<Stage> stage_;

	// 敵 (unique_ptrのまま維持)
	std::vector<std::unique_ptr<Enemy>> enemies_;
	std::vector<std::unique_ptr<Enemy2>> enemies2_;
	const int kMaxEnemies = 10;
	const int kMaxEnemies2 = 5;
	int enemySpawnTimer_ = 0;
	int enemy2SpawnTimer_ = 0;

	// 弾丸・スキルアイテム (生ポインタのvectorのまま維持)
	std::vector<Bullet*> bullets_;
	const int kBulletSpawnInterval = 60; // 弾丸発射間隔の定数を追加
	int bulletSpawnTimer_ = 0;
	std::vector<Book*> books_;
	std::vector<Wine*> wines_;
	const int kWineSpawnInterval = 600; // Wine生成間隔の定数を追加
	int wineSpawnTimer_ = 0;

	// 経験値アイテム (生ポインタのvectorのまま維持)
	std::vector<Experience*> experiences_;

	// UI
	KamataEngine::Sprite* hpBarBase_ = nullptr;
	KamataEngine::Sprite* hpBar_ = nullptr;
	uint32_t hpBarBaseTexture_ = 0;
	uint32_t hpBarTexture_ = 0;
	// ★未定義エラー解消のため追加: スコア表示系 ★
	int score_ = 0;
	DebugText* font_ = nullptr; // DebugTextのインスタンスを保持すると仮定
	// ----------------------------------------
	void DrawHPBar();
	void DrawLevelAndExp();

	// ------------------ スキル・レベルアップ関連 ------------------
	bool isSkillSelectionMode = false;

	// 全スキル情報 (kNoneを除く)
	std::array<SkillInfo, SkillType::kSkillCount - 1> allSkills_;

	// ランダムに選ばれた3つのスキル
	std::array<SkillInfo*, 3> selectedSkills_ = {nullptr, nullptr, nullptr};
	int currentSelectedIndex_ = 0;

	// スキル強化レベル (GameSceneでも保持)
	int bookLevel_ = 0;
	int bulletLevel_ = 0;
	int wineLevel_ = 0;

	// レベルアップシステム
	int level_ = 1;
	int currentExp_ = 0;
	int requiredExp_ = 100; // 初期必要経験値
	void UpdateLevel();
	void StartLevelUp() { UpdateLevel(); } // GameScene::cppでStartLevelUp()が呼ばれていたため追加
	// -----------------------------------------------------------------------------------

	// 衝突判定関数
	void CheckAllCollisions();
	// void PlayerAttackCollision(); // CheckAllCollisionsに統合されているため削除
	// void BulletEnemyCollision(); // CheckAllCollisionsに統合されているため削除
	void PlayerEnemyCollision();
	void PlayerExperienceCollision();
	void PlayerWineCollision();

	// 敵の生成関数 (GameScene.cppでSpawnEnemyを実装しているため、その宣言も追加)
	void EnemySpawn();  // EnemySpawnTimerの管理関数として宣言
	void Enemy2Spawn(); // Enemy2SpawnTimerの管理関数として宣言
	void SpawnEnemy();  // 実際の敵を生成する関数として宣言
	void SpawnWine();   // Wineアイテムを生成する関数として宣言

	// 弾の発射関数
	void BulletSpawn();

	// リストからの削除
	void RemoveDeadEntities();

	// ゲームオーバー処理
	bool isGameOver_ = false;
};