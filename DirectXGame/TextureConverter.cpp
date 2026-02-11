#include "TextureConverter.h"
#include <Windows.h>
#include <filesystem>



// TextureConverter.cpp 内の修正例
void TextureConverter::ConvertTextureWICToDDS(const std::string& filePath) {
	LoadWICTextureFromFile(filePath);

	std::filesystem::path srcPath(filePath);
	std::filesystem::path ddsPath = srcPath;
	ddsPath.replace_extension(".dds");
	std::wstring wddsFilePath = ddsPath.wstring();

	// ② ミップマップの生成
	DirectX::ScratchImage mipChain;
	HRESULT hr = DirectX::GenerateMipMaps(m_WICData.GetImages(), m_WICData.GetImageCount(), m_WICData.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);

	DirectX::ScratchImage* midImage = &m_WICData;
	if (SUCCEEDED(hr)) {
		midImage = &mipChain;
	}

	// ★追加: テクスチャ圧縮 (スライドの指示通りミップマップ生成の後に差し込む) ★
	DirectX::ScratchImage converted;
	hr = DirectX::Compress(
	    midImage->GetImages(), midImage->GetImageCount(), midImage->GetMetadata(),
	    DXGI_FORMAT_BC7_UNORM_SRGB, // 圧縮形式
	    DirectX::TEX_COMPRESS_BC7_QUICK | DirectX::TEX_COMPRESS_SRGB_OUT | DirectX::TEX_COMPRESS_PARALLEL, 1.0f, converted);

	DirectX::ScratchImage* saveImage = midImage;
	if (SUCCEEDED(hr)) {
		saveImage = &converted;
	}

	// ③ DDS ファイルとして保存
	hr = DirectX::SaveToDDSFile(saveImage->GetImages(), saveImage->GetImageCount(), saveImage->GetMetadata(), DirectX::DDS_FLAGS_NONE, wddsFilePath.c_str());

	if (FAILED(hr)) {
		OutputDebugStringA("Failed to save DDS file.\n");
	}
}


// テクスチャファイル読み込み関数の実装（LoadWICTextureFromFile）
void TextureConverter::LoadWICTextureFromFile(const std::string& filePath) {
	// ① ファイルパスをワイド文字列に変換する
	std::wstring wFilePath = ConvertMultiByteStringToWideString(filePath);

	// ② テクスチャを読み込む
	// 最初の宣言だけ HRESULT を付け、2回目は hr = ... と書きます
	HRESULT hr = DirectX::LoadFromWICFile(
	    wFilePath.c_str(),       // ワイド文字列に変換されたファイルパス
	    DirectX::WIC_FLAGS_NONE, // フラグ
	    &m_metadata,             // メタデータを格納
	    m_WICData                // イメージデータを格納
	);

	// 読み込みに失敗した場合はログを出して終了
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to load WIC texture file.\n");
		assert(SUCCEEDED(hr));
		return;
	}

	// パスの分離処理（保存名の決定に使用）
	SeparateFilePath(wFilePath);
}

void TextureConverter::SeparateFilePath(const std::wstring& filePath)
{
    size_t pos1;
    std::wstring exceptExt;

    // // 区切り文字 '.' が出てくる一番最後の部分を検索
    pos1 = filePath.rfind(L'.');
    // // 検索がヒットしたら
    if (pos1 != std::wstring::npos) {
        // // 区切り文字の後ろをファイル拡張子として保存
        fileExt = filePath.substr(pos1 + 1, filePath.size() - pos1 - 1);
        // // 区切り文字の前までを抜き出す
        exceptExt = filePath.substr(0, pos1);
    }
    else {
        fileExt = L"";
        exceptExt = filePath;
    }
    // 拡張子以外の部分をさらに分解する。

    // // 区切り文字 '\\' が出てくる一番最後の部分を検索
    pos1 = exceptExt.rfind(L'\\');

    if (pos1 != std::wstring::npos) {
        // // 区切り文字の前までをディレクトリパスとして保存
        directoryPath = exceptExt.substr(0, pos1 + 1);
        // // 区切り文字の後ろをファイル名として保存
        fileName = exceptExt.substr(pos1 + 1);
        return;
    }

    // // 区切り文字 '/' が出てくる一番最後の部分を検索
    pos1 = exceptExt.rfind(L'/');
    if (pos1 != std::wstring::npos) {
        // // 区切り文字の前までをディレクトリパスとして保存 (自分で考えよう の部分を実装)
        directoryPath = exceptExt.substr(0, pos1 + 1);
        // // 区切り文字の後ろをファイル名として保存 (自分で考えよう の部分を実装)
        fileName = exceptExt.substr(pos1 + 1);
        return;
    }

    if (!std::filesystem::exists(filePath)) {
		// ここで止まれば、やはり「プログラムからファイルが見えていない」ことが確定します
		OutputDebugStringA("File Not Found!");
	}

    // // 区切り文字がないのでファイル名のみとして扱う
    directoryPath = L"";
    fileName = exceptExt;
}

// TextureConverter.cpp 内の関数を以下に差し替え
std::wstring TextureConverter::ConvertMultiByteStringToWideString(const std::string& mString) {
	if (mString.empty())
		return L"";

	// 必要なバッファサイズを取得
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, mString.c_str(), -1, nullptr, 0);

	std::wstring wString(filePathBufferSize, L'\0');
	MultiByteToWideChar(CP_ACP, 0, mString.c_str(), -1, &wString[0], filePathBufferSize);

	// std::wstringは終端NULLを内部で管理するため、末尾のNULL文字を手動で取り除く
	size_t nullPos = wString.find_last_not_of(L'\0');
	if (nullPos != std::wstring::npos) {
		wString.resize(nullPos + 1);
	}

	return wString;
}