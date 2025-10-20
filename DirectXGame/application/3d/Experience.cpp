#include "Experience.h"

#define NOMINMAX
#include "math/MathUtility.h" // Math::Length, Math::Normalizeを使うために必要
#include <algorithm>
#include <cmath>  // std::cos, std::sin を使うために必要
#include <random> // 乱数を使うために必要

// 【修正】静的定数の定義 (実体化)
const float Experience::ATTRACTION_RADIUS = 3.0f;      // 吸引開始半径
const float Experience::ATTRACTION_SPEED = 0.3f;       // 吸引速度
const float Experience::INITIAL_VELOCITY_SCALE = 0.5f; // 初速の最大値

Experience::Experience(const Vector3& initialPos) {
	worldTransform.translation_ = initialPos;
	worldTransform.scale_ = {0.5f, 0.5f, 0.5f}; // 初期スケールを設定

	// 乱数生成器 (Experienceごとのランダムな散らばり処理)
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * 3.1415926535f);
	std::uniform_real_distribution<float> distSpeed(0.1f, INITIAL_VELOCITY_SCALE);

	// ランダムな方向と強さの初速を生成 (X, Z平面)
	float angle = distAngle(engine);
	float speed = distSpeed(engine);

	scatterVelocity_.x = std::cos(angle) * speed;
	scatterVelocity_.z = std::sin(angle) * speed;
	scatterVelocity_.y = 0.0f;
}

Experience::~Experience() { delete model_; }

void Experience::Initialize() {
	model_ = Model::CreateFromOBJ("block_4");
	worldTransform.Initialize();
	worldTransform.TransferMatrix();
}

// 【修正】Update関数の引数を追加
void Experience::Update(const Vector3& playerPos) {
	targetPosition_ = playerPos; // ターゲット位置を毎フレーム更新

	if (!isAttracted_) {
		// 1. 散らばり処理（SCATTER_TIME経過で停止）
		if (scatterTimer_ < SCATTER_TIME) {
			// Vector3の演算子オーバーロードを使用
			worldTransform.translation_ = worldTransform.translation_ + scatterVelocity_;
			// 速度を徐々に減衰させる
			scatterVelocity_ = scatterVelocity_ * 0.9f;
			scatterTimer_++;
		}

		// GameScene側で吸引判定を行うため、ここでは吸引開始判定は行わない
	}

	if (isDead_) {
		// 死亡済みの場合は処理をスキップ
		return;
	}

	if (isAttracted_) {
		// 3. プレイヤーへの追尾（吸引処理）
		// Vector3の演算子オーバーロードを使用
		Vector3 direction = targetPosition_ - worldTransform.translation_;
		float distance = Math::Length(direction);

		if (distance < ATTRACTION_SPEED * 2.0f) { // 取得判定を少し広めに
			// プレイヤーに十分に近づいたら、取得済みとしてマーク (GameSceneで削除される)
			isDead_ = true;
		} else {
			// プレイヤーに向かって移動
			direction = Math::Normalize(direction);
			// Vector3の演算子オーバーロードを使用
			worldTransform.translation_ = worldTransform.translation_ + (direction * ATTRACTION_SPEED);
		}
	}

	// Y座標が0を下回らないようにする
	if (worldTransform.translation_.y < 0.0f) {
		worldTransform.translation_.y = 0.0f;
	}

	worldTransform.UpdateMatarix();
}


void Experience::Draw(const Camera& camera) {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	if (!isDead_) {
		// isDead_がfalseの場合のみ描画
		model_->Draw(worldTransform, camera);
	}

	Model::PostDraw();
}