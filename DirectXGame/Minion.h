#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/Vector3.h"

using namespace KamataEngine;

class Minion {
public:
	// ミニオンの総数と自身のインデックスを受け取る (周回位置計算用)
	Minion(int index, int total);
	~Minion();

	void Initialize();
	// プレイヤーの位置を渡して追従・周回
	void Update(const Vector3& playerPosition);
	void Draw(const Camera& camera);

	Vector3 GetPosition() const { return worldTransform.translation_; }
	int GetDamage() const { return damage_; }

	// 攻撃頻度のためのタイマー管理
	bool CanAttack() const { return attackTimer_ <= 0; }
	void ResetAttackTimer() { attackTimer_ = kMaxAttackInterval; }

private:
	const float kFollowRadius = 3.0f; // プレイヤーからの追従半径
	const float kFollowSpeed = 0.1f;  // 追従速度
	int damage_ = 1;                  // 攻撃ダメージ

	int index_;
	int total_;

	// 攻撃頻度
	int attackTimer_ = 0;
	const int kMaxAttackInterval = 120; // 2秒ごとに攻撃

	Model* model_ = nullptr;
	WorldTransform worldTransform;
};