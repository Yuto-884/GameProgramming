// レンダーターゲット制御クラス

#pragma once

#include "device.h"
#include "swap_chain.h"
#include "descriptor_heap.h"
#include <vector>

//---------------------------------------------------------------------------------
/**
 * @brief	レンダーターゲット制御クラス
 */
class RenderTarget final {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    コンストラクタ
     */
    RenderTarget() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~RenderTarget();


    //---------------------------------------------------------------------------------
    /**
     * @brief	バックバッファを生成する
     * @param	device		デバイスクラスのインスタンス
     * @param	swapChain	スワップチェインのポインタ
     * @param	heap		ディスクリプターヒープのインスタンス
     * @return	生成の成否
     */
    [[nodiscard]] bool createBackBuffer(const Device& device, const SwapChain& swapChain, const DescriptorHeap& heap) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ビュー（ディスクリプタハンドル）を取得する
     * @param	device	デバイスクラスのインスタンス
     * @param	heap	ディスクリプタヒープのインスタンス
     * @param	index	インデックス
     * @return	ディスクリプタハンドル
     */
    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE getDescriptorHandle(const Device& device, const DescriptorHeap& heap, UINT index) const noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットを取得する
     * @param	index	インデックス
     */
    [[nodiscard]] ID3D12Resource* get(uint32_t index) const noexcept;


private:
    std::vector<ID3D12Resource*> renderTargets_;  /// レンダーターゲットリソースの配列
};
