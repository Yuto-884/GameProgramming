#include "vertex_buffer.h"
#include <cassert>
#include <cstring>

VertexBuffer::~VertexBuffer() {
    if (vertexBuffer_) {
        vertexBuffer_->Release();
        vertexBuffer_ = nullptr;
    }
}

// ムーブコンストラクタ
VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept {
    vertexBuffer_ = other.vertexBuffer_;
    vbView_ = other.vbView_;
    vertexCount_ = other.vertexCount_;
    strideBytes_ = other.strideBytes_;

    other.vertexBuffer_ = nullptr;
}

// ムーブ代入
VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
    if (this != &other) {
        if (vertexBuffer_) {
            vertexBuffer_->Release();
        }
        vertexBuffer_ = other.vertexBuffer_;
        vbView_ = other.vbView_;
        vertexCount_ = other.vertexCount_;
        strideBytes_ = other.strideBytes_;

        other.vertexBuffer_ = nullptr;
    }
    return *this;
}

bool VertexBuffer::create(
    const Device& device,
    const void* vertexData,
    uint32_t vertexCount,
    uint32_t strideBytes
) noexcept
{
    assert(vertexData);
    assert(vertexCount > 0);
    assert(strideBytes > 0);

    vertexCount_ = vertexCount;
    strideBytes_ = strideBytes;

    const UINT64 bufferSize = UINT64(vertexCount) * strideBytes;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resDesc{};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = bufferSize;
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
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
    assert(SUCCEEDED(hr));

    void* mapped = nullptr;
    vertexBuffer_->Map(0, nullptr, &mapped);
    std::memcpy(mapped, vertexData, bufferSize);
    vertexBuffer_->Unmap(0, nullptr);

    vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vbView_.SizeInBytes = UINT(bufferSize);
    vbView_.StrideInBytes = strideBytes;

    return true;
}

const D3D12_VERTEX_BUFFER_VIEW& VertexBuffer::view() const noexcept {
    assert(vertexBuffer_);
    return vbView_;
}

