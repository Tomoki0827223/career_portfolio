#include "TextureConverter.h"
#include <Windows.h>
#include <filesystem>


void TextureConverter::ConvertTextureWICToDDS(const std::string& filePath) {
	// ファイルを読み込む
	LoadWICTextureFromFile(filePath);

	// 保存先パスの作成（単純に末尾を書き換えるのではなく、安全な方法にする）
	std::filesystem::path srcPath(filePath);
	std::filesystem::path ddsPath = srcPath;
	ddsPath.replace_extension(".dds"); // 拡張子を .dds に変更

	std::wstring wddsFilePath = ddsPath.wstring();

	// ② ミップマップの生成
	DirectX::ScratchImage mipChain;
	HRESULT hr = DirectX::GenerateMipMaps(m_WICData.GetImages(), m_WICData.GetImageCount(), m_WICData.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);

	// 生成に成功したデータを使う（失敗した場合は元のデータを使う）
	DirectX::ScratchImage* saveImage = &m_WICData;
	if (SUCCEEDED(hr)) {
		saveImage = &mipChain;
	}

	// ③ DDS ファイルとして保存
	hr = DirectX::SaveToDDSFile(saveImage->GetImages(), saveImage->GetImageCount(), saveImage->GetMetadata(), DirectX::DDS_FLAGS_NONE, wddsFilePath.c_str());

	if (FAILED(hr)) {
		// 保存に失敗した場合はここにくる
		OutputDebugStringA("Failed to save DDS file.\n");
	}
}

// テクスチャファイル読み込み関数の実装（LoadWICTextureFromFile）
void TextureConverter::LoadWICTextureFromFile(const std::string& filePath)
{
    // ① ファイルパスをワイド文字列に変換する (←前ステップで実装済み)
    std::wstring wFilePath = ConvertMultiByteStringToWideString(filePath);

    // 実行結果の確認: (←このコメントは削除または無視)
    // デバッグ時にブレークポイントを置いて wFilePath の中身を確認し、
    // `L\"ファイルパス\"` のようになっていれば成功です。


    // ▼▼▼ 【ここから置き換え】 ▼▼▼
    // ② テクスチャを読み込む
    HRESULT hr = DirectX::LoadFromWICFile(
        wFilePath.c_str(),      // ワイド文字列に変換されたファイルパス
        DirectX::WIC_FLAGS_NONE, // フラグ（ここでは特に指定なし）
        &m_metadata,            // 読み込んだテクスチャのメタデータを格納するポインタ
        m_WICData               // 読み込んだテクスチャのイメージデータを格納するScratchImage
    );

    // 読み込みに失敗した場合は強制終了
    assert(SUCCEEDED(hr));
    // ▲▲▲ 【ここまで置き換え】 ▲▲▲

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

// マルチバイト文字列をワイド文字列に変換する関数の実装
std::wstring TextureConverter::ConvertMultiByteStringToWideString(const std::string& mString)
{
    // ① まず第5引数に nullptr を入れて呼び出すことで、必要な文字数 (バッファサイズ) を取得する
    // ワイド文字列に変換した際の文字数を計算 (終端NULLを含む)
    // CP_ACP: 現在のシステムANSIコードページを使用
    int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, mString.c_str(), -1, nullptr, 0);

    // ワイド文字列
    std::wstring wString;
    // ② その文字数分のバッファを wString に用意する
    wString.resize(filePathBufferSize);

    // ③ 実際に変換する
    MultiByteToWideChar(CP_ACP, 0, mString.c_str(), -1, &wString[0], filePathBufferSize);

    // 終端NULLを含めてリサイズしているので、NULL文字を取り除く (std::wstringの扱いとして)
    wString.resize(filePathBufferSize - 1);

    return wString;
}