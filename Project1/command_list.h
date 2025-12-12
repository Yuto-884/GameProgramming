// コマンドリスト制御クラス

#pragma once

#include "device.h"
#include "command_allocator.h"

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリスト制御クラス
 */
class CommandList final {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    コンストラクタ
     */
    CommandList() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~CommandList();

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリスト作成
     * @param	device	デバイスクラスのインスタンス
     * @param	command	コマンドアロケータクラスのインスタンス
     * @return	生成の成否
     */
    [[nodiscard]] bool create(const Device& device, const CommandAllocator& commandAllocator) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストのリセット
     * @param	commandAllocator	コマンドアロケータクラスのインスタンス
     */
    void reset(const CommandAllocator& commandAllocator) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	コマンドリストを取得する
     * @return	コマンドリストのポインタ
     */
    [[nodiscard]] ID3D12GraphicsCommandList* get() const noexcept;


private:
    ID3D12GraphicsCommandList* commandList_{};       /// コマンドリスト
};
