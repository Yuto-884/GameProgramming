// デプスバッファ制御クラス

#include "depth_buffer.h"
#include <cassert>

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
DepthBuffer::~DepthBuffer() {
    // デプスバッファの解放
    if (depthBuffer_) {
        depthBuffer_->Release();
        depthBuffer_ = nullptr;
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	デプスバッファを生成する
 * @param	device			デバイスクラスのインスタンス
 * @param	heap			登録先のディスクリプタヒープのインスタンス
 * @param	window			ウィンドウクラスのインスタンス
 * @return	生成の成否
 */
[[nodiscard]] bool DepthBuffer::create(const Device& device, const DescriptorHeap& heap, const Window& window) noexcept {
    // ウィンドウサイズを取得
    const auto [w, h] = window.size();

    // デプスバッファ用のテクスチャリソースの作成
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    D3D12_RESOURCE_DESC depthDesc{};
    depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthDesc.Width = w;
    depthDesc.Height = h;
    depthDesc.DepthOrArraySize = 1;
    depthDesc.MipLevels = 1;
    depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    // デプスバッファのクリア値の設定
    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = depthDesc.Format;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    const auto res = device.get()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&depthBuffer_));

    if (FAILED(res)) {
        assert(false && "デプスバッファの作成に失敗しました");
        return false;
    }

    // ビューの作成
    auto heapType = heap.getType();
    if (heapType != D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
        assert(false && "ディスクリプタヒープのタイプが DSV ではありません");
        false;
    }

    // デプスビューの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = depthDesc.Format;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

    // ディスクリプタヒープの開始ハンドルを取得
    handle_ = heap.get()->GetCPUDescriptorHandleForHeapStart();
    // デプスビューの作成
    device.get()->CreateDepthStencilView(depthBuffer_, &dsvDesc, handle_);

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	デプスバッファを取得する
 * @return	デプスバッファのポインタ
 */
[[nodiscard]] ID3D12Resource* DepthBuffer::depthBuffer() const noexcept {
    assert(depthBuffer_ && "デプスバッファが未生成です");
    return depthBuffer_;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ディスクリプタハンドルを取得する
 * @return	ディスクリプタハンドル
 */
[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE DepthBuffer::getCpuDescriptorHandle() const noexcept {
    assert(depthBuffer_ && "デプスバッファが未生成です");
    return handle_;
}
