#include "Enemy4.h"

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
	worldTransform.UpdateMatarix();  // ★ 先に計算！
	worldTransform.TransferMatrix(); // ★ 後で転送！
}