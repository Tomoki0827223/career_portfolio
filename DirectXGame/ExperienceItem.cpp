#include "ExperienceItem.h"
#include "affine.h"

void ExperienceItem::Initialize(const KamataEngine::Vector3& position) {
	// モデルの生成 (例: 小さな球や別のブロック)
	model_ = Model::CreateFromOBJ("block_4");
	model_ = Model::CreateFromOBJ("block_4");

	worldTransform.Initialize();
	worldTransform.translation_ = position;
	// ★ 大きさ（スケール）を固定値に設定 (例: 標準サイズ 1.0f)
	const float fixedScale = 1.0f;
	worldTransform.scale_ = {0.2f * fixedScale, 0.2f * fixedScale, 0.2f * fixedScale};
	worldTransform.scale_ = {0.2f * sizeFactor, 0.2f * sizeFactor, 0.2f * sizeFactor};
	// ★ 半径と吸引距離を固定値に設定
	const float fixedRadius = 0.5f * fixedScale;          // 例: 0.5f
	const float fixedAttractionRange = 5.0f * fixedScale; // 例: 5.0f
	radius_ = 0.5f * sizeFactor;

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

		Model::PreDraw();

		model_->Draw(worldTransform, camera);
	}
}