#include "ExperienceItem.h"
#include "affine.h"

// sizeFactor を受け取るように変更
void ExperienceItem::Initialize(const KamataEngine::Vector3& position, float sizeFactor) {
	// モデルの生成 (例: 小さな球や別のブロック)
	model_ = Model::CreateFromOBJ("block_4");

	worldTransform.Initialize();
	worldTransform.translation_ = position;

	// ★ 大きさ（スケール）を設定
	// sizeFactorが1.0fで標準サイズ、2.0fで2倍の大きさになる
	worldTransform.scale_ = {0.2f * sizeFactor, 0.2f * sizeFactor, 0.2f * sizeFactor};

	// ★ 半径と吸引距離を設定
	// 半径はスケールに連動させる (例: 1.0f * sizeFactor)
	radius_ = 0.5f * sizeFactor;

	// ★ 吸引距離は大きさに比例させる (例: 吸引の基準距離 5.0f * sizeFactor)
	attractionRange_ = 5.0f * sizeFactor;

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
		Model::PreDraw();
		model_->Draw(worldTransform, camera);
		Model::PostDraw();
	}

}