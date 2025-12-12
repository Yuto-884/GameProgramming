// ルートシグネチャクラス

#include "root_signature.h"
#include <cassert>

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
RootSignature::~RootSignature() {
    // コマンドアロケータの解放
    if (rootSignature_) {
        rootSignature_->Release();
        rootSignature_ = nullptr;
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	ルートシグネチャを作成する
 * @param	device	デバイスクラスのインスタンス
 * @return	成功すれば true
 */
[[nodiscard]] bool RootSignature::create(const Device& device) noexcept {
    // 描画に必要なリソースをシェーダに伝える
    // 今回は特にリソースを利用しないので空で用意
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.NumParameters = 0;
    rootSignatureDesc.pParameters = nullptr;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // ルートシグネチャのシリアライズ
    ID3DBlob* signature{};
    auto      res = D3D12SerializeRootSignature(
        &rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &signature,
        nullptr);

    bool success = SUCCEEDED(res);
    if (!success) {
        assert(false && "ルートシグネチャのシリアライズに失敗");
    }
    else {
        // ルートシグネチャの生成
        res = device.get()->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&rootSignature_));

        success &= SUCCEEDED(res);
        if (!success) {
            assert(false && "ルートシグネチャの生成に失敗");
        }
    }

    if (signature) {
        signature->Release();
    }

    return success;
}

//---------------------------------------------------------------------------------
/**
 * @brief	ルートシグネチャを取得する
 * @return　ルートシグネチャ
 */
[[nodiscard]] ID3D12RootSignature* RootSignature::get() const noexcept {
    if (!rootSignature_) {
        assert(false && "ルートシグネチャが生成されていません");
    }

    return rootSignature_;
}
