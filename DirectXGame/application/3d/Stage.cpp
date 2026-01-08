#include "Stage.h"
#include <cmath>

Stage::~Stage() { delete model_; }

void Stage::Initialize() {
	// 床用のモデル（"floor" または "cube"）
	model_ = Model::CreateFromOBJ("tile");

	for (int x = 0; x < kFloorCountX; x++) {
		for (int y = 0; y < kFloorCountY; y++) {
			worldTransforms_[x][y].Initialize();

			// 床のサイズに合わせてスケール調整
			// もし1mのCubeなら、平たく伸ばして大きくする
			// worldTransforms_[x][y].scale_ = {60.0f, 60.0f, 0.1f};

			// もし60mの板モデルならそのまま
			worldTransforms_[x][y].scale_ = {1.0f, 1.0f, 1.0f};

			// Z軸（奥行き）はプレイヤーの後ろに配置
			worldTransforms_[x][y].translation_.z = 5.0f;
		}
	}
}

void Stage::Update(const Vector3& playerPos) {
	// プレイヤーが「何枚目の床」にいるかを計算 (X軸とY軸)
	int currentX = static_cast<int>(std::floor(playerPos.x / kFloorSize));
	int currentY = static_cast<int>(std::floor(playerPos.y / kFloorSize));

	// 3x3枚の床をプレイヤーの周囲に配置
	for (int x = 0; x < kFloorCountX; x++) {
		for (int y = 0; y < kFloorCountY; y++) {
			// -1, 0, +1 のオフセット
			int offsetX = x - 1;
			int offsetY = y - 1;

			// 配置すべき座標
			float targetX = (currentX + offsetX) * kFloorSize;
			float targetY = (currentY + offsetY) * kFloorSize;

			worldTransforms_[x][y].translation_.x = targetX;
			worldTransforms_[x][y].translation_.y = targetY;

			// 行列更新
			worldTransforms_[x][y].UpdateMatarix();
			worldTransforms_[x][y].TransferMatrix();
		}
	}
}

void Stage::Draw(const Camera& camera) {
	// 9枚分描画
	for (int x = 0; x < kFloorCountX; x++) {
		for (int y = 0; y < kFloorCountY; y++) {
			model_->Draw(worldTransforms_[x][y], camera);
		}
	}
}