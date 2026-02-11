#include "TextureConverter.h"
#include <Windows.h>
#include <filesystem> // 追加
#include <iostream>

// 名前空間の省略を定義（これで fs:: が使えるようになります）
namespace fs = std::filesystem;

void ConvertAllImages() {
	TextureConverter converter;
	std::string rootPath = "Resources";

	if (!fs::exists(rootPath)) {
		OutputDebugStringA("Error: 'Resources' folder not found!\n");
		return;
	}

	for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();
			if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
				std::string filePath = entry.path().string();
				std::string log = "Converting: " + filePath + "\n";
				OutputDebugStringA(log.c_str());

				converter.ConvertTextureWICToDDS(filePath);
			}
		}
	}
	OutputDebugStringA("Conversion Process Finished.\n");
}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// コンバータの実行のみを行う
	ConvertAllImages();

	MessageBoxA(nullptr, "Conversion Finished!", "TextureConverter", MB_OK);

	return 0;
}