#include "Dx12.h"

#include"command_allocator.h"
#include"command_list.h"
#include"command_queue.h"
#include"descriptor_heap.h"
#include"device.h"
#include"Fence.h"
#include"render_target.h"
#include"swap_chain.h"


#include<cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
IDXGIFactory4* Dx12::CreateDXGIFactory() {
    IDXGIFactory4* factory;
    UINT           createFactoryFlags = 0;

#if defined(_DEBUG)
    // デバッグビルドではデバッグフラグを設定
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        // エラーハンドリング：ファクトリー作成失敗
        OutputDebugString("Failed to create DXGI Factory\n");
        return nullptr;
    }

    return factory;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
IDXGIAdapter1* Dx12::GetHardwareAdapter(IDXGIFactory4* factory) {
    IDXGIAdapter1* adapter;

    // ハードウェアアダプターを順番に検索
    for (UINT adapterIndex = 0;; ++adapterIndex) {
        adapter = nullptr;
        if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adapterIndex, &adapter)) {
            break;  // アダプターが見つからない場合は終了
        }

        // アダプター情報を取得
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        // ソフトウェアアダプターをスキップ
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            adapter->Release();
            continue;
        }

        // DirectX12対応テスト
        if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
            return adapter;  // 適切なアダプターを発見
        }

        adapter->Release();
    }

    return nullptr;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
ID3D12Device* Dx12::CreateD3D12Device(IDXGIAdapter1* adapter) {
    ID3D12Device* device;

    // デバイス作成を試行
    HRESULT hr = D3D12CreateDevice(
        adapter,                 // 使用するアダプター
        D3D_FEATURE_LEVEL_11_0,  // 最小機能レベル
        IID_PPV_ARGS(&device)    // 作成されるデバイス
    );

    if (FAILED(hr)) {
        // フォールバック：ソフトウェアアダプターを試行
        hr = D3D12CreateDevice(
            nullptr,  // 規定アダプター
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&device));

        if (FAILED(hr)) {
            OutputDebugString("Failed to create D3D12 Device\n");
            return nullptr;
        }

        OutputDebugString("Using software adapter (WARP)\n");
    }

    return device;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
ID3D12CommandQueue* Dx12::CreateCommandQueue(ID3D12Device* device) {
    // コマンドキューの設定
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;       // 直接実行型
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;  // 通常優先度
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;        // 特別フラグなし
    queueDesc.NodeMask = 0;                                    // 単一GPU使用

    ID3D12CommandQueue* commandQueue;
    HRESULT             hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

    if (FAILED(hr)) {
        OutputDebugString("Failed to create Command Queue\n");
        return nullptr;
    }

    // デバッグ用の名前設定（任意だが推奨）
    commandQueue->SetName(L"Main Command Queue");

    return commandQueue;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
IDXGISwapChain3* Dx12::CreateSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, HWND hwnd) {
    // スワップチェーンの詳細設定
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = 2;                                // ダブルバッファリング
    swapChainDesc.Width = 1280;                             // 画面幅
    swapChainDesc.Height = 720;                              // 画面高
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;       // ピクセルフォーマット
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // レンダーターゲット用
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    // 高速切替
    swapChainDesc.SampleDesc.Count = 1;                                // マルチサンプリングなし

    IDXGISwapChain1* swapChain1;
    HRESULT          hr = factory->CreateSwapChainForHwnd(
        commandQueue,    // コマンドキュー
        hwnd,            // ターゲットウィンドウ
        &swapChainDesc,  // 設定
        nullptr,         // フルスクリーン設定
        nullptr,         // 出力制限
        &swapChain1      // 作成されるスワップチェーン
    );

    if (FAILED(hr)) {
        OutputDebugString("Failed to create Swap Chain\n");
        return nullptr;
    }

    // より高機能なインターフェースにキャスト
    IDXGISwapChain3* swapChain;
    hr = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain));
    swapChain1->Release();

    if (FAILED(hr)) {
        OutputDebugString("Failed to cast to SwapChain3\n");
        return nullptr;
    }

    return swapChain;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
void Dx12::EnableDebugLayer() {
#if defined(_DEBUG)
    // デバッグインターフェースを取得
    ID3D12Debug* debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        // デバッグレイヤーを有効化
        debugController->EnableDebugLayer();

        // より詳細な検証を有効化（任意）
        ID3D12Debug1* debugController1;
        if (SUCCEEDED(debugController->QueryInterface(IID_PPV_ARGS(&debugController1)))) {
            debugController1->SetEnableGPUBasedValidation(TRUE);
        }
    }
