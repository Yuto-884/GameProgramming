// コンスタントバッファクラス

#pragma once

#include "device.h"
#include "descriptor_heap.h"

//---------------------------------------------------------------------------------
/**
 * @brief	コンスタントバッファクラス
 */
class ConstantBuffer final {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    コンストラクタ
     */
    ConstantBuffer() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~ConstantBuffer();

    //---------------------------------------------------------------------------------
    /**
     * @brief	コンスタントバッファの作成
     * @param	device			デバイスクラスのインスタンス
     * @param	heap			登録先のディスクリプタヒープのインスタンス
     * @param	bufferSize		コンスタントバッファのサイズ
     * @param	descriptorIndex	ディスクリプタの登録インデックス
     * @return	生成の成否
     */
    [[nodiscard]] bool create(const Device& device, const DescriptorHeap& heap, UINT bufferSize, UINT descriptorIndex) noexcept;


    //---------------------------------------------------------------------------------
    /**
     * @brief	コンスタントバッファを取得する
     * @return	コンスタントバッファのポインタ
     */
    [[nodiscard]] ID3D12Resource* constantBuffer() const noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	GPU 用ディスクリプタハンドルを取得する
     * @return	GPU 用ディスクリプタハンドル
     */
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getGpuDescriptorHandle() const noexcept;

private:
    ID3D12Resource* constantBuffer_{};  /// コンスタントバッファ
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};       /// GPU 用ディスクリプタハンドル
};