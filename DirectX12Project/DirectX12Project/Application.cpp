#include "Application.h"
#include <iostream>
#include "DirectX12/Dx12Wrapper.h"

using namespace std;

constexpr auto className = L"DirectXtest";
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HIGHT = 720;

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool Application::Initialize()
{

	if (!InitWindow())
	{
		return false;
	}
	dx_ = make_unique<Dx12Wrapper>();
	if (!dx_->Init(windowH_))
	{
		return false;
	}
	return true;
}

void Application::Run()
{
	ShowWindow(windowH_, SW_SHOW);
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // OSからのメッセージをmsgに格納
		{
			TranslateMessage(&msg);	//仮想キー関連の変換
			DispatchMessage(&msg);
		}
		if (!dx_->Update())
		{
			break;
		}
	}	
}

void Application::Terminate()
{	
	dx_->Terminate();
	UnregisterClass(className, inst_);
}

Size Application::GetWindowSize()
{
	return { WINDOW_WIDTH, WINDOW_HIGHT };
}

Application::~Application()
{

}

Application::Application()
{
}

bool Application::InitWindow()
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	inst_ = GetModuleHandle(0);
	WNDCLASSEX wndclass = {};
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.lpfnWndProc = (WNDPROC)WindowProcedure;	// コールバック関数の指定
	wndclass.lpszClassName = className; // アプリケーションクラス名
	wndclass.hInstance = inst_;	// ハンドルの取得
	RegisterClassEx(&wndclass);	// アプリケーションクラス(OS予約)


	RECT wrc = { 0, 0, WINDOW_WIDTH, WINDOW_HIGHT };	// ウインドウサイズを決める
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);	// ウインドウサイズの補正
	
	windowH_ = CreateWindow(className,		// クラス名指定
					L"DX12テスト",			// タイトルバーの文字
					WS_OVERLAPPEDWINDOW,	// タイトルバーと境界線があるウインドウ
					CW_USEDEFAULT,			// 表示X座標(OSに任せる)
					CW_USEDEFAULT,			// 表示Y座標(OSに任せる)
					wrc.right - wrc.left,	// ウインドウ横サイズ
					wrc.bottom - wrc.top,	// ウインドウ縦サイズ
					nullptr,				// 親ウインドウハンドル	
					nullptr,				// メニューハンドル
					inst_,					// 呼び出しアプリケーションハンドル	
					nullptr);				// 追加パラメーター


	if (windowH_ == nullptr)
	{
		LPVOID messageBuffer = nullptr;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&messageBuffer,
			0,
			nullptr);
			OutputDebugString((TCHAR*)messageBuffer);
			cout << (TCHAR*)messageBuffer << endl;
			LocalFree(messageBuffer);
			return false;
	}
	return true;
}


