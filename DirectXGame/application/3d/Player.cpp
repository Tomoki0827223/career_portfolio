#include "Player.h"
#include "KamataEngine.h"
#include <random>
#include <cmath>

using namespace KamataEngine;

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

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

	// ★修正: HPとスキルレベルのリセットを追加 ★
	currentHp_ = kMaxHp_;
	bookLevel_ = 0;
	bulletLevel_ = 0;
	wineLevel_ = 0;
	boomerangLevel_ = 0;
	minionLevel_ = 0;
	missileLevel_ = 0;
	// ------------------------------------
}

void Player::Update() {
	const float speed = 0.5f;
	Vector3 moveVector = {0.0f, 0.0f, 0.0f};

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
		worldTransform.translation_.y += speed;
		moveVector.y += 1.0f; // 方向ベクトルを記録
	}
	if (input_->PushKey(DIK_S)) {
		worldTransform.translation_.y -= speed;
		moveVector.y -= 1.0f; // 方向ベクトルを記録
	}
	if (input_->PushKey(DIK_A)) {
		worldTransform.translation_.x -= speed;
		moveVector.x -= 1.0f; // 方向ベクトルを記録
	}
	if (input_->PushKey(DIK_D)) {
		worldTransform.translation_.x += speed;
		moveVector.x += 1.0f; // 方向ベクトルを記録
	}

	// ★ プレイヤーの旋回処理 (ここから追加) ★
	// 移動ベクトルがある場合にのみ回転
	if (moveVector.x != 0.0f || moveVector.y != 0.0f) {
		// Y軸回転角度を計算 (Yaw): atan2(X成分, Z成分)
		// 現在のコードではW/SでY軸を動かしていますが、3Dモデルの正面はZ軸が基準となることが多いため、
		// 移動Y成分をZ成分として扱い、Y軸周りの回転を計算します。
		float targetRotationY = std::atan2(moveVector.x, moveVector.y);

		// 現在のY軸回転角度を取得
		float currentRotationY = worldTransform.rotation_.y;

		// 角度の差分を計算
		float diff = targetRotationY - currentRotationY;

		// 角度の最短経路を計算 (最短距離で回転させるための処理: -PI から PI の範囲に正規化)
		if (diff > M_PI) {
			diff -= 2.0f * M_PI;
		} else if (diff < -M_PI) {
			diff += 2.0f * M_PI;
		}

		// 補間（緩やかに回転させる）
		const float rotateSpeed = 0.2f; // 回転速度 (0.0f〜1.0fで調整してください)
		currentRotationY += diff * rotateSpeed;

		// 回転を適用
		worldTransform.rotation_.y = currentRotationY;
	}
	// ---------------------------------------------

	// SPACEキーで攻撃
	if (input_->TriggerKey(DIK_SPACE) && !isAttacking_) {
		isAttacking_ = true;
		attackTimer_ = kMaxAttackTime_;

		// ★追加: 攻撃開始時に効果音を鳴らす ★
		if (audio_ && attackSeHandle_ != 0) { //
			audio_->PlayWave(attackSeHandle_, false);
		}
	}

	// ★追加: スキル選択中は移動・攻撃処理をスキップ ★
	if (isSkillSelecting_) { //
		return;
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

// ★追加: HP回復メソッドの実装 ★
void Player::Heal(int amount) {
	currentHp_ += amount;
	if (currentHp_ > kMaxHp_) {
		currentHp_ = kMaxHp_;
	}
}
// ----------------------------------------