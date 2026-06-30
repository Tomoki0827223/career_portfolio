#pragma once
#include "math/MathUtility.h" // Vector3用

class Enemy; // 前方宣言

class EnemyState {
public:
	virtual ~EnemyState() = default;
	virtual void Enter(Enemy* enemy) = 0;
	virtual void Update(Enemy* enemy, const KamataEngine::Vector3& playerPos) = 0;
};

// --- 1. 出現状態 ---
class EnemyStateSpawn : public EnemyState {
private:
	float timer_ = 0.0f;

public:
	// 引数名「enemy」を「/*enemy*/」にすることで警告を回避します
	void Enter(Enemy* /*enemy*/) override { timer_ = 0.0f; }
	void Update(Enemy* enemy, const KamataEngine::Vector3& playerPos) override;
};

// --- 2. 追尾状態 ---
class EnemyStateApproach : public EnemyState {
public:
	// 引数名「enemy」を「/*enemy*/」にすることで警告を回避します
	void Enter(Enemy* /*enemy*/) override {}
	void Update(Enemy* enemy, const KamataEngine::Vector3& playerPos) override;
};