#include "Experience.h"

Experience::Experience(int textureHandle, Vector2 position, Model* model) : position_(position), collected_(false), model_(model) { sprite_ = Sprite::Create(textureHandle, position_); }

void Experience::Update() {
	// 取得済みなら何もしない
	if (collected_)
		return;
	// 必要ならアニメーションなど
}

void Experience::Draw() {
	if (!collected_) {
		sprite_->Draw(); // 2Dスプライト

		if (model_) {
			// 3Dモデルの描画
			WorldTransform wt;
			wt.translation_ = {position_.x, position_.y, 0.0f}; // 2D座標を3Dに
			wt.scale_ = {0.5f, 0.5f, 0.5f};                     // 必要に応じて調整
			wt.rotation_ = {0.0f, 0.0f, 0.0f};
			wt.UpdateMatarix();

			// カメラはGameSceneのcamera_を渡す
			extern Camera camera_; // もしくはGameSceneから参照
			model_->Draw(wt, camera_);
		}
	}
}

Vector3 Experience::GetPosition() const { return position_; }

bool Experience::IsCollected(const Vector3& playerPos, float collectRadius) {
	if (collected_)
		return false;
	float dx = playerPos.x - position_.x;
	float dy = playerPos.y - position_.y;
	float distSq = dx * dx + dy * dy;
	if (distSq < collectRadius * collectRadius) {
		collected_ = true;
		return true;
	}
	return false;
}