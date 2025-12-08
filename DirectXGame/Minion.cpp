#include "Minion.h"
#include "math/MathUtility.h"

const float PI = 3.14159265358979323846f;

Minion::Minion(int index, int total) : index_(index), total_(total) {}

Minion::~Minion() { delete model_; }


void Minion::Initialize() {
	model_ = Model::CreateFromOBJ("Minion"); // playerモデルを使用
	worldTransform.Initialize();
	worldTransform.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform.translation_.z = 0.0f;
	worldTransform.UpdateMatarix();
}

void Minion::Update(const Vector3& playerPosition) {

	// 1. 周回目標位置を計算
	// プレイヤーを中心に、等間隔に配置する円周上の位置
	// 【修正】Math::PI -> PI
	float angle = (float)index_ / total_ * PI * 2.0f;
	// float rotationSpeed = 0.01f; // 【修正】未使用変数C4189/C2220のエラー回避のため削除
	// 時間経過で回転させる
	// 【修正】KamataEngine::Engine::GetInstance() が未定義のため、処理を削除
	// angle += KamataEngine::Engine::GetInstance()->GetGlobalGameTimer() * rotationSpeed;

	Vector3 targetPos;
	targetPos.x = std::cos(angle) * kFollowRadius + playerPosition.x;
	targetPos.y = std::sin(angle) * kFollowRadius + playerPosition.y;
	targetPos.z = 0.0f;

	// 2. 目標位置に向かって追従
	Vector3 diff = targetPos - worldTransform.translation_;

	// 線形補間(Lerp)で滑らかに移動させる
	worldTransform.translation_ += diff * kFollowSpeed;

	// 3. 攻撃タイマー更新
	if (attackTimer_ > 0) {
		attackTimer_--;
	}

	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void Minion::Draw(const Camera& camera) {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();
	model_->Draw(worldTransform, camera);
	Model::PostDraw();
}