#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScnce.h"
#include "TutorialScene.h" // ★追加
#include <Windows.h>

using namespace KamataEngine;

// Scene enumに Tutorial を追加
enum class Scene { Title, Tutorial, Game };

Scene scene = Scene::Tutorial;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// 初期化処理
	//  // エンジンの初期化

	KamataEngine::Initialize(L"Bike_Savaiver");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// main関数の前に
	TitleScnce* titleScnce = nullptr;
	TutorialScene* tutorialScnce = nullptr; // ★追加
	GameScene* gameScnce = nullptr;

	// タイトルシーンの初期化
	titleScnce = new TitleScnce();
	titleScnce->Initialize();

	// ★ チュートリアルシーンの初期化
	tutorialScnce = new TutorialScene();
	tutorialScnce->Initialize();

	// ゲームシーンの初期化
	gameScnce = new GameScene();
	gameScnce->Initialize();

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
				// タイトル演出完了後、Tutorialシーンへ移行
				scene = Scene::Tutorial;
			}
		} else if (scene == Scene::Tutorial) {
			tutorialScnce->Update();
			tutorialScnce->Draw();

			if (tutorialScnce->IsBackToTitle()) {
				scene = Scene::Title;
				// ★TitleScnceを再初期化 (Initialize内でisFinished_がリセットされる)
				titleScnce->Initialize();

				// ★追加: チュートリアルシーンも再初期化し、次のゲーム開始に備える
				tutorialScnce->Initialize(); // isFinished_とisBackToTitle_をリセット
			} else if (tutorialScnce->IsFinished()) {
				scene = Scene::Game;
			}

		} else if (scene == Scene::Game) {
			gameScnce->Update();
			gameScnce->Draw();
		}

		dxCommon->PostDraw();
	}

	// 終了処理
	delete gameScnce;
	gameScnce = nullptr;
	delete tutorialScnce; // ★追加
	tutorialScnce = nullptr;
	delete titleScnce;
	titleScnce = nullptr;

	// 終了処理
	KamataEngine::Finalize();
	return 0;
}
