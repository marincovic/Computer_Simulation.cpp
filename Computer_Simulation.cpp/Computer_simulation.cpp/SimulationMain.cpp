#include<Windows.h>
#include"D3Class.cpp"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	D3DClass main = D3DClass("simulation",hInstance);

	main.App_Run();

	return (int)main.GetMSG().wParam;
}