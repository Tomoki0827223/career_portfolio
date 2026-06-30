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
		// カマタエンジンのメッセージ処理・更新（×ボタンなどでループを抜ける）
		if (KamataEngine::Update()) {
			break;
		}

		// 1. シーンの更新（キー入力や状態の更新）
		currentScene->Update();

		// ★ 2. シーン切り替えロジック（Updateの直後に判定するのが鉄則！）
		if (currentScene->IsFinished()) {
			// 次にいきたいシーンの種類を取得
			Scene nextType = currentScene->GetNextScene();

			// 古いシーンを削除
			delete currentScene;
			currentScene = nullptr;

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
			if (currentScene != nullptr) {
				currentScene->Initialize();
			}

			// シーンが切り替わったフレームは、古いシーンのDrawを呼ぶと危険なので、
			// 描画をスキップして次のフレーム（ループの先頭）へ進む
			continue;
		}

		// 3. 描画処理（切り替えが発生しなかった場合のみ、安全に現在のシーンを描画）
		dxCommon->PreDraw();

		currentScene->Draw();

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