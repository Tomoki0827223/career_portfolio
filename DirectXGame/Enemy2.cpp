#include "Enemy2.h" // ヘッダーをEnemy2.hに変更

Enemy2::Enemy2(const Vector3& position) { worldTransform.translation_ = position; }

Enemy2::~Enemy2() { delete model_; }

void Enemy2::Initialize() {
	// 敵モデルとして"cube"を使用
	model_ = Model::CreateFromOBJ("enemy"); // ★ モデルを"cube"に変更 ★

	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f};

	worldTransform.translation_.z = 0.0f; // Z座標を固定
	worldTransform.UpdateMatarix();
}

void Enemy2::Update(const Vector3& playerPosition) {
	if (isDead_) {
		return;
	}

	// 1. プレイヤーへの方向ベクトルを計算
	Vector3 diff = playerPosition - worldTransform.translation_;

	// --- 移動処理 (既存) ---
	Vector3 direction = Math::Normalize(diff);
	worldTransform.translation_ += direction * kMoveSpeed;

	// --- ★追加: 旋回（イージング）処理 ---
	// 目標とする角度を算出
	float targetRotationY = std::atan2(diff.x, diff.z);

	// 現在の角度から目標角度へ補間 (Lerp)
	// worldTransform.rotation_.y を直接更新
	worldTransform.rotation_.y = (1.0f - kRotationLerpRate) * worldTransform.rotation_.y + kRotationLerpRate * targetRotationY;

	// 2. 行列の更新 (既存)
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void Enemy2::Draw(const Camera& camera) {
	if (isDead_) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	model_->Draw(worldTransform, camera);

	Model::PostDraw();
}

void Enemy2::TakeDamage(int damage) {
	currentHp_ -= damage;
	if (currentHp_ <= 0) {
		currentHp_ = 0;
		isDead_ = true;
	}
}