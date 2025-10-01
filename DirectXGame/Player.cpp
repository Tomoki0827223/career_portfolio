#include "Player.h"
#include "KamataEngine.h"
#include <random>

using namespace KamataEngine;

Player::~Player() {

}

void Player::Initialize() {
	// 3Dモデルデータの生成
	// modelPlayer_ = Model::CreateFromOBJ("block_4"); // ★ 削除: GameSceneでロードするため

	input_ = KamataEngine::Input::GetInstance();

	worldTransform.Initialize();
}


void Player::Update() {
	const float speed = 0.5f;

	// 入力取得
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

void Player::Draw(const Camera& camera) {

	// Nullチェック (モデルが GameScene から設定されているか確認)
	if (modelPlayer_ == nullptr) {
		return;
	}

	modelPlayer_->Draw(worldTransform, camera);
}