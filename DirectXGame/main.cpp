#include "GameOverScene.h"
#include "GameClearScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScnce.h"
#include "TutorialScene.h"
#include <Windows.h>

using namespace KamataEngine;


// Scene enumに Tutorial と GameOver を追加
enum class Scene { Title, Tutorial, Game, GameOver, GameClear };

Scene scene = Scene::Game;

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
	GameClearScene* gameClearScene = nullptr;

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

	gameClearScene = new GameClearScene();
	gameClearScene->Initialize();


	// メインループ
	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

		dxCommon->PreDraw();

		// シーンごとに処理を分岐
		if (scene == Scene::Title) {
			// ... (Title, Tutorialのロジックは省略)
		} else if (scene == Scene::Tutorial) {
			// ... (Tutorialのロジックは省略)
		} else if (scene == Scene::Game) {
			gameScnce->Update();
			gameScnce->Draw();

			// ゲームクリア判定とシーン遷移 (★★★ 追加 ★★★)
			if (gameScnce->IsGameClear()) {
				scene = Scene::GameClear;
				gameClearScene->Initialize(); // ゲームクリアシーンを初期化

				// ★重要: GameSceneのisGameClear_フラグをリセット
				gameScnce->ResetGameClearFlag();

			} else if (gameScnce->IsGameOver()) { // ゲームオーバー判定とシーン遷移
				scene = Scene::GameOver;
				gameOverScene->Initialize();

				// ★重要: GameSceneのisGameOver_フラグをリセット
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
			}
			// ★★★ 追加: ゲームクリアシーンの処理 ★★★
		} else if (scene == Scene::GameClear) {
			gameClearScene->Update();
			gameClearScene->Draw();

			if (gameClearScene->IsFinished()) {
				if (gameClearScene->IsRetrySelected()) {
					// リトライ: GameSceneを再初期化してGameSceneへ
					scene = Scene::Game;
					gameScnce->Initialize();
				} else {
					// タイトルへ: TitleSceneへ
					scene = Scene::Title;
					titleScnce->Initialize();
				}
			}
		}

		dxCommon->PostDraw();
	}

	// 終了処理
	// ★★★ 追加: ゲームクリアシーンの解放 ★★★
	delete gameClearScene;
	gameClearScene = nullptr;
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