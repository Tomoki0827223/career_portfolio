#include "Enemy.h"

Enemy::Enemy(const Vector3& position) { worldTransform.translation_ = position; }

Enemy::~Enemy() { delete model_; }

void Enemy::Initialize() {
	// 敵モデルとして"cube"を使用
	//model_ = Model::CreateFromOBJ("enemy");
	model_ = Model::CreateFromOBJ("enemy");

	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f}; // ★ 敵のサイズを2倍に ★

	worldTransform.translation_.z = 0.0f; // Z座標を固定
	worldTransform.UpdateMatarix();
}

///**
// * @brief 敵の更新。プレイヤーを追尾するロジックを実装。
// * @param playerPosition プレイヤーの現在位置
// */


void Enemy::Update(const Vector3& playerPosition) {
	if (isDead_) {
		return;
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

void Enemy::Draw(const Camera& camera) {
	if (isDead_) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	model_->Draw(worldTransform, camera);

	Model::PostDraw();
}

void Enemy::TakeDamage(int damage) {
	currentHp_ -= damage;
	if (currentHp_ <= 0) {
		currentHp_ = 0;
		isDead_ = true;
	}
}