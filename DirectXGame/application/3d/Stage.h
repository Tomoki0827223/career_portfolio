#pragma once
#include "KamataEngine.h"
#include <cstdint>

using namespace KamataEngine;

class Stage {
public:
	~Stage();
	void Initialize();
	void Update();
	void Draw();

private:
	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* sprite_ = nullptr;
	KamataEngine::Sprite* sprite2_ = nullptr;

	Input* input_ = nullptr;

	float scrollSpeed_ = 4.0f;
};