#include "Enemy.h"
#include "EnemyState.h" // ★追加

Enemy::Enemy(const Vector3& position) { worldTransform.translation_ = position; }

Enemy::~Enemy() {
	delete model_;
	if (currentState_)
		delete currentState_; // ★追加：状態クラスの解放
}

void Enemy::Initialize() {
	model_ = Model::CreateFromOBJ("enemy");

	worldTransform.Initialize();
	worldTransform.scale_ = {2.0f, 2.0f, 2.0f};
	worldTransform.translation_.z = 0.0f;
	worldTransform.UpdateMatarix();

	// ★初期状態として「出現状態」をセットする
	ChangeState(new EnemyStateSpawn());
}

void Enemy::Update(const Vector3& playerPosition) {
	if (isDead_)
		return;

	// ★★★ スライドの条件3：ポリモーフィズムによる状態の関数呼び出し ★★★
	if (currentState_) {
		currentState_->Update(this, playerPosition);
	}

	// 行動（移動など）が適用された後にマトリクスを更新
	worldTransform.UpdateMatarix();
	worldTransform.TransferMatrix();
}

// ★状態遷移のための関数
void Enemy::ChangeState(EnemyState* newState) {
	if (currentState_) {
		delete currentState_; // 古い状態を削除
	}
	currentState_ = newState;
	if (currentState_) {
		currentState_->Enter(this); // 新しい状態の初期化
	}
}

void Enemy::Draw(const Camera& camera) {
	if (isDead_)
		return;

	if (model_) {
		model_->Draw(worldTransform, camera);
	}
}

void Enemy::TakeDamage(int damage) {
	if (isDead_)
		return;

	currentHp_ -= damage;
	if (currentHp_ <= 0) {
		currentHp_ = 0;
		isDead_ = true;
	}
}