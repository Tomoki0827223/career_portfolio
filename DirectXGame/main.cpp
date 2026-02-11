#include "GameOverScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScnce.h"
#include "TutorialScene.h"
#include <Windows.h>
#include "TextureConverter.h"
#include <filesystem>
#include <vector>
#include <string>

using namespace KamataEngine;

namespace fs = std::filesystem;

void ConvertAllImages() {
	TextureConverter converter;
	std::string rootPath = "Resources";

	// フォルダが存在するかまずチェック
	if (!fs::exists(rootPath)) {
		OutputDebugStringA("Error: 'Resources' folder not found!\n");
		return;
	}

	for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();
			if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
				std::string filePath = entry.path().string();

				// 変換開始をログに出す
				std::string log = "Converting: " + filePath + "\n";
				OutputDebugStringA(log.c_str());

				converter.ConvertTextureWICToDDS(filePath);
			}
		}
	}
	OutputDebugStringA("Conversion Process Finished.\n");
}

// Scene enumに Tutorial と GameOver を追加
enum class Scene { Title, Tutorial, Game, GameOver };

Scene scene = Scene::Title;
//Scene scene = Scene::Game;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// 初期化処理
	KamataEngine::Initialize(L"LE3C_19_ムラタ_トモキ_Bike_Savaiver");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// main関数の前に
	TitleScnce* titleScnce = nullptr;
	TutorialScene* tutorialScnce = nullptr;
	GameScene* gameScnce = nullptr;
	GameOverScene* gameOverScene = nullptr;

	// タイトルシーンの初期化
	titleScnce = new TitleScnce();
	titleScnce->Initialize();

	// ★ チュートリアルシーンの初期化
	tutorialScnce = new TutorialScene();
	tutorialScnce->Initialize();

	// ゲームシーンの初期化
	gameScnce = new GameScene();
	gameScnce->Initialize();

	// ★ ゲームオーバーシーンの初期化 (追加)
	gameOverScene = new GameOverScene();
	gameOverScene->Initialize();

	ConvertAllImages();

	// メインループ
	while (true) {

		if (KamataEngine::Update()) {
			break;
		}


		dxCommon->PreDraw();

		// シーンごとに処理を分岐
		if (scene == Scene::Title) {
			titleScnce->Update();
			titleScnce->Draw();
			if (titleScnce->IsSelectFinished()) {
				// TitleSceneの終了 (Start選択)
				scene = Scene::Tutorial;
				// ※ ここではTutorialSceneのInitialize()は呼ばない（Tutorial::Update()内でリセットするため）
			}
		} else if (scene == Scene::Tutorial) {
			tutorialScnce->Update();
			tutorialScnce->Draw();

			if (tutorialScnce->IsBackToTitle()) {
				scene = Scene::Title;
				titleScnce->Initialize();

				// ★修正: フラグをリセットしてループを防ぐ
				// TutorialSceneのInitialize()を呼び出し、フラグを確実にリセット
				tutorialScnce->Initialize();
			} else if (tutorialScnce->IsFinished()) {
				scene = Scene::Game;
				gameScnce->Initialize();

				// ★修正: フラグをリセットしてループを防ぐ
				// TutorialSceneのInitialize()を呼び出し、フラグを確実にリセット
				tutorialScnce->Initialize();
			}

		} else if (scene == Scene::Game) {
			gameScnce->Update();
			gameScnce->Draw();

			// ★修正: ゲームオーバー判定
			if (gameScnce->IsGameOver()) {
				// プレイヤーの死亡演出（タイマー）が終了したかチェックする
				if (gameScnce->GetPlayer()->GetDeadTimer() >= gameScnce->GetPlayer()->GetMaxDeadTime()) {
					int finalScore = gameScnce->GetScore();

					scene = Scene::GameOver;
					gameOverScene->Initialize();
					gameOverScene->SetResultScore(finalScore);

					gameScnce->ResetGameOverFlag();
				}
			}

		} else if (scene == Scene::GameOver) {
			gameOverScene->Update();
			gameOverScene->Draw();

			if (gameOverScene->IsFinished()) {
				if (gameOverScene->IsRetrySelected()) {
					
					// リトライ: GameSceneを再初期化してGameSceneへ
					scene = Scene::Game;
					gameScnce->Initialize();

				} else {
					
					// タイトルへ: TitleSceneへ
					scene = Scene::Title;
					titleScnce->Initialize();
				}
				// GameOverSceneのisFinished_はInitializeでリセットされている
			}
		}

		dxCommon->PostDraw();
	}

	// 終了処理
	delete gameOverScene;
	gameOverScene = nullptr;
	delete gameScnce;
	gameScnce = nullptr;
	delete tutorialScnce;
	tutorialScnce = nullptr;
	delete titleScnce;
	titleScnce = nullptr;

	// 終了処理
	KamataEngine::Finalize();
	return 0;
}