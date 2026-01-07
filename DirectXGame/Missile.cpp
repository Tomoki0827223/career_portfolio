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

	// --- 1. ホーミング処理（既存の移動ロジック） ---
	Vector3 currentPos = worldTransform.translation_;
	Vector3 directionToTarget = targetPosition - currentPos;
	Vector3 desiredVelocity = Math::Normalize(directionToTarget) * kMoveSpeed;

	// 速度ベクトルの更新（慣性をつける）
	velocity_.x = velocity_.x + (desiredVelocity.x - velocity_.x) * kHomingRate;
	velocity_.y = velocity_.y + (desiredVelocity.y - velocity_.y) * kHomingRate;
	velocity_.z = velocity_.z + (desiredVelocity.z - velocity_.z) * kHomingRate;

	// ベクトルの正規化と速度適用
	// (速度がほぼ0の時にNormalizeするとバグる可能性があるため長さをチェック推奨ですが、今回は簡易的に)
	velocity_ = Math::Normalize(velocity_) * kMoveSpeed;

	// --- 2. 移動 ---
	worldTransform.translation_ += velocity_;

	// --- 3. ★旋回制御（修正版）★ ---

	// 移動方向（velocity）から目標角度（ラジアン）を算出
	// std::atan2(y, x) はX軸プラス方向を0度として計算します
	float targetAngle = std::atan2(velocity_.y, velocity_.x);

	// 【重要】モデルの元々の向きに合わせて補正値を足す必要があります
	// モデルが「上(+Y)」を向いて作られている場合: -M_PI / 2.0f (-90度) を足す
	// モデルが「右(+X)」を向いて作られている場合: 補正なし
	// モデルが「下(-Y)」を向いて作られている場合: +M_PI / 2.0f (+90度) を足す
	// ※ミサイルが横向きになってしまう場合は、ここの値を変更してください
	float angleOffset = -M_PI / 2.0f;
	targetAngle += angleOffset;

	// 現在の角度との差分を計算
	float angleDiff = targetAngle - worldTransform.rotation_.z;

	// 角度差を -PI ～ +PI (-180度 ～ +180度) の範囲に正規化
	// これにより、350度→10度への移動が「+20度」と判定され、逆回転しなくなります
	while (angleDiff > M_PI)
		angleDiff -= 2.0f * M_PI;
	while (angleDiff < -M_PI)
		angleDiff += 2.0f * M_PI;

	// 差分に補間率を掛けて、現在の角度に足す（スムーズな回転）
	worldTransform.rotation_.z += angleDiff * kRotationLerpRate;

	// --- 4. 行列の更新 ---
	// 動きを確認するため、一旦X軸回転（傾き）はコメントアウトしています。
	// Z軸回転が正しくできてから有効にすることをお勧めします。
	worldTransform.rotation_.x = 0.0f;

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