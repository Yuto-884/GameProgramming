// フェンス（CPUとGPUの動機）制御クラス

#include "fence.h"
#include <cassert>

//---------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
Fence::~Fence() {
	// フェンスの解放
	if (fence_) {
		fence_->Release();
		fence_ = nullptr;
	}
}

//---------------------------------------------------------------------------------
/**
 * @brief	フェンスを作成する
 */
[[nodiscard]] bool Fence::create(const Device& device) noexcept {

	// フェンスの生成
	HRESULT hr = device.get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	if (FAILED(hr)) {
		assert(false && "フェンスの作成に失敗しました");
		return false;
	}
	// GPU 同期用のイベントハンドルを作成
	waitGpuEvent_ = CreateEvent(nullptr, false, false, "WAIT_GPU");
	if (!waitGpuEvent_) {
		assert(false && "GPU 同期用のイベントハンドルの作成に失敗しました");
		return false;
	}
	return true;
}

//---------------------------------------------------------------------------------
/**
 * @brief	同期待ちを行う
 * @param fenceValue	フェンス値
 */
void Fence::wait(UINT64 fenceValue) const noexcept {
	if (!fence_) {
		assert(false && "フェンスが未作成です");
		return;
	}

	// フェンスの値が指定された値に達するまで待機
	if (fence_->GetCompletedValue() < fenceValue) {
		// GPU がフェンス値に到達するまで待つ
		fence_->SetEventOnCompletion(fenceValue, waitGpuEvent_);
		WaitForSingleObject(waitGpuEvent_, INFINITE);
	}
}

//---------------------------------------------------------------------------------
/**
 * @brief	フェンスを取得する
 */
[[nodiscard]] ID3D12Fence* Fence::get() const noexcept {
	if (!fence_) {
		assert(false && "フェンスが未作成です");
		return nullptr;
	}
	return fence_;
}