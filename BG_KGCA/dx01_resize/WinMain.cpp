#include "dx01.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	dx01 win;
	if (win.InitWindow(hInstance, L"BG Project!", 1024, 768) == true)
	{
		win.AppRun();
	}
	return 1;
}
