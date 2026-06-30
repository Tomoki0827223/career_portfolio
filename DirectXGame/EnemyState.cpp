#include "EnemyState.h"
#include "Enemy.h"

// 使っていない playerPos を /*playerPos*/ に変更して警告を回避
void EnemyStateSpawn::Update(Enemy* enemy, const KamataEngine::Vector3& /*playerPos*/) {
	timer_ += 1.0f;

	// 出現演出：だんだん大きくなる
	float scale = (timer_ / 30.0f) * 2.0f;
	if (scale > 2.0f)
		scale = 2.0f;
	enemy->SetScale({scale, scale, scale});

	// 30フレーム経ったら、自分自身で「追尾状態」に切り替える
	if (timer_ >= 30.0f) {
		enemy->ChangeState(new EnemyStateApproach());
	}
}

void EnemyStateApproach::Update(Enemy* enemy, const KamataEngine::Vector3& playerPos) {
	// プレイヤーへの追従
	Vector3 diff = playerPos - enemy->GetPosition();
	Vector3 direction = KamataEngine::Math::Normalize(diff);

	enemy->Move(direction * enemy->GetMoveSpeed());
}