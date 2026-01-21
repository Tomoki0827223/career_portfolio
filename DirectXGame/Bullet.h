#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/MathUtility.h"
#include "math/Vector3.h"
#include <Enemy.h>

using namespace KamataEngine;

class Bullet {
public:
	// プレイヤーの位置と最も近い敵の方向を受け取るコンストラクタ
	Bullet(const Vector3& position, const Vector3& velocity);
	~Bullet();

	void Initialize();
	void Update();
	void Draw(const Camera& camera);

	void SetTargetPos(const Vector3& pos) {
		targetPos_ = pos;
		hasTarget_ = true;
	}

	Vector3 GetPosition() const { return worldTransform.translation_; }
	float GetRadius() const { return radius_; }
	bool IsDead() const { return isDead_; }
	void Die() { isDead_ = true; }

	void SetDamage(int damage) { damage_ = damage; }
	int GetDamage() const { return damage_; }

private:
	const float kMoveSpeed = 0.5f;
	const float radius_ = 0.3f; // 弾の判定半径
	int damage_ = 1;            // ダメージ量

	Model* model_ = nullptr;
	WorldTransform worldTransform;
	Vector3 velocity_;    // 移動ベクトル
	bool isDead_ = false; // 命中または画面外でtrue
	

	Vector3 targetPos_;              // ターゲットの座標
	bool hasTarget_ = false;         // ターゲットがいるか
	const float kHomingLimit = 0.1f; // 曲がる強さ (0.01～0.2の間で調整)
};