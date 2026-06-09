#pragma once
#include "Enemy.h" // 親をインクルード

class Enemy4 : public Enemy { // ★継承を追加
public:
	Enemy4(const Vector3& position) : Enemy(position) {} // ★コンストラクタ修正

	void Initialize();
	void Update(const Vector3& playerPosition);
	// Draw, TakeDamage は親に任せるので消してOK

	bool CanShoot() override { return shotTimer_ >= kShotInterval; }
	void ResetShotTimer() override { shotTimer_ = 0; }
	int GetType() override { return 3; }

	float GetBulletSpeed() const override { return 0.6f; }
	int GetBulletDamage() const override { return 70; }

private:
	// 固有のタイマーだけ残す
	int shotTimer_ = 0;
	const int kShotInterval = 180;
	// model_ や worldTransform は親にあるので削除！
};