// デプスバッファ制御クラス

#pragma once

#include "device.h"
#include "descriptor_heap.h"
#include "window.h"

//---------------------------------------------------------------------------------
/**
 * @brief	デプスバッファ制御クラス
 */
class DepthBuffer final {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    コンストラクタ
     */
    DepthBuffer() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~DepthBuffer();

    //---------------------------------------------------------------------------------
    /**
     * @brief	デプスバッファを生成する
     * @param	device			デバイスクラスのインスタンス
     * @param	heap			登録先のディスクリプタヒープのインスタンス
     * @param	window			ウィンドウクラスのインスタンス
     * @return	生成の成否
     */
    [[nodiscard]] bool create(const Device& device, const DescriptorHeap& heap, const Window& window) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	デプスバッファを取得する
     * @return	デプスバッファのポインタ
     */
    [[nodiscard]] ID3D12Resource* depthBuffer() const noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ディスクリプタハンドルを取得する
     * @return	ディスクリプタハンドル
     */
    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getCpuDescriptorHandle() const noexcept;

private:
    ID3D12Resource* depthBuffer_;  /// レンダーターゲットリソースの配列
    D3D12_CPU_DESCRIPTOR_HANDLE handle_{};     /// ディスクリプタハンドル
};
