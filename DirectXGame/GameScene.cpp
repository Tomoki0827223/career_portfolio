#include "GameScene.h"

Camera camera_;

std::vector<Experience*> experiences;

void SpawnExperiences(int count, int textureHandle, Model* model) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distX(0, 1280);
	std::uniform_real_distribution<float> distY(0, 720);

	for (int i = 0; i < count; ++i) {
		Vector2 pos = {distX(gen), distY(gen)};
		experiences.push_back(new Experience(textureHandle, pos, model));
	}
}

void UpdateExperiences(const Vector3& playerPos) {
	for (auto& exp : experiences) {
		exp->Update();
		if (exp->IsCollected(playerPos, 30.0f)) {
			// プレイヤーに経験値加算
		}
	}
}

void DrawExperiences() {
	for (auto& exp : experiences) {
		exp->Draw();
	}
}

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

	expTextureHandle_ = TextureManager::Load("HPR.png");
	exp_ = Sprite::Create(expTextureHandle_, {100, 100});

	SpawnExperiences(10, expTextureHandle_, playerModel_);
}

void GameScene::Update() {
	stage_->Update();

	player_->Update();

	graph_->Update();

	score_++;
	font_->Set(score_);

	UpdateExperiences();
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	Model::PreDraw();

	stage_->Draw();

	player_->Draw();

	graph_->Draw();

	font_->Draw();

	DrawExperiences();

	Model::PostDraw();
	Sprite::PostDraw();
}