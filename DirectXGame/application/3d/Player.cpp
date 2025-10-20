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
			// 攻撃終了時にスケールをリセット
			worldTransform.scale_ = {2.0f, 2.0f, 2.0f}; // ★ 攻撃終了時のリセット ★
		}

		// ★ プレイヤー攻撃時の視覚的フィードバック: スケールアニメーション ★
		// 攻撃フレーム数が 10 のため、前半 5f で拡大、後半 5f で縮小
		const float kDefaultScale = 2.0f;
		const float kMaxScale = 3.0f;              // より目立つように 3.0f に拡大
		const int kHalfTime = kMaxAttackTime_ / 2; // 5フレーム

		float currentScale = kDefaultScale;

		if (attackTimer_ >= kHalfTime) {
			// 攻撃前半 (10 -> 6): 2.0f -> 3.0f に拡大
			// t_expand: 0.0f (attackTimer_=10) -> 1.0f (attackTimer_=6)
			float t_expand = 1.0f - (float)(attackTimer_ - kHalfTime) / kHalfTime;
			currentScale = kDefaultScale + (kMaxScale - kDefaultScale) * t_expand;
		} else {
			// 攻撃後半 (5 -> 1): 3.0f -> 2.0f に縮小
			// t_shrink: 1.0f (attackTimer_=5) -> 0.0f (attackTimer_=1)
			float t_shrink = (float)attackTimer_ / kHalfTime;
			currentScale = kDefaultScale + (kMaxScale - kDefaultScale) * t_shrink;
		}

		worldTransform.scale_ = {currentScale, currentScale, currentScale};

	} else {
		// 攻撃中でない場合はデフォルトのスケールを維持 (初期化時の 2.0f に戻す)
		worldTransform.scale_ = {2.0f, 2.0f, 2.0f};
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