#include "Enemy2.h" // ヘッダーをEnemy2.hに変更

Enemy2::Enemy2(const Vector3& position) { worldTransform.translation_ = position; }

Enemy2::~Enemy2() { delete model_; }

void Enemy2::Initialize() {
	// 敵モデルとして"cube"を使用
	model_ = Model::CreateFromOBJ("cube"); // ★ モデルを"cube"に変更 ★

	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f};

	worldTransform.translation_.z = 0.0f; // Z座標を固定
	worldTransform.UpdateMatarix();
}

void Enemy2::Update(const Vector3& playerPosition) {
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