#include "BaseScene.h"
#include "GameOverScene.h" // 切り替え先をnewするために必要
#include "GameScene.h"     // 切り替え先をnewするために必要
#include "KamataEngine.h"
#include "TitleScnce.h"    // 最初のシーン用に必要
#include "TutorialScene.h" // 切り替え先をnewするために必要

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	KamataEngine::Initialize(L"LE3C_19_ムラタ_トモキ_Bike_Savaiver");
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 現在のシーンを基底クラスのポインタで保持
	BaseScene* currentScene = new TitleScnce();
	currentScene->Initialize();

	// メインループ
	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		dxCommon->PreDraw();

		// どのシーンであっても、呼び出し方は常に同じ
		currentScene->Update();
		currentScene->Draw();

		// ★ シーン切り替えロジック
		if (currentScene->IsFinished()) {
			// 次にいきたいシーンの種類を取得
			Scene nextType = currentScene->GetNextScene();

			// 古いシーンを削除
			delete currentScene;
			currentScene = nullptr;

			// --- ★ここから下が途切れている、または足りない部分です ---
			// 次のシーンのインスタンスを作成
			if (nextType == Scene::Title) {
				currentScene = new TitleScnce();
			} else if (nextType == Scene::Tutorial) {
				currentScene = new TutorialScene();
			} else if (nextType == Scene::Game) {
				currentScene = new GameScene();
			} else if (nextType == Scene::GameOver) {
				currentScene = new GameOverScene();
			}

			// 新しいシーンを初期化
			if (currentScene) {
				currentScene->Initialize();
			}
		}

		dxCommon->PostDraw();
	}

	// 終了処理
	if (currentScene) {
		delete currentScene;
		currentScene = nullptr;
	}

	KamataEngine::Finalize();
	return 0;
}