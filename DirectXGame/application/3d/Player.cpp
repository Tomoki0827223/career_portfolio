#include "Player.h"
#include "KamataEngine.h"
#include <random>

using namespace KamataEngine;

// ★ Player::GetInstance()の実装 (シングルトンパターン) がないとリンクエラーになるため、追加します。
// Player.hには宣言がありますが、ここでは定義がないため、簡略的な定義を追加します。
Player* Player::instance_ = nullptr;
Player* Player::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new Player();
	}
	return instance_;
}
// ★

Player::~Player() {

	delete modelPlayer_;
	modelPlayer_ = nullptr;
}

void Player::Initialize() {
	// 3Dモデルデータの生成
	modelPlayer_ = Model::CreateFromOBJ("player");

	input_ = KamataEngine::Input::GetInstance();

	// カメラの初期化
	camera_.Initialize();

	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f}; // ★ プレイヤーのサイズを2倍に ★

	// ★修正: HPのリセットを追加 (Player.hの定義 kMaxHP_ に合わせる) ★
	// スキルレベル (bookLevel_, bulletLevel_, wineLevel_) はPlayer.hに定義がないため削除
	currentHP_ = kMaxHP_;
	// ------------------------------------
}

void Player::Update() {
	// ★修正: 移動速度にスキルによる乗数を適用する ★
	const float baseSpeed = 0.5f;
	const float currentSpeed = baseSpeed * moveSpeedMultiplier_;

	// ★ 死亡している場合は、移動・攻撃処理をスキップし、死亡モーションのみ実行 ★
	if (isDead_) {
		deadTimer_++;

		if (deadTimer_ <= kMaxDeadTime_) {
			// t: 0.0f -> 1.0f へ線形に変化
			float t = (float)deadTimer_ / kMaxDeadTime_;

			// 2.0f (初期スケール) から 0.0f へ縮小
			float currentScale = 2.0f * (1.0f - t);

			// モデルを回転させる
			worldTransform.rotation_.x += 0.8f;
			worldTransform.rotation_.y += 0.4f;

			worldTransform.scale_ = {currentScale, currentScale, currentScale};

		} else {
			// モーション終了後は、描画されないようにスケールを0にする
			worldTransform.scale_ = {0.0f, 0.0f, 0.0f};
		}

		// 死亡モーションの反映と終了
		worldTransform.TransferMatrix();
		worldTransform.UpdateMatarix();
		return; // 死亡時は以降の処理をスキップ
	}

	// 入力取得 (移動処理)
	if (input_->PushKey(DIK_W)) {
		worldTransform.translation_.y += currentSpeed; // ★修正: currentSpeedを使用 ★
	}
	if (input_->PushKey(DIK_S)) {
		worldTransform.translation_.y -= currentSpeed; // ★修正: currentSpeedを使用 ★
	}
	if (input_->PushKey(DIK_A)) {
		worldTransform.translation_.x -= currentSpeed; // ★修正: currentSpeedを使用 ★
	}
	if (input_->PushKey(DIK_D)) {
		worldTransform.translation_.x += currentSpeed; // ★修正: currentSpeedを使用 ★
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
		// ★ 死亡時はこの処理をスキップするために、上に isDead_ のチェックを追加しました ★
		worldTransform.scale_ = {2.0f, 2.0f, 2.0f};
	}

	// 移動を反映
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();

	// ★修正: 変数名を currentHP_ に修正 ★
	if (currentHP_ < 0) {
		currentHP_ = 0;
	}
}

void Player::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	modelPlayer_->Draw(worldTransform, camera_);

	Model::PostDraw();
}

// ★修正: HP回復メソッドの実装 (変数名を currentHP_ に修正) ★
void Player::Heal(int amount) {
	currentHP_ += amount;
	if (currentHP_ > kMaxHP_) {
		currentHP_ = kMaxHP_;
	}
}
// ----------------------------------------