#include "shader.h"
#include <cassert>
#include <string>

#include <D3Dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

Shader::~Shader()
{
    if (vertexShader_) {
        vertexShader_->Release();
        vertexShader_ = nullptr;
    }
    if (pixelShader_) {
        pixelShader_->Release();
        pixelShader_ = nullptr;
    }
}

[[nodiscard]] bool Shader::create(const Device&) noexcept
{
    const wchar_t* filePath = L"asset/shader.hlsl";

    ID3DBlob* error = nullptr;
    HRESULT hr;

    // ---------- Vertex Shader ----------
    hr = D3DCompileFromFile(
        filePath,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "vs",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vertexShader_,
        &error
    );

    if (FAILED(hr)) {
        if (error) {
            OutputDebugStringA((char*)error->GetBufferPointer());
            error->Release();
        }
        assert(false && "Vertex Shader compile failed");
        return false;
    }

    // ---------- Pixel Shader ----------
    hr = D3DCompileFromFile(
        filePath,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "ps",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &pixelShader_,
        &error
    );

    if (FAILED(hr)) {
        if (error) {
            OutputDebugStringA((char*)error->GetBufferPointer());
            error->Release();
        }
        assert(false && "Pixel Shader compile failed");
        return false;
    }

    if (error) {
        error->Release();
    }

    return true;
}

ID3DBlob* Shader::vertexShader() const noexcept
{
    assert(vertexShader_);
    return vertexShader_;
}

ID3DBlob* Shader::pixelShader() const noexcept
{
    assert(pixelShader_);
    return pixelShader_;
}
