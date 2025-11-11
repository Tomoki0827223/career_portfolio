#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/MathUtility.h"
#include "math/Vector3.h"

using namespace KamataEngine;

class Boomerang {
public:
	// 投擲開始位置と初期進行方向を受け取る
	Boomerang(const Vector3& startPosition, const Vector3& targetVelocity);
	~Boomerang();

	void Initialize();
	// プレイヤーの位置を渡して戻り方向を計算
	void Update(const Vector3& playerPosition);
	void Draw(const Camera& camera);

	Vector3 GetPosition() const { return worldTransform.translation_; }
	float GetRadius() const { return radius_; }
	bool IsDead() const { return isDead_; }
	void Hit() { isDead_ = true; } // 敵に当たったら消滅

	void SetDamage(int damage) { damage_ = damage; }
	int GetDamage() const { return damage_; }

private:
	const float kMaxDistance = 15.0f; // プレイヤーからの最大飛距離
	const float kMoveSpeed = 0.8f;    // 移動速度
	const float radius_ = 0.5f;       // 判定半径
	int damage_ = 1;                  // ダメージ量

	Model* model_ = nullptr;
	WorldTransform worldTransform;
	Vector3 velocity_;         // 現在の移動ベクトル
	Vector3 startPosition_;    // 投擲開始位置
	bool isDead_ = false;      // 消滅フラグ
	bool isReturning_ = false; // 戻り中フラグ
};