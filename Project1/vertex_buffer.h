#pragma once

#include "device.h"
#include <d3d12.h>
#include <cstdint>

class VertexBuffer final {
public:
    VertexBuffer() = default;
    ~VertexBuffer();

    // コピー禁止（超重要）
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    // ムーブのみ許可
    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    [[nodiscard]] bool create(
        const Device& device,
        const void* vertexData,
        uint32_t vertexCount,
        uint32_t strideBytes
    ) noexcept;

    [[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& view() const noexcept;

private:
    ID3D12Resource* vertexBuffer_ = nullptr;
    D3D12_VERTEX_BUFFER_VIEW vbView_{};
    uint32_t vertexCount_{};
    uint32_t strideBytes_{};
};
