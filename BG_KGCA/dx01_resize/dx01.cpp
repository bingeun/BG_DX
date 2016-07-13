#include "dx01.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	dx01 core;
	if (core.InitWindow(hInstance, L"BG Project!", 1024, 768))
	{
		core.AppRun();
	}
	return 1;
}

dx01::dx01()
{
}

dx01::~dx01()
{
}

bool dx01::Init()
{
	return true;
}

bool dx01::Frame()
{
	return true;
}

bool dx01::Render()
{
	return true;
}

bool dx01::Release()
{
	return true;
}
