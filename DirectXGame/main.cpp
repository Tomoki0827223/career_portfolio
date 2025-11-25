#include "GameOverScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScnce.h"
#include "TutorialScene.h"
#include <Windows.h>

using namespace KamataEngine;


// Scene enumに Tutorial と GameOver を追加
enum class Scene { Title, Tutorial, Game, GameOver };

Scene scene = Scene::Tutorial;

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

			// ゲームオーバー判定とシーン遷移
			if (gameScnce->IsGameOver()) {
				scene = Scene::GameOver;
				gameOverScene->Initialize(); // ゲームオーバーシーンを初期化

				// ★重要: GameSceneのisGameOver_フラグをリセット
				// これがないと次のGameScene::Initialize()が呼ばれる前に再度IsGameOver()がtrueを返し続ける可能性がある
				gameScnce->ResetGameOverFlag();
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