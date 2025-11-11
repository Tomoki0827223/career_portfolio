#include "Boomerang.h"
#include "math/MathUtility.h"

Boomerang::Boomerang(const Vector3& startPosition, const Vector3& targetVelocity) : startPosition_(startPosition) {
	velocity_ = Math::Normalize(targetVelocity) * kMoveSpeed;
	worldTransform.translation_ = startPosition;
}

Boomerang::~Boomerang() { delete model_; }

void Boomerang::Initialize() {
	model_ = Model::CreateFromOBJ("axe"); // axeモデルを使用
	worldTransform.Initialize();
	worldTransform.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform.translation_.z = 0.0f;

	// 初期回転 (進行方向に向ける)
	float angle = std::atan2(velocity_.x, velocity_.y);
	worldTransform.rotation_.z = angle;

	worldTransform.UpdateMatarix();
}

void Boomerang::Update(const Vector3& playerPosition) {
	if (isDead_) {
		return;
	}

	Vector3 currentPos = worldTransform.translation_;
	float distanceToPlayer = Math::Length(currentPos - playerPosition);
	float distanceToStart = Math::Length(currentPos - startPosition_);

	if (!isReturning_) {
		// 1. 投擲フェーズ
		worldTransform.translation_ += velocity_;

		// プレイヤーから最大飛距離を超えたら戻りフェーズへ
		if (distanceToStart >= kMaxDistance) {
			isReturning_ = true;
		}
	}

	if (isReturning_) {
		// 2. 戻りフェーズ
		Vector3 directionToPlayer = playerPosition - currentPos;

		// プレイヤーに近づく方向のベクトルを計算
		Vector3 returnVelocity = Math::Normalize(directionToPlayer) * kMoveSpeed * 1.5f; // 戻りは速めに
		worldTransform.translation_ += returnVelocity;

		// プレイヤーの近くに戻ったら消滅
		if (distanceToPlayer <= 1.0f) {
			isDead_ = true;
		}
	}

	// 回転アニメーション
	worldTransform.rotation_.x += 0.5f;
	worldTransform.rotation_.y += 0.2f;

	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void Boomerang::Draw(const Camera& camera) {
	if (isDead_) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();
	model_->Draw(worldTransform, camera);
	Model::PostDraw();
}