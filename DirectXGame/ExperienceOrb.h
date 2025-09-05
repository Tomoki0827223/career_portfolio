#pragma once
#include "3d/WorldTransform.h"
#include "KamataEngine.h"

class ExperienceOrb {
public:
	// コンストラクタ
	ExperienceOrb();
	// デストラクタ
	~ExperienceOrb();

	// 初期化処理
	void Initialize(const KamataEngine::Vector3& position);
	// 更新処理
	void Update();
	// 描画処理
	void Draw(const KamataEngine::Camera& camera);

	// 座標の取得
	KamataEngine::Vector3 GetPosition() const;

	// 生存状態
	bool IsAlive() const;

	// 消滅させる
	void SetIsDead();

private:
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	bool isAlive_ = true;
};