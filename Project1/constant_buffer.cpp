// コンスタントバッファクラス

#include "constant_buffer.h"
#include <cassert>

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
ConstantBuffer::~ConstantBuffer() {
    if (constantBuffer_) {
        constantBuffer_->Release();
        constantBuffer_ = nullptr;
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	コンスタントバッファの作成
 * @param	device			デバイスクラスのインスタンス
 * @param	heap			登録先のディスクリプタヒープのインスタンス
 * @param	bufferSize		コンスタントバッファのサイズ
 * @param	descriptorIndex	ディスクリプタの登録インデックス
 * @return	生成の成否
 */
[[nodiscard]] bool ConstantBuffer::create(const Device& device, const DescriptorHeap& heap, UINT bufferSize, UINT descriptorIndex) noexcept {
    // アライメント済みサイズの計算
    const auto size = (sizeof(bufferSize) + 255) & ~255;

    // バッファリソースの作成
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = size;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    const auto res = device.get()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer_));
    if (FAILED(res)) {
        assert(false && "コンスタントバッファの作成に失敗しました");
        return false;
    }

    // ビューの作成
    auto heapType = heap.getType();
    if (heapType != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
        assert(false && "ディスクリプタヒープのタイプが CBV_SRV_UAV ではありません");
        false;
    }

    // コンスタントバッファビューの設定
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = constantBuffer_->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = size;

    // ディスクリプタのサイズを取得
    UINT cbvDescriptorSize = device.get()->GetDescriptorHandleIncrementSize(heap.getType());

    // ディスクリプタヒープの開始ハンドルを取得
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap.get()->GetCPUDescriptorHandleForHeapStart();
    // 指定されたインデックス分ハンドルを進める
    cpuHandle.ptr += descriptorIndex * cbvDescriptorSize;

    // コンスタントバッファビューとハンドルを関連付ける
    device.get()->CreateConstantBufferView(&cbvDesc, cpuHandle);

    // GPU 用ディスクリプタハンドルを保存
    gpuHandle_ = heap.get()->GetGPUDescriptorHandleForHeapStart();
    // 指定されたインデックス分ハンドルを進める
    gpuHandle_.ptr += descriptorIndex * cbvDescriptorSize;

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	コンスタントバッファを取得する
 * @return	コンスタントバッファのポインタ
 */
[[nodiscard]] ID3D12Resource* ConstantBuffer::constantBuffer() const noexcept {
    assert(constantBuffer_ && "コンスタントバッファが未作成です");
    return constantBuffer_;
}

//---------------------------------------------------------------------------------
/**
 * @brief	GPU 用ディスクリプタハンドルを取得する
 * @return	GPU 用ディスクリプタハンドル
 */
[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer::getGpuDescriptorHandle() const noexcept {
    assert(constantBuffer_ && "コンスタントバッファが未作成です");
    return gpuHandle_;
}