#endif
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

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

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
    
    RenderTarget::~RenderTarget() {
        // レンダーターゲットリソースの解放
        for (auto& rt : renderTargets_) {
            if (rt) {
                rt->Release();
                rt = nullptr;
            }
        }
        renderTargets_.clear();
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	バックバッファを生成する
     * @param	device		デバイスクラスのインスタンス
     * @param	swapChain	スワップチェインのポインタ
     * @param	heap		ディスクリプターヒープのインスタンス
     * @return	生成の成否
     */
    [[nodiscard]] bool RenderTarget::createBackBuffer(const Device& device, const SwapChain& swapChain, const DescriptorHeap& heap) noexcept {
        // スワップチェインの設定を取得
        const auto& desc = swapChain.getDesc();

        // レンダーターゲットリソースのサイズを設定
        renderTargets_.resize(desc.BufferCount);

        // ディスクリプターヒープのハンドルを取得
        auto handle = heap.get()->GetCPUDescriptorHandleForHeapStart();

        // ディスクリプターヒープのタイプを取得
        auto heapType = heap.getType();
        assert(heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV && "ディスクリプタヒープのタイプが RTV ではありません");

        // バックバッファの生成
        for (uint8_t i = 0; i < desc.BufferCount; ++i) {
            const auto hr = swapChain.get()->GetBuffer(i, IID_PPV_ARGS(&renderTargets_[i]));
            if (FAILED(hr)) {
                assert(false && "バックバッファの取得に失敗しました");
                return false;
            }

            // レンダーターゲットビューを作成してディスクリプタヒープのハンドルと関連付ける
            device.get()->CreateRenderTargetView(renderTargets_[i], nullptr, handle);

            // 次のハンドルへ移動
            handle.ptr += device.get()->GetDescriptorHandleIncrementSize(heapType);
        }

        return true;
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	ビュー（ディスクリプタハンドル）を取得する
     * @param	heap	ディスクリプタヒープのインスタンス
     * @param	index	インデックス
     * @return	ディスクリプタハンドル
     */
    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::getDescriptorHandle(const Device& device, const DescriptorHeap& heap, UINT index) const noexcept {

        if (index >= renderTargets_.size() || !renderTargets_[index]) {
            assert(false && "不正なレンダーターゲットです");
        }

        // ディスクリプタヒープのハンドルを取得
        auto handle = heap.get()->GetCPUDescriptorHandleForHeapStart();

        // ディスクリプタヒープのタイプを取得
        auto heapType = heap.getType();
        assert(heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV && "ディスクリプタヒープのタイプが RTV ではありません");

        // インデックスに応じてハンドルを移動
        handle.ptr += index * device.get()->GetDescriptorHandleIncrementSize(heapType);
        return handle;
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	レンダーターゲットを取得する
     * @param	index	インデックス
     */
    [[nodiscard]] ID3D12Resource* RenderTarget::get(uint32_t index) const noexcept {
        if (index >= renderTargets_.size() || !renderTargets_[index]) {
            assert(false && "不正なレンダーターゲットです");
            return nullptr;
        }
        return renderTargets_[index];
    }

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

    // コマンドアロケータ制御クラス

#include "command_allocator.h"
#include <cassert>

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
    CommandAllocator::~CommandAllocator() {
        // コマンドアロケータの解放
        if (commandAllocator_) {
            commandAllocator_->Release();
            commandAllocator_ = nullptr;
        }
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドキューの生成
     * @param	device	デバイスクラスのインスタンス
     * @return	成功すれば true
     */
    [[nodiscard]] bool CommandAllocator::create(const Device& device, const D3D12_COMMAND_LIST_TYPE type) noexcept {

        // コマンドリストのタイプを設定
        type_ = type;

        // コマンドアロケータの生成
        const auto hr = device.get()->CreateCommandAllocator(type_, IID_PPV_ARGS(&commandAllocator_));
        if (FAILED(hr)) {
            assert(false && "コマンドアロケータの作成に失敗しました");
            return false;
        }

        return true;
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドアロケータをリセットする
     */
    void CommandAllocator::reset() noexcept {

        if (!commandAllocator_) {
            assert(false && "コマンドアロケータが未作成です");
        }

        commandAllocator_->Reset();
    }


    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドアロケータを取得する
     * @return	コマンドアロケータのポインタ
     */
    [[nodiscard]] ID3D12CommandAllocator* CommandAllocator::get() const noexcept {
        if (!commandAllocator_) {
            assert(false && "コマンドアロケータが未作成です");
            return nullptr;
        }
        return commandAllocator_;
    }
    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストのタイプを取得する
     * @return	コマンドリストのタイプ
     */
    [[nodiscard]] D3D12_COMMAND_LIST_TYPE CommandAllocator::getType() const noexcept {
        if (!commandAllocator_) {
            assert(false && "コマンドリストのタイプが未設定です");
        }
        return type_;
    }

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

    Fence::~Fence() {
        // フェンスの解放
        if (fence_) {
            fence_->Release();
            fence_ = nullptr;
        }
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	フェンスを作成する
     */
    [[nodiscard]] bool Fence::create(const Device& device) noexcept {

        // フェンスの生成
        HRESULT hr = device.get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
        if (FAILED(hr)) {
            assert(false && "フェンスの作成に失敗しました");
            return false;
        }
        // GPU 同期用のイベントハンドルを作成
        waitGpuEvent_ = CreateEvent(nullptr, false, false, "WAIT_GPU");
        if (!waitGpuEvent_) {
            assert(false && "GPU 同期用のイベントハンドルの作成に失敗しました");
            return false;
        }
        return true;
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	同期待ちを行う
     * @param fenceValue	フェンス値
     */
    void Fence::wait(UINT64 fenceValue) const noexcept {
        if (!fence_) {
            assert(false && "フェンスが未作成です");
            return;
        }

        // フェンスの値が指定された値に達するまで待機
        if (fence_->GetCompletedValue() < fenceValue) {
            // GPU がフェンス値に到達するまで待つ
            fence_->SetEventOnCompletion(fenceValue, waitGpuEvent_);
            WaitForSingleObject(waitGpuEvent_, INFINITE);
        }
    }

    //---------------------------------------------------------------------------------
    /**
     * @brief	フェンスを取得する
     */
    [[nodiscard]] ID3D12Fence* Fence::get() const noexcept {
        if (!fence_) {
            assert(false && "フェンスが未作成です");
            return nullptr;
        }
        return fence_;
    }