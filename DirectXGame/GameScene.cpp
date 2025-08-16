#include "GameScene.h"

GameScene::~GameScene() {
	delete stage_;
	delete player_;
	delete playerModel_;
	delete model_;
	delete graph_;
	delete font_;
}

void GameScene::Initialize() {


	graph_ = new Graph();
	graph_->Initialize();

	stage_ = new Stage();
	stage_->Initialize();

	stage_->Update();

	// カメラの初期化
	camera_.Initialize();

	worldTransform.Initialize();

	player_ = new Player();
	player_->Initialize();
	playerModel_ = Model::CreateFromOBJ("block_4");

	font_ = new BIt_Map_Font();

	font_->Initialize();
}

void GameScene::Update() {
	stage_->Update();

	player_->Update();

	graph_->Update();

	score_++;
	font_->Set(score_);
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Model::PreDraw();

	stage_->Draw();

	player_->Draw();

	graph_->Draw();

	font_->Draw();

	Model::PostDraw();
	Sprite::PostDraw();
}