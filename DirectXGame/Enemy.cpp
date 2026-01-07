#include "Enemy.h"

Enemy::Enemy(const Vector3& position) { worldTransform.translation_ = position; }

Enemy::~Enemy() { delete model_; }

void Enemy::Initialize() {
	// 敵モデルとして"cube"を使用
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

	// 1. プレイヤーへの方向ベクトルを計算
	Vector3 diff = playerPosition - worldTransform.translation_;

	// 2. 移動処理 (既存)
	Vector3 direction = Math::Normalize(diff);
	worldTransform.translation_ += direction * kMoveSpeed;

	// 3. 【追加】プレイヤーの方を向く回転制御（イージング/補間）
	// atan2を使って、X軸とZ軸の差分から目標のY軸角度を求める
	float targetRotationY = std::atan2(diff.x, diff.z);

	// 現在の角度から目標の角度へ滑らかに補間
	// ※角度の最短補間（360度をまたぐ場合）を考慮する場合はさらなる処理が必要ですが、
	// シンプルなイージングであれば以下のように記述できます。
	worldTransform.rotation_.y = (1.0f - kRotationLerpRate) * worldTransform.rotation_.y + kRotationLerpRate * targetRotationY;

	// 4. 行列の更新
	worldTransform.UpdateMatarix();
	// TransferMatrixは定数バッファへの転送
	worldTransform.TransferMatrix();
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