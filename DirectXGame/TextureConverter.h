#pragma once
#include <string>

#include "DirectXTex.h"

class TextureConverter
{
public:
    /// <summary>
    /// テクスチャをWICからDDSに変換する
    /// </summary>
    /// <param name="filePath">ファイルパス</param>
    void ConvertTextureWICToDDS(const std::string& filePath); // publicな変換関数

    /// <summary>
    /// マルチバイト文字列をワイド文字列に変換
    /// </summary>
    /// <param name="mString">マルチバイト文字列</param>
    /// <returns>ワイド文字列</returns>
    static std::wstring ConvertMultiByteStringToWideString(const std::string& mString);


private:
    /// <summary>
    /// テクスチャファイル読み込み
    /// </summary>
    /// <param name="filePath">ファイルパス</param>
    void LoadWICTextureFromFile(const std::string& filePath); // privateな読み込み関数

    // ▼▼▼ 【ここから追加】 ▼▼▼
    // WICから読み込んだテクスチャのイメージデータとメタデータを格納する
    DirectX::ScratchImage m_WICData;

    // テクスチャのメタデータ
    DirectX::TexMetadata m_metadata;
    // ▲▲▲ 【ここまで追加】 ▲▲▲

    void SeparateFilePath(const std::wstring& filePath);

	std::wstring directoryPath; // ディレクトリパス
	std::wstring fileName;      // ファイル名
	std::wstring fileExt; // 拡張子
};