// DXGI 制御クラス

#include "DXGI.h"
#include <cassert>

#pragma comment(lib, "dxgi.lib")

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
DXGI::~DXGI() {
    // DXGIファクトリーの解放
    if (dxgiFactory_) {
        dxgiFactory_->Release();
        dxgiFactory_ = nullptr;
    }
    // アダプタの解放
    if (dxgiAdapter_) {
        dxgiAdapter_->Release();
        dxgiAdapter_ = nullptr;
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	アダプタの設定
 * @return	情報が正しく取得できた場合は true
 */
[[nodiscard]] bool DXGI::setDisplayAdapter() noexcept {
#if _DEBUG
    // デバッグレイヤーをオンに
    // これを行う事で、DirectXのエラー内容をより詳細に知ることができる
    ID3D12Debug* debug;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
        debug->EnableDebugLayer();
    }
#endif

    // DXGIファクトリーの作成
    {
        UINT createFactoryFlags = 0;
#if _DEBUG
        createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
        const auto hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory_));
        if (FAILED(hr)) {
            assert(false && "DXGIファクトリーの作成に失敗");
            return false;
        }
    }

    // アダプタの取得
    {
        // アダプタを列挙
        auto           select = 0;
        IDXGIAdapter1* dxgiAdapter{};

        // 適切なアダプタを選ぶ
        while (dxgiFactory_->EnumAdapters1(select, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND) {

            DXGI_ADAPTER_DESC1 desc{};
            dxgiAdapter->GetDesc1(&desc);

            select++;

            // ソフトウェアアダプタは除外
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                dxgiAdapter->Release();
                continue;
            }

            // Direct3D12 が動かない場合も除外
            if (FAILED(D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
                dxgiAdapter->Release();
                continue;
            }

            dxgiAdapter_ = dxgiAdapter;
            break;
        }

        if (!dxgiAdapter_) {
            assert(false && "アダプタの取得に失敗");
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	dxgi ファクトリーを取得する
 * @return	dxgi ファクトリーのポインタ
 */
[[nodiscard]] IDXGIFactory4* DXGI::factory() const noexcept {
    if (!dxgiFactory_) {
        assert(false && "DXGIファクトリーが未作成です");
        return nullptr;
    }

    return dxgiFactory_;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ディスプレイアダプターを取得する
 * @return	ディスプレイアダプターのポインタ
 */
[[nodiscard]] IDXGIAdapter1* DXGI::displayAdapter() const noexcept {
    if (!dxgiAdapter_) {
        assert(false && "ディスプレイアダプターが未作成です");
        return nullptr;
    }

    return dxgiAdapter_;
}

