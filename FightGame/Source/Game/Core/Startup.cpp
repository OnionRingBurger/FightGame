#include <windows.h>
#include "Defines.h"
#include "Main.h"
#include <stdio.h>
#include <crtdbg.h>
#include "DebugConsole.h"
#include <iostream>
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "DirectX.h"
#include "Sound.h"
#include "Texture.h"

// timeGetTime周りの使用
#pragma comment(lib, "winmm.lib")

//--- プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// エントリポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//--- 変数宣言
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG message;

	// ウィンドクラス情報の設定
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.hInstance = hInstance;
	wcex.lpszClassName = "Class Name";
	wcex.lpfnWndProc = WndProc;
	wcex.style = CS_CLASSDC | CS_DBLCLKS;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = wcex.hIcon;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	// ウィンドウクラス情報の登録
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, "Failed to RegisterClassEx", "Error", MB_OK);
		return 0;
	}

	// ウィンドウの作成
	RECT rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	DWORD style = WS_CAPTION | WS_SYSMENU;
	DWORD exStyle = WS_EX_OVERLAPPEDWINDOW;
	AdjustWindowRectEx(&rect, style, false, exStyle);
	const int windowWidth = rect.right - rect.left;
	const int windowHeight = rect.bottom - rect.top;

	// タスクバーを除いた作業領域の中央に配置
	RECT workArea = {};
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
	const int posX = workArea.left + (workArea.right - workArea.left - windowWidth) / 2;
	const int posY = workArea.top + (workArea.bottom - workArea.top - windowHeight) / 2;

	hWnd = CreateWindowEx(
		exStyle, wcex.lpszClassName,
		APP_TITLE, style,
		posX, posY,
		windowWidth, windowHeight,
		HWND_DESKTOP,
		NULL, hInstance, NULL
	);


	// デバッグコンソールの表示
#ifdef _DEBUG

	DebugConsole::InitDebugConsole(hWnd);

#endif


	// ウィンドウの表示
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetForegroundWindow(hWnd);

	MainGame game;

	HRESULT hr = InitDirectX(hWnd, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	if (FAILED(hr)) 
	{	
		UnregisterClass(wcex.lpszClassName, hInstance);
		return 0;
	}

	// 初期化処理
	game.Init();

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	auto a = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(GetDevice(), GetContext());

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // キーボード捜査
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	game.MainLoop(hWnd, message);

	// 終了時
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	game.Uninit();
	UninitDirectX();
	timeEndPeriod(1);
	UnregisterClass(wcex.lpszClassName, hInstance);

	return 0;
}

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}