#include <Windows.h>
#include "Application.h"

int WINAPI WinMain(_In_ HINSTANCE inst,_In_opt_ HINSTANCE prev,_In_ LPSTR,_In_ int)
{
	auto& app = Application::GetInstance();
	if(!app.Initialize())
	{
		return -1;
	}
	app.Run();
	app.Terminate();
	return 0;
}
