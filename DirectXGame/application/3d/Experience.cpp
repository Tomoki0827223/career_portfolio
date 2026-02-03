#include "Experience.h"

#define NOMINMAX
#include "math/MathUtility.h"
#include <algorithm>
#include <cmath>
#include <random> // 乱数を使うために必要

// 【修正】乱数ジェネレータを無名名前空間に静的に定義
namespace {
std::random_device exp_seed_gen;
std::mt19937 exp_engine(exp_seed_gen());
} // namespace

// 静的定数の定義 (実体化)
const float Experience::ATTRACTION_RADIUS = 3.0f;      // 吸引開始半径
const float Experience::ATTRACTION_SPEED = 0.3f;       // 吸引速度
const float Experience::INITIAL_VELOCITY_SCALE = 0.5f; // 初速の最大値

Experience::Experience(const Vector3& initialPos) {
	worldTransform.translation_ = initialPos;
	isAttracted_ = false;
	isDead_ = false;

	// 散らばる力（初速）を与える
	std::uniform_real_distribution<float> angleDist(0, 2.0f * 3.14159f);

	// ★ engine を exp_engine に修正
	float angle = angleDist(exp_engine);

	float speed = 0.5f;
	scatterVelocity_.x = std::cos(angle) * speed;
	scatterVelocity_.y = std::sin(angle) * speed;
	scatterTimer_ = 15;
}

Experience::~Experience() { delete model_; }

void Experience::Initialize() {
	model_ = Model::CreateFromOBJ("block_4");
	worldTransform.Initialize();
	worldTransform.TransferMatrix();
}

void Experience::Update(const Vector3& playerPos) {
	// 距離を計算
	Vector3 toPlayer = playerPos - worldTransform.translation_;
	float distance = Math::Length(toPlayer);

	// ★ 追記：プレイヤーに触れたら即座に消滅フラグを立てる (0.5f は当たり判定サイズ)
	if (distance < 1.0f) {
		isDead_ = true;
	}

	if (scatterTimer_ > 0) {
		worldTransform.translation_ += scatterVelocity_;
		scatterVelocity_ *= 0.9f;
		scatterTimer_--;
	} else if (isAttracted_) {
		// 吸い込み中
		attractionSpeed_ += 0.05f;
		worldTransform.translation_ += Math::Normalize(toPlayer) * attractionSpeed_;
	} else {
		// 通常時：すごく近づいた時だけ自動で吸い始める
		if (distance < 3.0f) {
			isAttracted_ = true;
		}
	}

	worldTransform.UpdateMatarix();
	worldTransform.TransferMatrix();
}

void Experience::Draw(const Camera& camera) {
	if (!isDead_) {
		DirectXCommon* dxCommon = DirectXCommon::GetInstance();
		dxCommon->ClearDepthBuffer();
		Model::PreDraw();
		model_->Draw(worldTransform, camera);
		Model::PostDraw();
	}
}