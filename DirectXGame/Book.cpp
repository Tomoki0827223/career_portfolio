#include "Book.h"
#include <cmath> // std::cos, std::sin, M_PIのためにインクルード

// M_PIが定義されていない環境のために手動で定義
// この定義がないとM_PIを使用できません。
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

Book::Book() {}

Book::~Book() { delete model_; }

void Book::Initialize() {
	// モデルとして"book"を使用
	model_ = Model::CreateFromOBJ("book");

	worldTransform.Initialize();
	worldTransform.scale_ = {1.0f, 1.0f, 1.0f};

	// 初期角度をランダムに設定 (複数生成時に重ならないように)
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	// ★修正: Math::PI -> M_PI に変更★
	std::uniform_real_distribution<float> dist(0.0f, 2.0f * (float)M_PI);
	currentAngle_ = dist(engine);

	worldTransform.translation_.z = 0.0f;
	worldTransform.UpdateMatarix();
}

/**
 * @brief Bookの更新。プレイヤーの周りを回転するロジックを実装。
 * @param playerPosition プレイヤーの現在位置
 */
void Book::Update(const Vector3& playerPosition) {
	// 角度を更新
	currentAngle_ += kRotationSpeed;
	// ★修正: Math::PI -> M_PI に変更★
	if (currentAngle_ > 2.0f * (float)M_PI) {
		currentAngle_ -= 2.0f * (float)M_PI; // ★修正: Math::PI -> M_PI に変更★
	}

	// プレイヤーを中心に円運動の座標を計算
	worldTransform.translation_.x = playerPosition.x + kRotationRadius * std::cos(currentAngle_);
	worldTransform.translation_.y = playerPosition.y + kRotationRadius * std::sin(currentAngle_);
	worldTransform.translation_.z = 0.0f;

	// 常にプレイヤーの方向に向くように回転（見栄えのため）
	// ★修正: Math::PI -> M_PI に変更★
	worldTransform.rotation_.z = currentAngle_ + (float)M_PI / 2.0f;

	// 移動を反映
	worldTransform.TransferMatrix();
	worldTransform.UpdateMatarix();
}

void Book::Draw(const Camera& camera) {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	dxCommon->ClearDepthBuffer();
	Model::PreDraw();

	model_->Draw(worldTransform, camera);

	Model::PostDraw();
}