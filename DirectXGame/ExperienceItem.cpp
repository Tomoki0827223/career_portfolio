#include "ExperienceItem.h"
#include "affine.h"

void ExperienceItem::Initialize(const Vector3& position) {

	worldTransform.Initialize();
	worldTransform.translation_ = position;

	// ★ 大きさ（スケール）を固定値に設定 (例: 標準サイズ 1.0f)
	const float fixedScale = 1.0f;
	// スケールを少し大きくする
	worldTransform.scale_ = {0.5f * fixedScale, 0.5f * fixedScale, 0.5f * fixedScale};

	// ★ 半径と吸引距離を固定値に設定
	// プレイヤーの半径が 1.0f なので、アイテムの半径も大きくする
	const float fixedRadius = 0.5f * fixedScale;
	const float fixedAttractionRange = 10.0f * fixedScale; // 吸引距離を広げる

	radius_ = fixedRadius;
	attractionRange_ = fixedAttractionRange;

	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void ExperienceItem::Update(const KamataEngine::Vector3& playerPosition) {
	// ... (以下のロジックは変更なし)
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