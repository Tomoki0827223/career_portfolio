#pragma once
#include "Particle.h"
#include <list>
#include <memory>
// 【追加】ViewProjectionの定義を含むヘッダーを追加
#include "3d/Camera.h" // ViewProjectionの定義は通常Camera.hに含まれます

using namespace KamataEngine; // ★これがインクルードの前に必要かもしれません★

class ParticleManager {

public:
	static ParticleManager* GetInstance(); // シングルトン

	void Initialize();
	void Update();
	void Draw(const Camera& camera);

	/// <summary>
	/// スプラッシュ（爆発）パーティクルを生成する
	/// </summary>
	void CreateSplash(const Vector3& position, const Vector4& baseColor);

private:
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	std::list<std::unique_ptr<Particle>> particles_;
};