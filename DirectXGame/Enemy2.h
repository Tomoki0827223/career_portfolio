#pragma once
#include "Enemy.h" // 親クラスをインクルード

// ★ : public Enemy を追加して継承させる
class Enemy2 : public Enemy {
public:
	// コンストラクタは親のコンストラクタを呼ぶようにします
	Enemy2(const Vector3& position) : Enemy(position) {}

	// 親にある関数を「上書き（オーバーライド）」するものだけ宣言
	void Initialize();
	void Update(const Vector3& playerPosition);

	// 弾発射ロジックを持つ場合のみ、親の仮想関数を上書き
	bool CanShoot() override { return shotTimer_ >= kShotInterval; }
	void ResetShotTimer() override { shotTimer_ = 0; }
	int GetType() override { return 1; } // 敵タイプ1

	// 親の関数を上書き
	float GetBulletSpeed() const override { return 1.2f; }
	int GetBulletDamage() const override { return 30; }

private:
	// Enemy2 固有の変数だけ残す
	int shotTimer_ = 0;
	const int kShotInterval = 45;
	// model_ や worldTransform は親(Enemy)にあるものを使うので削除してOK
};