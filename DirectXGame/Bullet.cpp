#include "Bullet.h"

Bullet::Bullet(const Vector3& position, const Vector3& velocity) : velocity_(Math::Normalize(velocity) * kMoveSpeed) { worldTransform.translation_ = position; }

Bullet::~Bullet() { delete model_; }

void Bullet::Initialize() {
	// 弾モデルとして"Bullet"を使用
	model_ = Model::CreateFromOBJ("Bullet");

	worldTransform.Initialize();
	worldTransform.scale_ = {0.5f, 0.5f, 0.5f};

	// 進行方向に向ける回転を計算（簡易的なもの）
	float angle = std::atan2(velocity_.x, velocity_.y);
	worldTransform.rotation_.z = angle;

	worldTransform.translation_.z = 0.0f;
	worldTransform.UpdateMatarix();
}

void Bullet::Update() {
	if (isDead_) {
		return;
	}

	// 移動
	worldTransform.translation_ += velocity_;

	// 画面外判定 (マップの範囲 MAP_HALF_RANGE = 50.0f を超えたら消滅)
	if (std::abs(worldTransform.translation_.x) > 55.0f || std::abs(worldTransform.translation_.y) > 55.0f) {
		isDead_ = true;
	}

	// 移動を反映
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void Bullet::Draw(const Camera& camera) {
	if (isDead_) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	model_->Draw(worldTransform, camera);

	Model::PostDraw();
}