#include "Stage.h"

Stage::~Stage() { delete model_; }

void Stage::Initialize() {
	// 地面用の 3D モデルを生成（cube などを大きく引き伸ばして地面にします）
	model_ = Model::CreateFromOBJ("cube");
	worldTransform_.Initialize();

	// 地面を非常に大きく広げる（ワールドの広さになります）
	worldTransform_.scale_ = {2000.0f, 1.0f, 2000.0f};
	// プレイヤーの足元（Y=0）より少し下に配置
	worldTransform_.translation_ = {0.0f, -2.0f, 0.0f};

	worldTransform_.UpdateMatarix();
}

void Stage::Update() {
	// 静止した地面なので TransferMatrix のみ
	worldTransform_.TransferMatrix();
}

void Stage::Draw(const Camera& camera) {

	Model::PreDraw();
	// スプライトではなく 3D モデルとして描画
	model_->Draw(worldTransform_, camera);
	Model::PreDraw();
}