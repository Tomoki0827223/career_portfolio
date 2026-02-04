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

	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f}; // ★ プレイヤーのサイズを2倍に ★

	// ★追加: モデルをX軸で90度倒して画面（XY平面）に合わせる
	worldTransform.rotation_.x = (float)M_PI / 2.0f;

	// ★修正: HPとスキルレベルのリセットを追加 ★
	currentHp_ = kMaxHp_;
	bookLevel_ = 0;
	bulletLevel_ = 0;
	wineLevel_ = 0;
	boomerangLevel_ = 0;
	minionLevel_ = 0;
	missileLevel_ = 0;
}

void Player::Update() {
	const float speed = 0.5f;
	Vector3 moveVector = {0.0f, 0.0f, 0.0f};

	// ★ 死亡している場合は、移動・攻撃処理をスキップし、死亡モーションのみ実行 ★
	if (isDead_) {
		deadTimer_++;

		if (deadTimer_ <= kMaxDeadTime_) {
			float t = (float)deadTimer_ / kMaxDeadTime_;
			float currentScale = 2.0f * (1.0f - t);

			worldTransform.rotation_.x += 0.8f;
			worldTransform.rotation_.y += 0.4f;
			worldTransform.scale_ = {currentScale, currentScale, currentScale};
		} else {
			worldTransform.scale_ = {0.0f, 0.0f, 0.0f};
		}

		// ★修正: UpdateMatrix (aを抜く)
		worldTransform.UpdateMatarix();
		worldTransform.TransferMatrix();
		return;
	}

	// ★追加: スキル選択中は移動・攻撃処理をスキップ ★
	if (isSkillSelecting_) {
		return;
	}

	if (isAutoMode_) {
		// --- 自動操作ロジック ---

		// 1. Y位置をかなり下に固定（タイトルロゴを完全に避ける）
		// -15.0f 程度まで下げるとかなり下になります
		float targetY = -15.0f;
		worldTransform.translation_.y += (targetY - worldTransform.translation_.y) * 0.1f;

		// 2. Z軸は一切いじらない（加算処理を削除）
		// worldTransform.translation_.z はそのまま保持

		// 3. 左右に少し揺らす演出（その場に留まりすぎないように）
		static float aiTimer = 0;
		aiTimer += 0.03f;
		worldTransform.translation_.x += std::sin(aiTimer) * 0.1f;

		worldTransform.UpdateMatarix();
	} else {

		// --- コントローラー情報の取得 ---
		XINPUT_STATE joyState;
		bool hasJoy = input_->GetJoystickState(0, joyState);

		// SPACEキー または RT(右トリガー) が半分以上押し込まれたら
		bool attackTrigger = input_->TriggerKey(DIK_SPACE);
		if (hasJoy && joyState.Gamepad.bRightTrigger > 128) {
			attackTrigger = true;
		}


		if (attackTrigger && !isAttacking_) {
			isAttacking_ = true;
			attackTimer_ = kMaxAttackTime_;
			if (audio_ && attackSeHandle_ != 0) {
				audio_->PlayWave(attackSeHandle_, false);
			}
		}

		// --- 移動処理 (キーボード + 十字キー + 左スティック) ---

		// 上方向
		if (input_->PushKey(DIK_W) || (hasJoy && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)) || (hasJoy && joyState.Gamepad.sThumbLY > 10000)) {
			worldTransform.translation_.y += speed;
			moveVector.y += 1.0f;
		}
		// 下方向
		if (input_->PushKey(DIK_S) || (hasJoy && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)) || (hasJoy && joyState.Gamepad.sThumbLY < -10000)) {
			worldTransform.translation_.y -= speed;
			moveVector.y -= 1.0f;
		}
		// 左方向
		if (input_->PushKey(DIK_A) || (hasJoy && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)) || (hasJoy && joyState.Gamepad.sThumbLX < -10000)) {
			worldTransform.translation_.x -= speed;
			moveVector.x -= 1.0f;
		}
		// 右方向
		if (input_->PushKey(DIK_D) || (hasJoy && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)) || (hasJoy && joyState.Gamepad.sThumbLX > 10000)) {
			worldTransform.translation_.x += speed;
			moveVector.x += 1.0f;
		}

		// --- 旋回処理（既存の moveVector ロジックがそのまま使えます） ---
		if (moveVector.x != 0.0f || moveVector.y != 0.0f) {
			float targetRotationY = std::atan2(moveVector.x, moveVector.y);
			float currentRotationY = worldTransform.rotation_.y;
			float diff = targetRotationY - currentRotationY;
			if (diff > (float)M_PI)
				diff -= 2.0f * (float)M_PI;
			else if (diff < -(float)M_PI)
				diff += 2.0f * (float)M_PI;
			worldTransform.rotation_.y += diff * 0.2f;
		}

		if (attackTrigger && !isAttacking_) {
			isAttacking_ = true;
			attackTimer_ = kMaxAttackTime_;
			if (audio_ && attackSeHandle_ != 0) {
				audio_->PlayWave(attackSeHandle_, false);
			}
		}

		// --- 攻撃処理・スケール演出 (ここはそのまま) ---
		if (input_->TriggerKey(DIK_SPACE) && !isAttacking_) {
			isAttacking_ = true;
			attackTimer_ = kMaxAttackTime_;
			if (audio_ && attackSeHandle_ != 0) {
				audio_->PlayWave(attackSeHandle_, false);
			}
		}

		// 攻撃中の演出
		if (isAttacking_) {
			attackTimer_--;
			if (attackTimer_ <= 0) {
				isAttacking_ = false;
				worldTransform.scale_ = {2.0f, 2.0f, 2.0f};
			} else {
				const float kDefaultScale = 2.0f;
				const float kMaxScale = 3.0f;
				const int kHalfTime = kMaxAttackTime_ / 2;
				float currentScale = kDefaultScale;

				if (attackTimer_ >= kHalfTime) {
					float t_expand = 1.0f - (float)(attackTimer_ - kHalfTime) / kHalfTime;
					currentScale = kDefaultScale + (kMaxScale - kDefaultScale) * t_expand;
				} else {
					float t_shrink = (float)attackTimer_ / kHalfTime;
					currentScale = kDefaultScale + (kMaxScale - kDefaultScale) * t_shrink;
				}
				worldTransform.scale_ = {currentScale, currentScale, currentScale};
			}
		} else {
			worldTransform.scale_ = {2.0f, 2.0f, 2.0f};
		}

		// ★修正: UpdateMatrix (aを抜く)
		worldTransform.UpdateMatarix();
	} // ここが else (isAutoMode_) の閉じカッコ

	// 最後に共通で行列転送
	worldTransform.TransferMatrix();

	if (currentHp_ < 0) {
		currentHp_ = 0;
	}
}

void Player::Draw(const Camera& camera) {
	Model::PreDraw();

	// ★ 変更: メンバ変数の camera_ ではなく、引数の camera を使う
	modelPlayer_->Draw(worldTransform, camera);

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