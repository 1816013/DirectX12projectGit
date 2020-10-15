#pragma once

#include <Windows.h>
#include <memory>

struct Size
{
	size_t width;
	size_t hight;
};

class Dx12Wrapper;
/// <summary>
/// アプリケーションを管理する
/// シングルトンクラス
/// </summary>
class Application
{
public:
	static Application& GetInstance()
	{
		static Application app;
		return app;
	}

	/// <summary>
	/// アプリケーションの初期化
	/// </summary>
	/// <returns>true : 成功 false : 失敗</returns>
	bool Initialize();

	/// <summary>
	/// アプリケーションの実行
	/// </summary>
	void Run();

	/// <summary>
	/// アプリケーションの終了処理
	/// </summary>
	void Terminate();

	Size GetWindowSize();

	~Application();
private:
	Application();
	Application(const Application&) = delete;
	Application operator=(const Application&) = delete;

	/// <summary>
	/// ウインドウ初期化
	/// </summary>
	bool InitWindow();

	HINSTANCE inst_ = {};
	HWND windowH_ = {};

	std::unique_ptr<Dx12Wrapper>dx_;
};

