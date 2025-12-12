// パイプラインステートオブジェクトクラス

#pragma once

#include "device.h"
#include "shader.h"
#include "root_signature.h"

//---------------------------------------------------------------------------------
/**
 * @brief	パイプラインステートオブジェクトクラス
 */
class PiplineStateObject final {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    コンストラクタ
     */
    PiplineStateObject() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~PiplineStateObject();

    //---------------------------------------------------------------------------------
    /**
     * @brief	パイプラインステートオブジェクトを作成する
     * @param	device			デバイスクラスのインスタンス
     * @param	shader			シェーダクラスのインスタンス
     * @param	rootSignature	ルートシグネチャクラスのインスタンス
     * @return	成功すれば true
     */
    [[nodiscard]] bool create(const Device& device, const Shader& shader, const RootSignature& rootSignature) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	パイプラインステートを取得する
     * @return	パイプラインステートのポインタ
     */
    [[nodiscard]] ID3D12PipelineState* get() const noexcept;

private:
    ID3D12PipelineState* pipelineState_ = {};  ///< パイプラインステート
};
