#pragma once

#include <KamataEngine.h>

using namespace KamataEngine;

class Experience {
public:
	Experience(int textureHandle, Vector2 position, Model* model = nullptr);
	void Update();
	void Draw();
	Vector2 GetPosition() const;
	bool IsCollected(const Vector2& playerPos, float collectRadius);

private:
	Sprite* sprite_;
	Vector2 position_;
	bool collected_;
	Model* model_; // 追加
};