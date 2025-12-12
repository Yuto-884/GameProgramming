#include "shader.h"
#include <cassert>
#include <string>
#include <Windows.h>

#include <D3Dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

Shader::~Shader() {
    if (vertexShader_) { vertexShader_->Release(); vertexShader_ = nullptr; }
    if (pixelShader_) { pixelShader_->Release();  pixelShader_ = nullptr; }
}

static void ShowCompileError(ID3DBlob* error, const char* title)
{
    if (!error) {
        MessageBoxA(nullptr, "D3DCompileFromFile failed (error blob is nullptr)", title, MB_OK | MB_ICONERROR);
        return;
    }
    const char* msg = (const char*)error->GetBufferPointer();
    MessageBoxA(nullptr, msg ? msg : "unknown error", title, MB_OK | MB_ICONERROR);
}

[[nodiscard]] bool Shader::create(const Device& device) noexcept
{
    // 実行ファイルの作業フォルダ で "asset/shader.hlsl" を探す
    const wchar_t* filePath = L"asset/shader.hlsl";

    UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

    ID3DBlob* error = nullptr;

    // VS
    HRESULT hr = D3DCompileFromFile(
        filePath, nullptr, nullptr,
        "vs", "vs_5_0",
        flags, 0,
        &vertexShader_, &error);

    if (FAILED(hr)) {
        ShowCompileError(error, "VS compile failed");
        if (error) error->Release();
        return false;
    }
    if (error) { error->Release(); error = nullptr; }

    // PS
    hr = D3DCompileFromFile(
        filePath, nullptr, nullptr,
        "ps", "ps_5_0",
        flags, 0,
        &pixelShader_, &error);

    if (FAILED(hr)) {
        ShowCompileError(error, "PS compile failed");
        if (error) error->Release();
        return false;
    }
    if (error) { error->Release(); error = nullptr; }

    return true;
}

[[nodiscard]] ID3DBlob* Shader::vertexShader() const noexcept {
    assert(vertexShader_ && "vertex shader is null");
    return vertexShader_;
}

[[nodiscard]] ID3DBlob* Shader::pixelShader() const noexcept {
    assert(pixelShader_ && "pixel shader is null");
    return pixelShader_;
}

