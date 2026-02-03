#include "Enemy3.h"

void Enemy3::Initialize() {
	model_ = Model::CreateFromOBJ("enemy3");
	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform.translation_.z = 0.0f;
	worldTransform.UpdateMatarix();
}

void Enemy3::Update(const Vector3& playerPosition) {
	if (IsDead())
		return;
	if (shotTimer_ < kShotInterval)
		shotTimer_++;

	Vector3 diff = playerPosition - worldTransform.translation_;
	Vector3 direction = Math::Normalize(diff);
	worldTransform.translation_ += direction * kMoveSpeed;

	worldTransform.UpdateMatarix();
	worldTransform.TransferMatrix();
}
// ★ Draw() や TakeDamage() は Enemy.cpp と同じなので、このファイルからは消してOK！