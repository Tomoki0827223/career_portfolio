#include "Experience.h"

Experience::Experience(const Vector3& initialPos) { worldTransform.translation_ = initialPos; }

Experience::~Experience() { delete model_; }

void Experience::Initialize() {
	// 経験値アイテム用の3Dモデルデータを生成（例としてブロックを使用）
	model_ = Model::CreateFromOBJ("block_4");

	worldTransform.Initialize();
	worldTransform.TransferMatrix();
}

void Experience::Update() {
	if (isAttracted_) {
		// 1. プレイヤー(target)へのベクトルを計算
		Vector3 direction = targetPosition_ - worldTransform.translation_;

		// 2. プレイヤーとの距離を計算
		float distance = Math::Length(direction);


		if (distance < 1.0f) {
			// プレイヤーに十分に近づいたら、取得済みとする
			isDead_ = true;
		} else {
			// 3. 正規化して単位ベクトルにする
			direction = Math::Normalize(direction);

			// 4. 移動処理 (Z座標は動かさない)
			worldTransform.translation_.x += direction.x * ATTRACTION_SPEED;
			worldTransform.translation_.y += direction.y * ATTRACTION_SPEED;
			// Z座標は動かさない: worldTransform.translation_.z = 0.0f; // 必要に応じて
		}
	}

	worldTransform.UpdateMatarix();
}

void Experience::Draw(const Camera& camera) {

	// Model::PreDraw() / PostDraw() は GameSceneでまとめて行う想定
	model_->Draw(worldTransform, camera);
}