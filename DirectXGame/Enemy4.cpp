#include "Enemy4.h"

Enemy4::Enemy4(const Vector3& position) { worldTransform.translation_ = position; }

Enemy4::~Enemy4() { delete model_; }

void Enemy4::Initialize() {
	// 敵モデルとして"cube"を使用
	model_ = Model::CreateFromOBJ("enemy2"); // Enemyモデルを使用

	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f}; // ★ 敵のサイズを2倍に ★

	worldTransform.translation_.z = 0.0f; // Z座標を固定
	worldTransform.UpdateMatarix();
}

/**
 * @brief 敵の更新。プレイヤーを追尾するロジックを実装。
 * @param playerPosition プレイヤーの現在位置
 */
void Enemy4::Update(const Vector3& playerPosition) {
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

void Enemy4::Draw(const Camera& camera) {
	if (isDead_) {
		return;
	}

	// ★ 修正: DirectXCommon関連の呼び出しを削除 (GameLogic::DrawObjectsでまとめて行うため) ★
	// DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// dxCommon->ClearDepthBuffer();
	 Model::PreDraw();

	 model_->Draw(worldTransform, camera);

	 Model::PostDraw();
}

void Enemy4::TakeDamage(int damage) {
	currentHp_ -= damage;
	if (currentHp_ <= 0) {
		currentHp_ = 0;
		isDead_ = true;
	}
}