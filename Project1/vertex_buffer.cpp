// 頂点バッファクラス

#include "vertex_buffer.h"
#include <cassert>
#include <cstring>

VertexBuffer::~VertexBuffer() {
    if (vertexBuffer_) {
        vertexBuffer_->Release();
        vertexBuffer_ = nullptr;
    }
}

[[nodiscard]] bool VertexBuffer::create(const Device& device,
    const void* vertexData,
    uint32_t vertexCount,
    uint32_t strideBytes) noexcept
{
    assert(vertexData && "vertexData is null");
    assert(vertexCount > 0 && "vertexCount must be > 0");
    assert(strideBytes > 0 && "strideBytes must be > 0");

    vertexCount_ = vertexCount;
    strideBytes_ = strideBytes;

    const UINT64 bufferSize = static_cast<UINT64>(vertexCount) * strideBytes;

    // UPLOAD ヒープ（CPU から書ける）
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    // バッファリソース
    D3D12_RESOURCE_DESC resDesc{};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = bufferSize;
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.Format = DXGI_FORMAT_UNKNOWN;
    resDesc.SampleDesc.Count = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = device.get()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer_)
    );

    if (FAILED(hr)) {
        assert(false && "頂点バッファの作成に失敗");
        return false;
    }

    // データ転送
    void* mapped = nullptr;
    hr = vertexBuffer_->Map(0, nullptr, &mapped);
    if (FAILED(hr) || !mapped) {
        assert(false && "頂点バッファの Map に失敗");
        return false;
    }
    std::memcpy(mapped, vertexData, static_cast<size_t>(bufferSize));
    vertexBuffer_->Unmap(0, nullptr);

    // VBV 作成
    vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vbView_.SizeInBytes = static_cast<UINT>(bufferSize);
    vbView_.StrideInBytes = strideBytes;

    return true;
}

[[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& VertexBuffer::view() const noexcept {
    assert(vertexBuffer_ && "頂点バッファが未作成です");
    return vbView_;
}

[[nodiscard]] uint32_t VertexBuffer::vertexCount() const noexcept {
    assert(vertexBuffer_ && "頂点バッファが未作成です");
    return vertexCount_;
}

[[nodiscard]] ID3D12Resource* VertexBuffer::resource() const noexcept {
    assert(vertexBuffer_ && "頂点バッファが未作成です");
    return vertexBuffer_;
}
