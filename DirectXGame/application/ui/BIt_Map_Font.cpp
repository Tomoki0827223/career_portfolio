#include "BIt_Map_Font.h"
#include <cmath>
#include <string>

using namespace KamataEngine;

// BIt_Map_Font.cpp の Initialize を修正して引数で座標を受け取れるようにするか、
// Drawの前に座標を再設定できるようにします。

void BIt_Map_Font::SetPosition(const Vector2& pos) {
	position_ = pos;
	for (int i = 0; i < kNumDigits; ++i) {
		// ★ここが重要: スプライトが存在するかチェック★
		if (numberSprite_[i] != nullptr) {
			numberSprite_[i]->SetPosition({position_.x + kFontSize.x * i, position_.y});
		}
	}
}

void BIt_Map_Font::Initialize() {
	numberTextureHandle_ = TextureManager::Load("number.png");
	for (int i = 0; i < kNumDigits; ++i) {
		// 初期位置（右上）
		numberSprite_[i] = Sprite::Create(numberTextureHandle_, {position_.x + kFontSize.x * i, position_.y});
		numberSprite_[i]->SetSize(kFontSize);
		numberSprite_[i]->SetTextureRect({0, 0}, kFontSize);
	}
}

void BIt_Map_Font::Set(int value) {
	for (int i = 0; i < kNumDigits; ++i) {
		int digit = (value / static_cast<int>(std::pow(10, kNumDigits - 1 - i))) % 10;

		Vector2 texBase = {kFontSize.x * digit, 0};
		numberSprite_[i]->SetTextureRect(texBase, kFontSize);
	}
}

void BIt_Map_Font::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Sprite::PreDraw(dxCommon->GetCommandList());

	for (int i = 0; i < kNumDigits; ++i) {
		numberSprite_[i]->Draw();
	}

	Sprite::PostDraw();
}