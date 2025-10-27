#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/Vector3.h"

using namespace KamataEngine;

class Wine {
public:
	Wine(const Vector3& position);
	~Wine();

	void Initialize();
	// Updateは現状、マップに固定配置されるため、プレイヤー位置で何もしない
	void Update(const Vector3& playerPosition);
	void Draw(const Camera& camera);

	Vector3 GetPosition() const { return worldTransform.translation_; }
	float GetRadius() const { return radius_; }
	bool IsDead() const { return isDead_; }
	void Die() { isDead_ = true; }

	int GetHealAmount() const { return kHealAmount; } // 回復量

private:
	const float radius_ = 0.5f;
	const int kHealAmount = 50; // HP回復量

	Model* model_ = nullptr;
	WorldTransform worldTransform;
	bool isDead_ = false;
};