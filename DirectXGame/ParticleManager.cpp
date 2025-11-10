// ParticleManager.cpp (新規作成)
#include "ParticleManager.h"
#include "math/MathUtility.h" // 乱数生成のためKamataEngineのMathUtilityを想定
#include <random>

ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Initialize() { particles_.clear(); }

void ParticleManager::Update() {
	// パーティクルを更新し、寿命が尽きたものを削除
	particles_.remove_if([](std::unique_ptr<Particle>& particle) {
		particle->Update();
		return particle->IsDead();
	});
}

void ParticleManager::Draw(const KamataEngine::Camera& camera) {
	// PrimitiveDrawerにカメラを設定します（これは DrawLine3d の描画に必要）
	KamataEngine::PrimitiveDrawer::GetInstance()->SetCamera(&camera);

	for (const auto& particle : particles_) {
		// 【修正】引数なしで呼び出す
		particle->Draw();
	}
}

void ParticleManager::CreateSplash(const Vector3& position, const Vector4& baseColor) {
	std::mt19937 randomEngine(std::random_device{}());
	// XZ平面のランダム速度、Y軸の上方向への速度、寿命
	std::uniform_real_distribution<float> velocityDistXY(-0.2f, 0.2f);
	std::uniform_real_distribution<float> velocityDistY(0.1f, 0.5f);
	std::uniform_int_distribution<int> lifeDist(30, 60);

	const int kNumParticles = 20; // 20個のパーティクルを生成

	for (int i = 0; i < kNumParticles; ++i) {
		Vector3 velocity = {velocityDistXY(randomEngine), velocityDistY(randomEngine), velocityDistXY(randomEngine)};

		int life = lifeDist(randomEngine);

		std::unique_ptr<Particle> newParticle = std::make_unique<Particle>();
		// 地面にめり込まないよう、生成位置を少し上げる (0.1f)
		Vector3 spawnPos = position;
		spawnPos.y += 0.1f;

		newParticle->Initialize(spawnPos, velocity, baseColor, life);
		particles_.push_back(std::move(newParticle));
	}
}