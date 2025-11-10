#include "Wine.h"
#include "3d/Model.h" 
#include "ParticleManager.h" // ParticleManager.hをインクルード
#include "math/Vector4.h"
#include "3d/PrimitiveDrawer.h"

void Wine::Initialize(const Vector3& initialPos) {
	model_ = KamataEngine::Model::CreateFromOBJ("wine");

	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	worldTransform_.translation_ = initialPos;

	isActive_ = true;
	dropSpeed_ = 0.5f;
	worldTransform_.UpdateMatarix();
}


void Wine::Update() {
	if (!isActive_) {
		return;
	}

	// 落下処理: Y座標を減らす
	worldTransform_.translation_.y -= dropSpeed_;

	if (worldTransform_.translation_.y <= kGroundY) {
		worldTransform_.translation_.y = kGroundY;

		// 【修正】KamataEngine:: を削除して、グローバルなParticleManagerを呼び出す
		//         Vector4には KamataEngine:: を維持します
		KamataEngine::Vector4 wineColor = {0.5f, 0.0f, 0.2f, 1.0f};

		ParticleManager::GetInstance()->CreateSplash(worldTransform_.translation_, wineColor);

		isActive_ = false;
	}
	worldTransform_.UpdateMatarix();
}


// Wine::Draw の修正
void Wine::Draw(const KamataEngine::Camera& camera) {
	if (isActive_ && model_) {
		model_->Draw(worldTransform_, camera);
	}
}