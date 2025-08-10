#include "Stage.h"
#include "GameScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

Stage::~Stage() {
	delete sprite_;
	delete sprite2_;
}

void Stage::Initialize() {

	input_ = KamataEngine::Input::GetInstance();

	textureHandle_ = TextureManager::Load("scroll_bg.png");

	sprite_ = Sprite::Create(textureHandle_, {0, 0});

	sprite2_ = Sprite::Create(textureHandle_, {1280, 0});
}

void Stage::Update() {
	// 1枚目
	Vector2 pos1 = sprite_->GetPosition();
	pos1.x -= scrollSpeed_;
	if (pos1.x <= -1280) {
		pos1.x = 1280 - scrollSpeed_; // 2枚目の右端にピッタリつなげる
	}
	sprite_->SetPosition(pos1);

	// 2枚目
	Vector2 pos2 = sprite2_->GetPosition();
	pos2.x -= scrollSpeed_;
	if (pos2.x <= -1280) {
		pos2.x = 1280 - scrollSpeed_;
	}
	sprite2_->SetPosition(pos2);
}

void Stage::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());

	sprite_->Draw();
	sprite2_->Draw();

	Sprite::PostDraw();
}