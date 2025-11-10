#pragma once
#include "3d/Camera.h" // Cameraが必要なため追加
#include "3d/Model.h"  // Modelが必要なため追加
#include "3d/WorldTransform.h"
#include "input/Input.h" // Inputが必要なため追加
#include "math/MathUtility.h"
#include "math/Vector3.h"

using namespace KamataEngine;

class Player {
public:
	// シングルトン化 (既存コードがGetInstance()を使っていると想定)
	static Player* GetInstance();

	Player();
	~Player();

	void Initialize();
	void Update();
	void Draw(); // Drawの引数がない既存コードのsnippetに合わせた

	Vector3 GetPosition() const { return worldTransform.translation_; }
	float GetRadius() const { return radius_; } // 半径のGetterを追加 (既存ロジックに必要)

	// HP関連
	void TakeDamage(int damage) { currentHP_ -= damage; }

	// ★修正: Heal関数は宣言のみに留め、定義はPlayer.cppで行う ★
	void Heal(int amount);

	int GetCurrentHP() const { return currentHP_; }
	int GetMaxHP() const { return kMaxHP_; }

	// 攻撃関連
	float GetAttackRadius() const { return kAttackRadius_; } // 近接攻撃の判定半径
	bool IsAttacking() const { return isAttacking_; }

	// 死亡関連
	void Die() { isDead_ = true; }
	bool IsDead() const { return isDead_; }
	int GetDeadTimer() const { return deadTimer_; }
	int GetMaxDeadTime() const { return kMaxDeadTime_; }

	// ★追加: スキルによる強化値のGetter/Setter (GameScene::ApplySkillで利用) ★
	void SetBulletDamage(int damage) { bulletDamage_ = damage; }
	int GetBulletDamage() const { return bulletDamage_; }
	void SetMoveSpeedMultiplier(float multiplier) { moveSpeedMultiplier_ = multiplier; }
	float GetMoveSpeedMultiplier() const { return moveSpeedMultiplier_; }

private:
	// シングルトン用の静的インスタンス
	static Player* instance_;

	// HP (kMaxHp_ -> kMaxHP_ に修正)
	const int kMaxHP_ = 100;
	int currentHP_ = kMaxHP_; // ★修正: 初期値を最大HPに設定 ★

	// 攻撃
	const float kAttackRadius_ = 1.0f; // 攻撃の判定半径
	bool isAttacking_ = false;
	const int kMaxAttackTime_ = 10;
	int attackTimer_ = 0;

	// 死亡
	const int kMaxDeadTime_ = 60; // 死亡モーションの最大時間
	int deadTimer_ = 0;
	bool isDead_ = false;

	// 移動
	const float kMoveSpeed = 0.2f;
	const float radius_ = 0.5f;

	// ★追加: スキルによる強化値 (Initializeでリセットされる) ★
	int bulletDamage_ = 1;
	float moveSpeedMultiplier_ = 1.0f;

	// オブジェクトデータ
	Model* modelPlayer_ = nullptr;
	Input* input_ = nullptr;
	Camera camera_; // PlayerのカメラはGameSceneのカメラと別物として扱う
	WorldTransform worldTransform;
};