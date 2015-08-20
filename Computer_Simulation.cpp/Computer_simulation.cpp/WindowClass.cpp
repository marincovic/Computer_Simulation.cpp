//#include<windows.h>
//#include<windowsx.h>
//#include"ErrorClass.cpp"
//
//class MainWindow{
//
//private:
//	HWND h_Window;
//	MSG m_message;
//
//public: 
//	MainWindow(char* ClassName, HINSTANCE hInstance)
//	{
//		if (!RegisterMyClass(hInstance, ClassName))
//			throw new RegistrationFailed();
//
//		h_Window = CreateWindow(ClassName, "Simulation", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
//			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
//	}
//	MainWindow(char* ClassName, HINSTANCE hInstance,HWND Parent)
//	{
//		if (!RegisterMyClass(hInstance, ClassName))
//			throw new RegistrationFailed();
//
//		h_Window = CreateWindow(ClassName, "Simulation", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
//			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, Parent, NULL, hInstance, NULL);
//	}
//
//	int RegisterMyClass(HINSTANCE hInstance, char* className)
//	{
//		WNDCLASS wc;
//		ZeroMemory(&wc, sizeof wc);
//
//		wc.lpfnWndProc = WndProc;
//		wc.hInstance = hInstance;
//		wc.lpszClassName = className;
//
//		wc.style = CS_HREDRAW | CS_VREDRAW;
//		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//
//		return RegisterClass(&wc);
//	}
//
//	HWND GetHWND()
//	{
//		return h_Window;
//	}
//
//	MSG GetMSG()
//	{
//		return m_message;
//	}
//
//	void SetMSG(MSG Message)
//	{
//		m_message = Message;
//	}
//	//
//	///callback funkcija za message
//	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//	{
//		switch (message)
//		{
//		case WM_DESTROY:
//			PostQuitMessage(0);
//			return 0;
//		}
//		return DefWindowProc(hwnd, message, wParam, lParam);
//	}
//};