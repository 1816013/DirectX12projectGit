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
/// �A�v���P�[�V�������Ǘ�����
/// �V���O���g���N���X
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
	/// �A�v���P�[�V�����̏�����
	/// </summary>
	/// <returns>true : ���� false : ���s</returns>
	bool Initialize();

	/// <summary>
	/// �A�v���P�[�V�����̎��s
	/// </summary>
	void Run();

	/// <summary>
	/// �A�v���P�[�V�����̏I������
	/// </summary>
	void Terminate();

	Size GetWindowSize();

	~Application();
private:
	Application();
	Application(const Application&) = delete;
	Application operator=(const Application&) = delete;

	/// <summary>
	/// �E�C���h�E������
	/// </summary>
	bool InitWindow();

	HINSTANCE inst_ = {};
	HWND windowH_ = {};

	std::unique_ptr<Dx12Wrapper>dx_;
};

