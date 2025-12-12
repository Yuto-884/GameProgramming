#include "window.h"

// ウィンドウプロシージャ（最小限）
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

HRESULT Window::create(HINSTANCE instance, int width, int height, std::string_view name) noexcept {

    // クラス名を設定
    const char* className = "MyWindowClass";

    // ウィンドウクラス設定
    WNDCLASSEXA wc{};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassExA(&wc)) {
        return E_FAIL;
    }

    // ウィンドウサイズの確定（クライアントサイズに調整）
    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    witdh_ = rect.right - rect.left;
    height_ = rect.bottom - rect.top;

    // ウィンドウ作成
    handle_ = CreateWindowA(
        className,
        name.data(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        witdh_,
        height_,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!handle_) {
        return E_FAIL;
    }

    ShowWindow(handle_, SW_SHOW);
    UpdateWindow(handle_);

    return S_OK;
}

bool Window::messageLoop() const noexcept {
    MSG msg{};
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

HWND Window::handle() const noexcept {
    return handle_;
}

std::pair<int, int> Window::size() const noexcept {
    return { witdh_, height_ };
}

