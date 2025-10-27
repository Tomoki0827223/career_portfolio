#include "Wine.h"

Wine::Wine(const Vector3& position) { worldTransform.translation_ = position; }

Wine::~Wine() { delete model_; }

void Wine::Initialize() {
	// モデルとして"wine"を使用
	model_ = Model::CreateFromOBJ("wine");

	worldTransform.Initialize();
	worldTransform.scale_ = {1.0f, 1.0f, 1.0f};

	worldTransform.translation_.z = 0.0f;
	worldTransform.UpdateMatarix();
}

void Wine::Update(const Vector3& playerPosition) {
	// Wineはマップに固定配置されるため、ここでは特別な移動処理は行わない
	(void)playerPosition;
}

void Wine::Draw(const Camera& camera) {
	if (isDead_) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	model_->Draw(worldTransform, camera);

	Model::PostDraw();
}