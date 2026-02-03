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

	// 【修正点 2】Z座標（奥行き）も強制的に0.0fに固定
	worldTransform.translation_.z = 0.0f;

	worldTransform.scale_ = {0.5f, 0.5f, 0.5f};

	// 乱数分布 (角度と速度)
	std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * 3.1415926535f);
	std::uniform_real_distribution<float> distSpeed(0.1f, INITIAL_VELOCITY_SCALE);

	float angle = distAngle(exp_engine);
	float speed = distSpeed(exp_engine);

	// X軸（左右）:
	scatterVelocity_.x = std::cos(angle) * speed;
	// Z軸（奥行き）の初速も0.0fに固定
	scatterVelocity_.z = 0.0f;

	// Y軸（上下）にも初速を与えて、飛び散るアニメーションを表現
	std::uniform_real_distribution<float> distYJump(0.2f, 0.4f);
	scatterVelocity_.y = distYJump(exp_engine);

	scatterTimer_ = 0;
}

Experience::~Experience() { delete model_; }

void Experience::Initialize() {
	model_ = Model::CreateFromOBJ("block_4");
	worldTransform.Initialize();
	worldTransform.TransferMatrix();
}

void Experience::Update(const Vector3& playerPos) {
	targetPosition_ = playerPos;

	if (isDead_) {
		return;
	}

	if (!isAttracted_) {
		// プレイヤーへの方向ベクトル
		Vector3 toPlayer = playerPos - worldTransform.translation_;
		float distance = Math::Length(toPlayer);

		if (distance > 0.1f) {
			// 徐々にスピードを上げて「シュルシュル」感を出す
			attractionSpeed_ += 0.05f;
			worldTransform.translation_ += Math::Normalize(toPlayer) * attractionSpeed_;
		}

		// 1. 散らばり処理フェーズ
		if (scatterTimer_ < SCATTER_TIME) {
			// 移動
			worldTransform.translation_ = worldTransform.translation_ + scatterVelocity_;
			// 速度減衰 (水平・上下方向両方に作用)
			scatterVelocity_ = scatterVelocity_ * 0.92f;
			// Y軸方向は重力のような減速をかける
			scatterVelocity_.y -= 0.05f;

			// 【追加】散らばり中もZ座標は常に0.0fに固定する
			worldTransform.translation_.z = 0.0f;

			scatterTimer_++;
		}

		// 2. 散らばりが完了してから、吸引開始の判定を行う
		if (scatterTimer_ >= SCATTER_TIME) {
			Vector3 directionToPlayer = targetPosition_ - worldTransform.translation_;
			if (Math::Length(directionToPlayer) <= ATTRACTION_RADIUS) {
				isAttracted_ = true; // 吸引開始
			}
		}

	} else { // isAttracted_ == true (吸引フェーズ)
		// 3. プレイヤーへの追尾（吸引処理）
		Vector3 direction = targetPosition_ - worldTransform.translation_;
		float distance = Math::Length(direction);

		if (distance < 0.5f) {
			isDead_ = true; // プレイヤーに取得された
		} else {
			direction = Math::Normalize(direction);
			worldTransform.translation_ = worldTransform.translation_ + (direction * ATTRACTION_SPEED);

			// 【追加】吸引中もZ座標は常に0.0fに固定する
			worldTransform.translation_.z = 0.0f;
		}
	}

	worldTransform.UpdateMatarix();
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