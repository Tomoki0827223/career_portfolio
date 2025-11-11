#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/MathUtility.h"
#include "math/Vector3.h"

using namespace KamataEngine;

class Missile {
public:
	// プレイヤー位置と初期ターゲット位置を受け取る
	Missile(const Vector3& position, const Vector3& targetPosition);
	~Missile();

	void Initialize();
	// 現在のターゲットの位置を受け取り、ホーミング処理を行う
	void Update(const Vector3& targetPosition);
	void Draw(const Camera& camera);

	Vector3 GetPosition() const { return worldTransform.translation_; }
	float GetRadius() const { return radius_; }
	bool IsDead() const { return isDead_; }
	void Die() { isDead_ = true; }

	void SetDamage(int damage) { damage_ = damage; }
	int GetDamage() const { return damage_; }

private:
	const float kMoveSpeed = 0.6f;
	const float kHomingRate = 0.06f; // 追尾率 (大きいほど急カーブ)
	const float radius_ = 0.4f;      // 弾の判定半径
	int damage_ = 3;                 // ダメージ量 (既存のBulletより高めに設定)

	Model* model_ = nullptr;
	WorldTransform worldTransform;
	Vector3 velocity_; // 移動ベクトル (Updateで方向が更新される)
	bool isDead_ = false;
};