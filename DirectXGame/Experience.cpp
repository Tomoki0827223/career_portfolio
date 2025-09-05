#include "Experience.h"

Experience::Experience(int textureHandle, Vector2 position, Model* model) : position_(position), collected_(false), model_(model) { sprite_ = Sprite::Create(textureHandle, position_); }

void Experience::Update() {
	// 取得済みなら何もしない
	if (collected_)
		return;
	// 必要ならアニメーションなど
}

void Experience::Draw2D() {
	if (!collected_) {
		sprite_->Draw(); // 2Dスプライトの描画のみ
	}
}

void Experience::Draw3D(const Camera& camera) {
	if (!collected_) {
		if (model_) {
			WorldTransform wt;
			wt.translation_ = {position_.x, position_.y, 0.0f};
			wt.scale_ = {0.5f, 0.5f, 0.5f};
			wt.rotation_ = {0.0f, 0.0f, 0.0f};
			wt.UpdateMatarix();
			model_->Draw(wt, camera); // 3Dモデルの描画のみ
		}
	}
}

Vector3 Experience::GetPosition() const { // 修正: 戻り値をVector3に
	return {position_.x, position_.y, 0.0f};
}

bool Experience::IsCollected(const Vector3& playerPos, float collectRadius) { // 修正: 引数をVector3に
	if (collected_)
		return false;
	float dx = playerPos.x - position_.x;
	float dy = playerPos.y - position_.y;
	float distance = std::sqrt(dx * dx + dy * dy);

	if (distance < collectRadius) {
		collected_ = true;
		return true;
	}

	return false;
}