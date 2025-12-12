// スワップチェイン制御クラス

#pragma once

#include "DXGI.h"
#include "command_queue.h"
#include "../window/window.h"

//---------------------------------------------------------------------------------
/**
 * @brief	スワップチェイン制御クラス
 */
class SwapChain final {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    コンストラクタ
     */
    SwapChain() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~SwapChain();

    //---------------------------------------------------------------------------------
    /**
     * @brief	スワップチェインの生成
     * @param	dxgi			dxgi クラスのインスタンス
     * @param	window			ウィンドウクラスのインスタンス
     * @param	commandQueue	コマンドキュークラスのインスタンス
     * @return	生成の成否
     */
    [[nodiscard]] bool create(const DXGI& dxgi, const Window& window, const CommandQueue& commandQueue) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	スワップチェインを取得する
     * @return	スワップチェインのポインタ
     */
    [[nodiscard]] IDXGISwapChain3* get() const noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	スワップチェインの設定を取得する
     * @return	スワップチェインの設定
     */
    [[nodiscard]] const DXGI_SWAP_CHAIN_DESC1& getDesc() const noexcept;


private:
    IDXGISwapChain3* swapChain_{};      /// スワップチェイン
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};  /// スワップチェインの設定
};
