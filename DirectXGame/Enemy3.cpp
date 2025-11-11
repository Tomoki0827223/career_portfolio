#include "Enemy3.h"

Enemy3::Enemy3(const Vector3& position) { worldTransform.translation_ = position; }

Enemy3::~Enemy3() { delete model_; }

void Enemy3::Initialize() {
	// 敵モデルとして"cube"を使用
	model_ = Model::CreateFromOBJ("enemy3");

	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f}; // ★ 敵のサイズを2倍に ★

	worldTransform.translation_.z = 0.0f; // Z座標を固定
	worldTransform.UpdateMatarix();
}

///**
// * @brief 敵の更新。プレイヤーを追尾するロジックを実装。
// * @param playerPosition プレイヤーの現在位置
// */

void Enemy3::Update(const Vector3& playerPosition) {
	if (isDead_) {
		return;
	}

	// ★ 追記: 弾発射タイマーの更新 ★
	if (shotTimer_ < kShotInterval) {
		shotTimer_++;
	}

	// プレイヤーへの方向ベクトルを計算
	Vector3 diff = playerPosition - worldTransform.translation_;

	// 正規化して移動 (追尾)
	Vector3 direction = Math::Normalize(diff);
	worldTransform.translation_ += direction * kMoveSpeed;

	// 移動を反映
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void Enemy3::Draw(const Camera& camera) {
	if (isDead_) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	model_->Draw(worldTransform, camera);

	Model::PostDraw();
}

void Enemy3::TakeDamage(int damage) {
	currentHp_ -= damage;
	if (currentHp_ <= 0) {
		currentHp_ = 0;
		isDead_ = true;
	}
}