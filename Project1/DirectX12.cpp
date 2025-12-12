#include "Dx12.h"

#pragma comment(lib, "d3d12.lib") 
#pragma comment(lib, "dxgi.lib")

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