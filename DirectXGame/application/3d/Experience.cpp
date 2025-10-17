#include "Experience.h"

#define NOMINMAX 
#include <algorithm> // std::minを使うために必要
#include <cmath>     // std::minを使うために必要
#include <algorithm> // 必要に応じて追加。std::min/maxは通常これに含まれる。

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

		// スケールを徐々に大きくする
		worldTransform.scale_.x = (std::min)(worldTransform.scale_.x + SCALE_SPEED, MAX_SCALE);
		worldTransform.scale_.y = (std::min)(worldTransform.scale_.y + SCALE_SPEED, MAX_SCALE);
		worldTransform.scale_.z = (std::min)(worldTransform.scale_.z + SCALE_SPEED, MAX_SCALE);

		if (distance < 1.0f) {
			// プレイヤーに十分に近づいたら、取得済みとする
			isDead_ = true;
		} else {
			// 3. 正規化して単位ベクトルにする
			direction = Math::Normalize(direction);

			// 4. 移動処理 (Z座標は動かさない)
			worldTransform.translation_.x += direction.x * ATTRACTION_SPEED;
			worldTransform.translation_.y += direction.y * ATTRACTION_SPEED;
			// Z座標は動かさない: worldTransform.translation_.z = 0.0f;
		}
	} else {
		// isAttracted_がfalseの場合
		// ここからMIN_SCALEを参照しようとしているが、ifブロックの外
		worldTransform.scale_ = {MIN_SCALE, MIN_SCALE, MIN_SCALE}; // ★ C2065エラー
	}

	worldTransform.UpdateMatarix();
}

void Experience::Draw(const Camera& camera) { // 【修正】引数としてカメラを受け取る

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	// Model::PreDraw() / PostDraw() は GameSceneでまとめて行う想定
	// 【修正】引数で受け取ったカメラを使用する
	model_->Draw(worldTransform, camera);

	Model::PostDraw();
}