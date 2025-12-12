#include "Window.h"
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    Window window;
    window.create(hInstance, 1280, 720, "Game");

    while (window.messageLoop()) {
        // ‚±‚±‚É•`‰æˆ—‚ğ‘‚­
    }
    return 0;
}
