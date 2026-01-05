#include "Particle.h"
#include "math/MathUtility.h"

using namespace MathUtility;

void Particle::Initialize(Model* model, Vector3 position, Vector3 velocity) {
	//assert(model);
	model_ = model;
	velocity_ = velocity; // 速度を初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position; // ワールド変形の位置を初期化

	// 大きさ
	worldTransform_.scale_ = {0.2f, 0.2f, 0.2f}; // ワールド変形の大きさを初期化

	objectcolor_.Initialize();
	color_ = {0, 1, 0, 1};
}

void Particle::Update() {
	// フェード処理
	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f); // アルファ値を更新

	if (isFinished_) {
		return;
	}

	counter_ += 1.0f / 60.0f; // カウンターを更新

	if (counter_ >= kDuration) {

		counter_ = kDuration; // カウンターを最大値にする

		isFinished_ = true; // 寿命を超えたら終了
	}

	worldTransform_.translation_.x += velocity_.x; // ワールド変形の位置を初期化
	worldTransform_.translation_.y += velocity_.y; // ワールド変形の位置を初期化+
	worldTransform_.translation_.z += velocity_.z; // ワールド変形の位置を初期化
	worldTransform_.TransferMatrix();              // ワールド変形の転送
	worldTransform_.UpdateMatarix();               // ワールド変形の更新

	objectcolor_.SetColor(color_); // 色を設定

	// ★修正: 以下の行は削除またはコメントアウトしてください
	// model_->Draw(worldTransform_, *camera, &objectcolor_);
}

void Particle::Draw(Camera* camera) {

	Model::PreDraw();

	model_->Draw(worldTransform_, *camera, &objectcolor_); // 必要な引数を渡す

	Model::PostDraw();
}
