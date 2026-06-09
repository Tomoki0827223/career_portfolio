#include "GameOverScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScnce.h"
#include "TutorialScene.h"
#include <Windows.h>

using namespace KamataEngine;


// Scene enumに Tutorial と GameOver を追加
enum class Scene { Title, Tutorial, Game, GameOver };

//Scene scene = Scene::Title;
Scene scene = Scene::Title;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// 初期化処理
	KamataEngine::Initialize(L"LE3C_19_ムラタ_トモキ_Bike_Savaiver");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 共通の親クラスのポインタだけを用意
	BaseScene* currentScene = nullptr;

	// 最初はタイトルシーンを生成して代入（実体は子クラス、持つポインタは親クラス）
	currentScene = new TitleScnce();
	currentScene->Initialize();

	// メインループ
	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

		dxCommon->PreDraw();

		// 今どのシーンであっても、分岐なしでただ呼び出すだけ！
		// 内部で自動的に、Title、Game、GameOver などの正しい Update/Draw が駆動します。
		if (currentScene) {
			currentScene->Update();
			currentScene->Draw();
		}

		dxCommon->PostDraw();
	}

	// 終了処理


	// 終了処理
	KamataEngine::Finalize();
	return 0;
}