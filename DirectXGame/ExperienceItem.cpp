#include "ExperienceItem.h"
#include "affine.h"

void ExperienceItem::Initialize(const Vector3& position) {

	// モデルのロードはGameSceneに移動済み

	worldTransform.Initialize();
	worldTransform.translation_ = position;

	// ★ 修正: sizeFactorを削除し、固定スケールを使用
	const float fixedScale = 1.0f;
	worldTransform.scale_ = {0.2f * fixedScale, 0.2f * fixedScale, 0.2f * fixedScale};

	// ★ 修正: 半径と吸引距離を固定値に設定
	const float fixedRadius = 0.5f * fixedScale;           // 例: 0.5f
	const float fixedAttractionRange = 10.0f * fixedScale; // 例: 10.0f（吸引距離を広げた）

	radius_ = fixedRadius;
	attractionRange_ = fixedAttractionRange;

	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void ExperienceItem::Update(const KamataEngine::Vector3& playerPosition) {
	if (isCollected_) {
		return;
	}

	// プレイヤーとの距離を計算
	Vector3 toPlayer = Subtract(playerPosition, worldTransform.translation_);
	float distanceSquared = toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z;
	float distance = std::sqrt(distanceSquared);

	// 吸い寄せ範囲内にいるかチェック (attractionRange_ を使用)
	if (distance < attractionRange_) {
		// プレイヤーの方向へ移動
		Vector3 moveDirection = Normalize(toPlayer);

		// 移動速度をかけて移動 (attractionSpeed_ を使用)
		worldTransform.translation_.x += moveDirection.x * attractionSpeed_;
		worldTransform.translation_.y += moveDirection.y * attractionSpeed_;
		worldTransform.translation_.z += moveDirection.z * attractionSpeed_;
	}

	// WorldTransformの更新
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void ExperienceItem::Draw(const Camera& camera) {

	if (!isCollected_) {
		// Nullチェック
		if (model_ == nullptr) {
			return;
		}

		model_->Draw(worldTransform, camera);
	}
}
