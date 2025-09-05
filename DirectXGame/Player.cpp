#include "Player.h"
#include "KamataEngine.h"
#include <random>

using namespace KamataEngine;


Player::~Player() {

	delete modelPlayer_;
	modelPlayer_ = nullptr;
}

void Player::Initialize() {
	// 3Dモデルデータの生成
	modelPlayer_ = Model::CreateFromOBJ("block_4");

	input_ = KamataEngine::Input::GetInstance();

	// カメラの初期化
	camera_.Initialize();

	worldTransform.Initialize();
}

void Player::Update() {
	const float speed = 0.5f;

	if (input_->PushKey(DIK_W)) {
		worldTransform.translation_.y += speed;
	}
	if (input_->PushKey(DIK_S)) {
		worldTransform.translation_.y -= speed;
	}
	if (input_->PushKey(DIK_A)) {
		worldTransform.translation_.x -= speed;
	}
	if (input_->PushKey(DIK_D)) {
		worldTransform.translation_.x += speed;
	}

	// 移動を反映
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

KamataEngine::Vector3 Player::GetPosition() const { return worldTransform.translation_; }

void Player::AddExperience(int exp) {
	experience_ += exp;
	if (experience_ >= experienceToNextLevel_) {
		level_++;
		experience_ -= experienceToNextLevel_;
		experienceToNextLevel_ = static_cast<int>(experienceToNextLevel_ * 1.5); // 次のレベルまでの必要経験値を増やす
	}
}

int Player::GetLevel() const { return level_; }

int Player::GetExperience() const { return experience_; }

void Player::Draw() {
	// DirectXCommon* dxCommon = DirectXCommon::GetInstance(); // 削除
	// dxCommon->ClearDepthBuffer();                           // 削除
	// Model::PreDraw();                                       // 削除

	modelPlayer_->Draw(worldTransform, camera_);

	// Model::PostDraw();                                      // 削除
}