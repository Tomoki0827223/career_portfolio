#include "Enemy5.h"

void Enemy5::Initialize() {
	// 敵モデルとして"enemy"を使用 (必要に応じて別のモデルに変更してください)
	model_ = Model::CreateFromOBJ("enemy2");

	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f};

	worldTransform.translation_.z = 0.0f; // Z座標を固定
	worldTransform.UpdateMatarix();
}

void Enemy5::Update(const Vector3& playerPosition) {
	if (isDead_) {
		return;
	}

	if (shotTimer_ < kShotInterval) {
		shotTimer_++;
	}

	// プレイヤーへの方向ベクトルを計算
	Vector3 diff = playerPosition - worldTransform.translation_;

	// 正規化して移動 (追尾)
	Vector3 direction = Math::Normalize(diff);
	worldTransform.translation_ += direction * kMoveSpeed; // kMoveSpeed (0.15f) を適用

	// 移動を反映
	worldTransform.UpdateMatarix();  // ★ 先に計算！
	worldTransform.TransferMatrix(); // ★ 後で転送！
}