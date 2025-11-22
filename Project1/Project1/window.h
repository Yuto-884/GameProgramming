// ウィンドウ制御クラス

#pragma once

#include <Windows.h>
#include <string>

//---------------------------------------------------------------------------------
/**
 * @brief	ウィンドウ制御クラス
 */
class Window final {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    コンストラクタ
     */
    Window() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~Window() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ウィンドウの生成
     * @param	instance	インスタンスハンドル
     * @param	width		横幅
     * @param	height		縦幅
     * @param	name		ウィンドウ名
     * @return	生成の成否
     */
    [[nodiscard]] HRESULT create(HINSTANCE instance, int width, int height, std::string_view name) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	メッセージループ
     */
    [[nodiscard]] bool messageLoop() const noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ウィンドウハンドルを取得する
     * @return	ウィンドウハンドル
     */
    [[nodiscard]] HWND handle() const noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ウィンドウのサイズを取得する
     * @return　ウィンドウのサイズ (横幅, 縦幅)
     */
    [[nodiscard]] std::pair<int, int> size() const noexcept;


private:
    HWND handle_{};  /// ウィンドウハンドル
    int  witdh_{};   /// ウィンドウの横幅
    int  height_{};  /// ウィンドウの縦幅
};