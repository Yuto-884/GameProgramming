// コマンドリスト制御クラス

#include "command_list.h"
#include <cassert>

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
CommandList::~CommandList() {
    // コマンドリストの解放
    if (commandList_) {
        commandList_->Release();
        commandList_ = nullptr;
    }
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリスト作成
 * @param	device	デバイスクラスのインスタンス
 * @param	command	コマンドアロケータクラスのインスタンス
 * @return	生成の成否
 */
[[nodiscard]] bool CommandList::create(const Device& device, const CommandAllocator& commandAllocator) noexcept {
    // コマンドリストの作成
    const auto hr = device.get()->CreateCommandList(0, commandAllocator.getType(), commandAllocator.get(), nullptr, IID_PPV_ARGS(&commandList_));
    if (FAILED(hr)) {
        assert(false && "コマンドリストの作成に失敗しました");
        return false;
    }

    // コマンドリストを初期化状態に設定
    commandList_->Close();
    return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストのリセット
 * @param	commandAllocator	コマンドアロケータクラスのインスタンス
 */
void CommandList::reset(const CommandAllocator& commandAllocator) noexcept {
    if (!commandList_) {
        assert(false && "コマンドリストが未作成です");
    }

    // コマンドリストをリセット
    commandList_->Reset(commandAllocator.get(), nullptr);
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストを取得する
 * @return	コマンドリストのポインタ
 */
[[nodiscard]] ID3D12GraphicsCommandList* CommandList::get() const noexcept {
    if (!commandList_) {
        assert(false && "コマンドリストが未作成です");
        return nullptr;
    }
    return commandList_;
}