#include "Enemy5.h"

Enemy5::Enemy5(const Vector3& position) { worldTransform.translation_ = position; }

Enemy5::~Enemy5() { delete model_; }

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

	// プレイヤーへの方向ベクトルを計算
	Vector3 diff = playerPosition - worldTransform.translation_;

	// 正規化して移動 (追尾)
	Vector3 direction = Math::Normalize(diff);
	worldTransform.translation_ += direction * kMoveSpeed; // kMoveSpeed (0.15f) を適用

	// 移動を反映
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void Enemy5::Draw(const Camera& camera) {
	if (isDead_) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	model_->Draw(worldTransform, camera);

	Model::PostDraw();
}

void Enemy5::TakeDamage(int damage) {
	currentHp_ -= damage;
	if (currentHp_ <= 0) {
		currentHp_ = 0;
		isDead_ = true;
	}
}