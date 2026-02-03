#include "Enemy2.h"

void Enemy2::Initialize() {
	// model_ や worldTransform は親クラス(Enemy)のメンバをそのまま使えます
	model_ = Model::CreateFromOBJ("enemy");

	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform.translation_.z = 0.0f;
	worldTransform.UpdateMatarix();
}

void Enemy2::Update(const Vector3& playerPosition) {
	if (IsDead())
		return; // 親の関数を使用

	if (shotTimer_ < kShotInterval)
		shotTimer_++;

	// 移動ロジック（親と同じなら親の Update を呼ぶだけでもOK）
	Vector3 diff = playerPosition - worldTransform.translation_;
	Vector3 direction = Math::Normalize(diff);
	worldTransform.translation_ += direction * 0.1f; // Speed

	worldTransform.UpdateMatarix();
	worldTransform.TransferMatrix();
}

// ★ Draw や TakeDamage は Enemy.cpp と全く同じなので、
// Enemy2.cpp からは削除してしまって大丈夫です！（親の関数が自動で呼ばれます）