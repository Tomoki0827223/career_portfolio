#include "Missile.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

Missile::Missile(const Vector3& position, const Vector3& targetPosition) {
	worldTransform.translation_ = position;
	// 初期速度はターゲットへ向かう方向
	Vector3 initialDirection = targetPosition - position;
	velocity_ = Math::Normalize(initialDirection) * kMoveSpeed;
}

Missile::~Missile() { delete model_; }

void Missile::Initialize() {
	model_ = Model::CreateFromOBJ("Missile"); // Bulletと同じモデルを使用
	worldTransform.Initialize();
	worldTransform.scale_ = {0.8f, 0.8f, 0.8f};
	worldTransform.translation_.z = 0.0f;
	worldTransform.UpdateMatarix();
}

void Missile::Update(const Vector3& targetPosition) {
	if (isDead_) {
		return;
	}

	// 1. ホーミング処理
	Vector3 currentPos = worldTransform.translation_;
	Vector3 directionToTarget = targetPosition - currentPos;

	Vector3 desiredVelocity = Math::Normalize(directionToTarget) * kMoveSpeed;

	// 現在の速度と目標速度を線形補間し、滑らかに方向転換する
	// 【修正】Math::Lerpが未定義のため、線形補間を直接記述
	velocity_.x = velocity_.x + (desiredVelocity.x - velocity_.x) * kHomingRate;
	velocity_.y = velocity_.y + (desiredVelocity.y - velocity_.y) * kHomingRate;
	velocity_.z = velocity_.z + (desiredVelocity.z - velocity_.z) * kHomingRate;

	// 速度を正規化し、速度係数を乗算して一定速度を保つ
	velocity_ = Math::Normalize(velocity_) * kMoveSpeed;

	// 2. 移動
	worldTransform.translation_ += velocity_;

	// 3. 画面外判定
	if (std::abs(worldTransform.translation_.x) > 55.0f || std::abs(worldTransform.translation_.y) > 55.0f) {
		isDead_ = true;
	}

	// 4. 進行方向に向ける回転を更新
	float angle = std::atan2(velocity_.x, velocity_.y);
	worldTransform.rotation_.z = angle;

	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
	// ★ 追加: 旋回を強調するために、機体を傾ける (オプション) ★
	// desiredVelocity (目標方向) と velocity (現在方向) の差分が回転の傾きになる
	// 差が大きければ大きく傾ける
	float velocityDiffAngle = std::atan2(desiredVelocity.x, desiredVelocity.y) - angle;
	// 角度の最短経路を計算 (-PI から PI の範囲に正規化)
	if (velocityDiffAngle > M_PI) {
		velocityDiffAngle -= 2.0f * M_PI;
	} else if (velocityDiffAngle < -M_PI) {
		velocityDiffAngle += 2.0f * M_PI;
	}

	// 角度差に応じてX軸を傾ける (Y軸が上方向だと仮定)
	// 角度差に係数をかけて傾きを調整 (例: 0.5f)
	worldTransform.rotation_.x = velocityDiffAngle * 0.5f;
	// ----------------------------------------------------

	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void Missile::Draw(const Camera& camera) {
	if (isDead_) {
		return;
	}

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();
	model_->Draw(worldTransform, camera);
	Model::PostDraw();
}