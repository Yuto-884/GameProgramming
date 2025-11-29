// スワップチェイン制御クラス

#include "swap_chain.h"

#include <cassert>

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
SwapChain::~SwapChain() {
    if (swapChain_) {
        swapChain_->Release();
        swapChain_ = nullptr;
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	スワップチェインの生成
 * @param	dxgi			dxgi クラスのインスタンス
 * @param	window			ウィンドウクラスのインスタンス
 * @param	commandQueue	コマンドキュークラスのインスタンス
 * @return	生成の成否
 */
[[nodiscard]] bool SwapChain::create(const Dx12& dxgi, const Window& window, const CommandQueue& commandQueue) noexcept {
    // ウィンドウサイズを取得
    const auto [w, h] = window.size();

    swapChainDesc_ = {};
    swapChainDesc_.BufferCount = 2;                                // バックバッファの数（ダブルバッファ）
    swapChainDesc_.Width = w;                                // バックバッファの横幅
    swapChainDesc_.Height = h;                                // バックバッファの縦幅
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;       // バックバッファのフォーマット
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // レンダーターゲットとして使用
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;    // 毎フレーム画面更新するので描画が終わったらバッファを破棄
    swapChainDesc_.SampleDesc.Count = 1;                                // マルチサンプリングなし

    // 一時的なスワップチェインの作成
    // スワップチェインのアップグレードが必要になる
    IDXGISwapChain1* tempSwapChain{};
    {
        const auto hr = dxgi.factory()->CreateSwapChainForHwnd(commandQueue.get(), window.handle(),
            &swapChainDesc_, nullptr, nullptr, &tempSwapChain);
        if (FAILED(hr)) {
            assert(false && "スワップチェインの作成に失敗");
            return false;
        }
    }

    // スワップチェインのバージョンをアップグレード
    {
        // 一時的なスワップチェインを IDXGISwapChain3 に変換
        const auto hr = tempSwapChain->QueryInterface(IID_PPV_ARGS(&swapChain_));
        // 一時的なスワップチェインは解放
        tempSwapChain->Release();

        if (FAILED(hr)) {
            assert(false && "スワップチェインのアップグレードに失敗");
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	スワップチェインを取得する
 * @return	スワップチェインのポインタ
 */
[[nodiscard]] IDXGISwapChain3* SwapChain::get() const noexcept {
    if (!swapChain_) {
        assert(false && "スワップチェインが未作成です");
        return nullptr;
    }
    return swapChain_;
}

//---------------------------------------------------------------------------------
/**
 * @brief	スワップチェインの設定を取得する
 * @return	スワップチェインの設定
 */
[[nodiscard]] const DXGI_SWAP_CHAIN_DESC1& SwapChain::getDesc() const noexcept {
    if (!swapChain_) {
        assert(false && "スワップチェインが未作成です");
    }
    return swapChainDesc_;
}
