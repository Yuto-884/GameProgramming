#include<windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:	//×ボタンが押されたとき
		PostQuitMessage(0);	//「アプリを終了します」とWindowsに通知
		return 0;
	case WM_PAINT:	//ウィンドウの再描画が必要な時
		//画面を更新する処理をここに書く
		return 0;
	case WM_KEYDOWN:	//キーが押されたとき
		//キー入力の処理をここに書く
		return 0;
	}
	//自分で処理しないメッセージはWindowsに任せる
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(
	HINSTANCE hInstance,		//アプリケーションの識別番号
	HINSTANCE hPrevInstance,	//基本使わなくていい
	LPSTR lpCmndLine,			//コマンドライン引数（起動時のオプション）
	int nCmndShow				//ウィンドウの表示方法（最大化、最小化など）
)

//ここにメインの処理を書く
//ウィンドウクラス登録
{
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc;			//ウィンドウプロシージャを指定(後述)
	wc.hInstance = hInstance;				//アプリケーションインスタンス
	wc.lpszClassName = "GameWindow";		//ウィンドウクラス名
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	//マウスカーソル
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//背景色

	RegisterClass(&wc);	//Windows1に登録

	//ウィンドウ作成
	HWND hwnd = CreateWindow(
		"GameWindow",			//ウィンドウ名
		"My Game",				//ウィンドウスタイル
		WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル
		CW_USEDEFAULT, CW_USEDEFAULT,	//位置（自動）
		500, 300,				//サイズ（幅、高さ）
		NULL, NULL,				//親ウィンドウ、メニュー
		hInstance,				//インスタンス
		NULL					//追加データ
	);
	ShowWindow(hwnd, nCmndShow);	//ウィンドウを表示


	//メッセージグループ
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))	//メッセージが来るまでで待機
	{
		TranslateMessage(&msg);	//キーボードメッセージを使いやすい形に変換
		DispatchMessage(&msg);	//適切なウィンドウプロシージャに送信
	}

	return 0;
}