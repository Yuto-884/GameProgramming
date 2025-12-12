// 頂点バッファクラス

#pragma once

#include "device.h"
#include <d3d12.h>
#include <cstdint>

//---------------------------------------------------------------------------------
/**
 * @brief  頂点バッファクラス
 */
class VertexBuffer final {
public:
    VertexBuffer() = default;
    ~VertexBuffer();

    //---------------------------------------------------------------------------------
    /**
     * @brief   頂点バッファを作成する（UPLOADヒープ）
     * @param   device          Device
     * @param   vertexData      頂点配列の先頭ポインタ
     * @param   vertexCount     頂点数
     * @param   strideBytes     1頂点あたりのサイズ（sizeof(Vertex)）
     */
    [[nodiscard]] bool create(const Device& device,
        const void* vertexData,
        uint32_t vertexCount,
        uint32_t strideBytes) noexcept;

    //---------------------------------------------------------------------------------
    /** @brief  VBV を取得 */
    [[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& view() const noexcept;

    //---------------------------------------------------------------------------------
    /** @brief  頂点数を取得 */
    [[nodiscard]] uint32_t vertexCount() const noexcept;

    //---------------------------------------------------------------------------------
    /** @brief  頂点バッファリソースを取得 */
    [[nodiscard]] ID3D12Resource* resource() const noexcept;

private:
    ID3D12Resource* vertexBuffer_{};          /// 頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW vbView_{};       /// 頂点バッファビュー
    uint32_t vertexCount_{};                  /// 頂点数
    uint32_t strideBytes_{};                  /// 1頂点のサイズ
};
