// シェーダクラス

#include "shader.h"
#include <cassert>
#include <string>

#include <D3Dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
Shader::~Shader() {
    // 頂点シェーダの解放
    if (vertexShader_) {
        vertexShader_->Release();
        vertexShader_ = nullptr;
    }
    // ピクセルシェーダの解放
    if (pixelShader_) {
        pixelShader_->Release();
        pixelShader_ = nullptr;
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	シェーダを作成する
 * @param	device	デバイスクラスのインスタンス
 * @return	成功すれば true
 */
[[nodiscard]] bool Shader::create(const Device& device) noexcept {
    // シェーダを読込、コンパイルして生成する

    // シェーダファイルのパス
    const std::string  filePath = "asset/shader.hlsl";
    const std::wstring temp = std::wstring(filePath.begin(), filePath.end());
    // シェーダのコンパイルエラーなどが分かる様にする
    ID3DBlob* error{};

    auto res = D3DCompileFromFile(temp.data(), nullptr, nullptr, "vs", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShader_, &error);
    if (FAILED(res)) {
        char* p = static_cast<char*>(error->GetBufferPointer());
        assert(false && "頂点シェーダのコンパイルに失敗しました");
    }
    res = D3DCompileFromFile(temp.data(), nullptr, nullptr, "ps", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShader_, &error);
    if (FAILED(res)) {
        char* p = static_cast<char*>(error->GetBufferPointer());
        assert(false && "ピクセルシェーダのコンパイルに失敗しました");
    }

    if (error) {
        error->Release();
    }

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	頂点シェーダを取得する
 * @return	頂点シェーダのデータ
 */
[[nodiscard]] ID3DBlob* Shader::vertexShader() const noexcept {
    if (!vertexShader_) {
        assert(false && "頂点シェーダが未作成です");
    }

    return vertexShader_;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ピクセルシェーダを取得する
 * @return	ピクセルシェーダのデータ
 */
[[nodiscard]] ID3DBlob* Shader::pixelShader() const noexcept {
    if (!pixelShader_) {
        assert(false && "ピクセルシェーダが未作成です");
    }

    return pixelShader_;
}