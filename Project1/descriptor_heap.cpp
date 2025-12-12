// ディスクリプターヒープ制御クラス

#include "descriptor_heap.h"
#include <cassert>

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
DescriptorHeap::~DescriptorHeap() {
    // ディスクリプタヒープの解放
    if (heap_) {
        heap_->Release();
        heap_ = nullptr;
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	ディスクリプタヒープを生成する
 * @param	device	デバイスクラスのインスタンス
 * @param	type	ディスクリプタヒープのタイプ
 * @param	numDescriptors	ディスクリプタの数
 * @param	shaderVisible	シェーダーからアクセス可能かどうか
 * @return	生成の成否
 */
[[nodiscard]] bool DescriptorHeap::create(const Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible) noexcept {
    // ヒープの設定
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = type;
    heapDesc.NumDescriptors = numDescriptors;
    heapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    type_ = type;  // ヒープのタイプを保存

    // ディスクリプタヒープの生成
    HRESULT hr = device.get()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap_));
    if (FAILED(hr)) {
        assert(false && "ディスクリプタヒープの生成に失敗しました");
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ディスクリプタヒープを取得する
 * @return	ディスクリプタヒープのポインタ
 */
[[nodiscard]] ID3D12DescriptorHeap* DescriptorHeap::get() const noexcept {
    if (!heap_) {
        assert(false && "ディスクリプタヒープが未生成です");
        return nullptr;
    }

    return heap_;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ディスクリプタヒープのタイプを取得する
 * @return	ディスクリプタヒープのタイプ
 */
[[nodiscard]] D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeap::getType() const noexcept {
    if (!heap_) {
        assert(false && "ディスクリプタヒープが未生成です");
    }
    return type_;
}
