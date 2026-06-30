#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/MathUtility.h"

using namespace KamataEngine;

// 前方宣言
class EnemyState;

class Enemy {
public:
	Enemy(const Vector3& position);
	virtual ~Enemy();

	virtual float GetBulletSpeed() const { return 1.0f; }
	virtual int GetBulletDamage() const { return 10; }

	virtual void Initialize();
	virtual void Update(const Vector3& playerPosition);

	void Draw(const Camera& camera);

	const Vector3& GetPosition() const { return worldTransform.translation_; }
	float GetRadius() const { return radius_; }
	bool IsDead() const { return isDead_; }
	void TakeDamage(int damage);

	virtual bool CanShoot() { return false; }
	virtual void ResetShotTimer() {}
	virtual Vector3 GetShotPosition() { return worldTransform.translation_; }
	virtual int GetType() { return 0; }

	// ★★★ State Pattern 用に追加する関数 ★自慢のコードになる部分 ★★★
	void ChangeState(EnemyState* newState);
	float GetMoveSpeed() const { return kMoveSpeed; }
	void Move(const Vector3& velocity) { worldTransform.translation_ += velocity; }
	void SetScale(const Vector3& scale) { worldTransform.scale_ = scale; }

protected:
	const float kMoveSpeed = 0.1f;
	const float radius_ = 1.0f;
	const int kMaxHp = 10;
	int currentHp_ = kMaxHp;

	Model* model_ = nullptr;
	WorldTransform worldTransform;

	bool isDead_ = false;

	// ★★★ 現在の状態を指すポインタ（条件3のポリモーフィズム用） ★★★
	EnemyState* currentState_ = nullptr;
};