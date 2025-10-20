#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/MathUtility.h"

using namespace KamataEngine;

class Enemy {
public:
	Enemy(const Vector3& position);
	~Enemy();

	void Initialize();
	// プレイヤーの位置を受け取り、追尾・更新を行う
	void Update(const Vector3& playerPosition);
	void Draw(const Camera& camera);

	// 衝突判定/被弾判定用
	Vector3 GetPosition() const { return worldTransform.translation_; }
	float GetRadius() const { return radius_; }
	bool IsDead() const { return isDead_; }

	// ダメージ処理
	void TakeDamage(int damage);

private:
	// 敵のステータス
	const float kMoveSpeed = 0.1f;
	const float radius_ = 0.5f;
	const int kMaxHp = 3;
	int currentHp_ = kMaxHp;

	// モデルとワールド変換
	Model* model_ = nullptr;
	WorldTransform worldTransform;

	bool isDead_ = false; // HPが0になったらtrue
};