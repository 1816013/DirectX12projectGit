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
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // OS����̃��b�Z�[�W��msg�Ɋi�[
		{
			TranslateMessage(&msg);	//���z�L�[�֘A�̕ϊ�
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
	wndclass.lpfnWndProc = (WNDPROC)WindowProcedure;	// �R�[���o�b�N�֐��̎w��
	wndclass.lpszClassName = className; // �A�v���P�[�V�����N���X��
	wndclass.hInstance = inst_;	// �n���h���̎擾
	RegisterClassEx(&wndclass);	// �A�v���P�[�V�����N���X(OS�\��)


	RECT wrc = { 0, 0, WINDOW_WIDTH, WINDOW_HIGHT };	// �E�C���h�E�T�C�Y�����߂�
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);	// �E�C���h�E�T�C�Y�̕␳
	
	windowH_ = CreateWindow(className,		// �N���X���w��
					L"DX12�e�X�g",			// �^�C�g���o�[�̕���
					WS_OVERLAPPEDWINDOW,	// �^�C�g���o�[�Ƌ��E��������E�C���h�E
					CW_USEDEFAULT,			// �\��X���W(OS�ɔC����)
					CW_USEDEFAULT,			// �\��Y���W(OS�ɔC����)
					wrc.right - wrc.left,	// �E�C���h�E���T�C�Y
					wrc.bottom - wrc.top,	// �E�C���h�E�c�T�C�Y
					nullptr,				// �e�E�C���h�E�n���h��	
					nullptr,				// ���j���[�n���h��
					inst_,					// �Ăяo���A�v���P�[�V�����n���h��	
					nullptr);				// �ǉ��p�����[�^�[


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


