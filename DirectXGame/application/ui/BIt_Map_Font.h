#pragma once
#include "KamataEngine.h"

class BIt_Map_Font {
public:
	void Initialize();
	void Set(int value);
	void Draw();

	// ★追加: 座標をセットする関数
	void SetPosition(const KamataEngine::Vector2& pos);

private:
	static constexpr int kNumDigits = 5;
	KamataEngine::Sprite* numberSprite_[kNumDigits] = {};
	uint32_t numberTextureHandle_ = 0;
	const KamataEngine::Vector2 kFontSize = {32.0f, 64.0f};

	// ★追加: 現在の座標を保持する変数
	KamataEngine::Vector2 position_ = {1100.0f, 10.0f}; // デフォルトは右上
};