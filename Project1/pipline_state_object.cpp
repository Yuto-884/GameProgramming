// パイプラインステートオブジェクトクラス

#include "pipline_state_object.h"
#include <cassert>

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
PiplineStateObject::~PiplineStateObject() {
    // パイプラインステートの解放
    if (pipelineState_) {
        pipelineState_->Release();
        pipelineState_ = nullptr;
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	パイプラインステートオブジェクトを作成する
 * @param	device			デバイスクラスのインスタンス
 * @param	shader			シェーダクラスのインスタンス
 * @param	rootSignature	ルートシグネチャクラスのインスタンス
 * @return	成功すれば true
 */
[[nodiscard]] bool PiplineStateObject::create(const Device& device, const Shader& shader, const RootSignature& rootSignature) noexcept {
    // 頂点レイアウト
    // 頂点バッファのフォーマットに合わせて設定する
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {"POSITION", 0,    DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {   "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    // ラスタライザステート
    // ポリゴンの塗りつぶし方法や裏面カリングの設定を行う
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.MultisampleEnable = false;
    rasterizerDesc.AntialiasedLineEnable = false;
    rasterizerDesc.ForcedSampleCount = 0;
    rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // ブレンドステート
    // 描画結果の合成方法を設定する
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
        FALSE,
        FALSE,
        D3D12_BLEND_ONE,
        D3D12_BLEND_ZERO,
        D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE,
        D3D12_BLEND_ZERO,
        D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    D3D12_BLEND_DESC blendDesc{};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
        blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
    }

    // パイプラインステート
    // 各種設定を構造体にまとめる
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = rootSignature.get();
    psoDesc.VS = { shader.vertexShader()->GetBufferPointer(), shader.vertexShader()->GetBufferSize() };
    psoDesc.PS = { shader.pixelShader()->GetBufferPointer(), shader.pixelShader()->GetBufferSize() };
    psoDesc.RasterizerState = rasterizerDesc;
    psoDesc.BlendState = blendDesc;
    psoDesc.DepthStencilState.DepthEnable = false;
    psoDesc.DepthStencilState.StencilEnable = false;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    auto res = device.get()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));
    if (FAILED(res)) {
        assert(false && "パイプラインステートの作成に失敗");
    }

    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	パイプラインステートを取得する
 * @return	パイプラインステートのポインタ
 */
[[nodiscard]] ID3D12PipelineState* PiplineStateObject::get() const noexcept {
    if (!pipelineState_) {
        assert(false && "パイプラインステートが未作成です");
    }
    return pipelineState_;
}