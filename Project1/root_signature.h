// ルートシグネチャクラス

#pragma once

#include "device.h"

//---------------------------------------------------------------------------------
/**
 * @brief	ルートシグネチャクラス
 */
class RootSignature final {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    コンストラクタ
     */
    RootSignature() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~RootSignature();

    //---------------------------------------------------------------------------------
    /**
     * @brief	ルートシグネチャを作成する
     * @param	device	デバイスクラスのインスタンス
     * @return	成功すれば true
     */
    [[nodiscard]] bool create(const Device& device) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ルートシグネチャを取得する
     * @return　ルートシグネチャ
     */
    [[nodiscard]] ID3D12RootSignature* get() const noexcept;

private:
    ID3D12RootSignature* rootSignature_{};  /// ルートシグネチャ
};
