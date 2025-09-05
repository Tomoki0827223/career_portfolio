#include "GameScene.h"

std::vector<Experience*> experiences;

void GameScene::SpawnExperiences(int count, int textureHandle, Model* model) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distX(0, 1280);
	std::uniform_real_distribution<float> distY(0, 720);

	for (int i = 0; i < count; ++i) {
		Vector2 pos = {distX(gen), distY(gen)};
		experiences.push_back(new Experience(textureHandle, pos, model));
	}
}

void GameScene::UpdateExperiences(const Vector3& playerPos) {
	for (auto& exp : experiences) {
		exp->Update();
		if (exp->IsCollected(playerPos, 30.0f)) {
			// プレイヤーに経験値加算
		}
	}
}

void GameScene::DrawExperiences() {
	for (auto& exp : experiences) {
		exp->Draw(camera_);
	}
}

GameScene::~GameScene() {
	delete stage_;
	delete player_;
	delete playerModel_;
	delete model_;
	delete graph_;
	delete font_;
	delete exp_;
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

	expModel_ = Model::CreateFromOBJ("cube");

	font_ = new BIt_Map_Font();
	font_->Initialize();

	expTextureHandle_ = TextureManager::Load("HPR.png");
	exp_ = Sprite::Create(expTextureHandle_, {100, 100});

	SpawnExperiences(10, expTextureHandle_, expModel_);
}

void GameScene::Update() {
	stage_->Update();

	player_->Update();

	UpdateExperiences(player_->GetPosition());

	graph_->Update();

	score_++;
	font_->SetScore(score_);
}

void GameScene::Draw() {
	// 3Dオブジェクト描画前処理
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());
	// 3Dモデルの描画
	player_->Draw(camera_);
	stage_->Draw(camera_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 2Dスプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 2Dスプライトの描画
	exp_->Draw();
	DrawExperiences();
	graph_->Draw();
	font_->Draw();
	// 2Dスプライト描画後処理
	Sprite::PostDraw();
}