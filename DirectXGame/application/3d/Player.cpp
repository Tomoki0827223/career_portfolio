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
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f}; // ★ プレイヤーのサイズを2倍に ★
}

void Player::Update() {
	const float speed = 0.5f;

	// 入力取得 (移動処理)
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

	// 攻撃入力 (追加)
	// スペースキーで攻撃開始
	if (input_->TriggerKey(DIK_SPACE) && !isAttacking_) {
		isAttacking_ = true;
		attackTimer_ = kMaxAttackTime_; // 10フレームの間、攻撃判定を有効にする
	}

	// 攻撃処理 (追加)
	if (isAttacking_) {
		attackTimer_--;
		if (attackTimer_ <= 0) {
			isAttacking_ = false;
		}
	}

	// 移動を反映
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();

	if (currentHp_ < 0) {
		currentHp_ = 0;
	}
}

void Player::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	modelPlayer_->Draw(worldTransform, camera_);

	Model::PostDraw();
}