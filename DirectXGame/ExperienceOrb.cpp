#include "ExperienceOrb.h"

ExperienceOrb::ExperienceOrb() {
	// "exp.obj" を "block_4.obj" に変更
	model_ = KamataEngine::Model::CreateFromOBJ("block_4.obj");
	worldTransform_.Initialize();
}

ExperienceOrb::~ExperienceOrb() { delete model_; }

void ExperienceOrb::Initialize(const KamataEngine::Vector3& position) {
	worldTransform_.translation_ = position;
	worldTransform_.UpdateMatarix();
}

void ExperienceOrb::Update() { worldTransform_.UpdateMatarix(); }

void ExperienceOrb::Draw(const KamataEngine::Camera& camera) {
	if (isAlive_) {
		model_->Draw(worldTransform_, camera);
	}
}

KamataEngine::Vector3 ExperienceOrb::GetPosition() const { return worldTransform_.translation_; }

bool ExperienceOrb::IsAlive() const { return isAlive_; }

void ExperienceOrb::SetIsDead() { isAlive_ = false; }