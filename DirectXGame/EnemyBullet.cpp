#include "EnemyBullet.h"

// 乱数生成器がGameLogic.cppで定義されているため、ここではマップ外判定は省略

EnemyBullet::EnemyBullet(const Vector3& position, const Vector3& velocity) : velocity_(velocity) { worldTransform.translation_ = position; }

EnemyBullet::~EnemyBullet() { delete model_; }

void EnemyBullet::Initialize() {
	// 仮のモデルとして"sphere"を使用
	model_ = Model::CreateFromOBJ("Bullet");

	worldTransform.Initialize();
	worldTransform.scale_ = {0.5f, 0.5f, 0.5f}; // 弾のサイズ
	worldTransform.UpdateMatarix();
}

void EnemyBullet::Update() {
	if (isDead_) {
		return;
	}

	// 寿命タイマー
	lifeTimer_++;
	if (lifeTimer_ >= kLifeTime) {
		isDead_ = true;
		return;
	}

	// 移動処理
	worldTransform.translation_ += velocity_;

	// 行列の更新
	worldTransform.TransferMatrix(); // ★ 修正: 移動を反映させるための行列転送を追加 ★
	worldTransform.UpdateMatarix();
}

void EnemyBullet::Draw(const Camera& camera) {
	if (isDead_) {
		return;
	}

	Model::PreDraw();

	model_->Draw(worldTransform, camera);

	Model::PostDraw();
}