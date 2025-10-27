#include "Book.h"

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
	std::uniform_real_distribution<float> dist(0.0f, 2.0f * (float)Math::PI);
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
	if (currentAngle_ > 2.0f * (float)Math::PI) {
		currentAngle_ -= 2.0f * (float)Math::PI;
	}

	// プレイヤーを中心に円運動の座標を計算
	worldTransform.translation_.x = playerPosition.x + kRotationRadius * std::cos(currentAngle_);
	worldTransform.translation_.y = playerPosition.y + kRotationRadius * std::sin(currentAngle_);
	worldTransform.translation_.z = 0.0f;

	// 常にプレイヤーの方向に向くように回転（見栄えのため）
	worldTransform.rotation_.z = currentAngle_ + (float)Math::PI / 2.0f;

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