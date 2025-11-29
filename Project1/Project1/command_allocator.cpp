// コマンドアロケータ制御クラス

#include "command_allocator.h"
#include <cassert>

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
CommandAllocator::~CommandAllocator() {
	// コマンドアロケータの解放
	if (commandAllocator_) {
		commandAllocator_->Release();
		commandAllocator_ = nullptr;
	}
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドキューの生成
 * @param	device	デバイスクラスのインスタンス
 * @return	成功すれば true
 */
[[nodiscard]] bool CommandAllocator::create(const Device& device, const D3D12_COMMAND_LIST_TYPE type) noexcept {

	// コマンドリストのタイプを設定
	type_ = type;

	// コマンドアロケータの生成
	const auto hr = device.get()->CreateCommandAllocator(type_, IID_PPV_ARGS(&commandAllocator_));
	if (FAILED(hr)) {
		assert(false && "コマンドアロケータの作成に失敗しました");
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	コマンドアロケータをリセットする
 */
void CommandAllocator::reset() noexcept {

	if (!commandAllocator_) {
		assert(false && "コマンドアロケータが未作成です");
	}

	commandAllocator_->Reset();
}


//---------------------------------------------------------------------------------
/**
 * @brief	コマンドアロケータを取得する
 * @return	コマンドアロケータのポインタ
 */
[[nodiscard]] ID3D12CommandAllocator* CommandAllocator::get() const noexcept {
	if (!commandAllocator_) {
		assert(false && "コマンドアロケータが未作成です");
		return nullptr;
	}
	return commandAllocator_;
}
//---------------------------------------------------------------------------------
/**
 * @brief	コマンドリストのタイプを取得する
 * @return	コマンドリストのタイプ
 */
[[nodiscard]] D3D12_COMMAND_LIST_TYPE CommandAllocator::getType() const noexcept {
	if (!commandAllocator_) {
		assert(false && "コマンドリストのタイプが未設定です");
	}
	return type_;
}