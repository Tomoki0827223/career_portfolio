#pragma once
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "KamataEngine.h"
#include "math/MathUtility.h"
#include "math/Vector3.h"
#include <random>
#include <cmath>

using namespace KamataEngine;

class Book {
public:
	Book();
	~Book();

	void Initialize();
	// プレイヤーの位置を受け取り、周回する
	void Update(const Vector3& playerPosition);
	void Draw(const Camera& camera);

	Vector3 GetPosition() const { return worldTransform.translation_; }
	float GetRadius() const { return radius_; }

	void SetDamage(int damage) { damage_ = damage; }
	int GetDamage() const { return damage_; }

private:
	// 周回パラメータ
	const float kRotationRadius = 3.0f; // プレイヤーからの距離
	const float kRotationSpeed = 0.08f; // 1フレームあたりの回転量 (ラジアン)
	const float radius_ = 0.5f;         // 判定半径
	int damage_ = 1;                    // ダメージ量

	Model* model_ = nullptr;
	WorldTransform worldTransform;
	float currentAngle_ = 0.0f; // 現在の角度 (ラジアン)
};