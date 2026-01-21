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
	if (isDead_)
		return;

	// ★追加: 追尾（ホーミング）ロジック
	if (hasTarget_) {
		// 敵への方向を計算
		Vector3 toTarget = Math::Normalize(targetPos_ - worldTransform.translation_);

		// 現在の速度に、敵への方向を少しだけ混ぜる（これで曲がる）
		velocity_ = velocity_ + (toTarget * kHomingLimit);

		// 弾の速さが変わらないように、元のスピード(kMoveSpeed)で固定する
		velocity_ = Math::Normalize(velocity_) * kMoveSpeed;

		// 弾の見た目の向きも更新
		float angle = std::atan2(velocity_.x, velocity_.y);
		worldTransform.rotation_.z = angle;
	}

	// 移動処理（既存）
	worldTransform.translation_ += velocity_;
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