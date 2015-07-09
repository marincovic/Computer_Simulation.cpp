#include<Windows.h>
#include"WindowClass.cpp"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hp, LPSTR cmdLine, int nShow)
{
	MainWindow main = MainWindow("simulation",hInstance);

	main.App_Run();

	return main.GetMSG().wParam;
}