#include<windows.h>
#include<windowsx.h>
#include<iostream>
#include<assert.h>

#include<d3d11.h>
#include<d3d9.h>
#include<dxgi.h>


#include"TimerClass.cpp"
#include"ErrorClass.cpp"




/*
	Vecina komentara sam prepisao iz knjige kako bi se kasnije lakse snasao
*/

class D3DClass
{
	HWND h_Window;
	MSG m_message;
	HRESULT hr;
	ULONG err_code;

	Time m_timer;

	UINT m_4MsaaQuality;
	WINDOWINFO m_windowsInfo;
	UINT createDeviceFlags = 0;

	D3D_FEATURE_LEVEL featureLevel;
	DXGI_SWAP_CHAIN_DESC m_swapChainDesc;
	D3D11_TEXTURE2D_DESC m_depthStencilDesc;


	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_backBuffer;
	ID3D11Device* m_d3dDevice;
	ID3D11DeviceContext* m_d3dImmediateContext;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewPort;


public:
	D3DClass(char* ClassName, HINSTANCE hInstance) : m_timer(Time())
	{
		if (!RegisterMyClass(hInstance, ClassName))
			throw new RegistrationFailed();

		h_Window = CreateWindow(ClassName, "Simulation", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
#ifdef DEBUG || _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		// Para 4.2.1 Creating  the device and context
		HRESULT D3D11Device = D3D11CreateDevice(
			NULL, //default adapter
			D3D_DRIVER_TYPE_HARDWARE,
			NULL, //software device we always specify null because we are using hardware for rendering.
			createDeviceFlags, //Optional device creatin flags 
			NULL, //array for feature level support
			NULL, // array for feature level support
			D3D11_SDK_VERSION,
			&m_d3dDevice, //Returns the created device
			&featureLevel, //returns the first supported feature level 
			&m_d3dImmediateContext //Returns the created devise context
			);
			
		if (FAILED(D3D11Device))
		{
			err_code = HRESULT_CODE(D3D11Device);
		}

		if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		{
			MessageBox(0, "Not supporting DirectX 11 feature Level", 0, 0);
		}

		
		//Checking if hardware supports 4x MSAA
		m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4MsaaQuality);
		assert(m_4MsaaQuality > 0);


		if (!GetWindowInfo(GetHWND(), &m_windowsInfo))//Function used to get the coordinates of the window used to calculate width and height
		{
			MessageBox(GetHWND(), ("GetWindowRect function failed"), ("error"), 1);
		}
		UINT window_width = m_windowsInfo.rcWindow.right - m_windowsInfo.rcWindow.left;
		UINT window_height = m_windowsInfo.rcWindow.bottom - m_windowsInfo.rcWindow.top;

		// Para 4.2.3 Desc the Swap Chain
		ZeroMemory(&m_swapChainDesc, sizeof(m_swapChainDesc));
		m_swapChainDesc.BufferDesc.Width = window_width;
		m_swapChainDesc.BufferDesc.Height = window_height;
		m_swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		m_swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		m_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		m_swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		m_swapChainDesc.SampleDesc.Count = 1;
		m_swapChainDesc.SampleDesc.Quality = 0;

		m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		m_swapChainDesc.BufferCount = 1;
		m_swapChainDesc.OutputWindow = GetHWND();
		m_swapChainDesc.Windowed = true;
		m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		m_swapChainDesc.Flags = 0;

		IDXGIDevice* m_dxgiDevice = 0;
		if (FAILED(hr = m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgiDevice)));
		{
			err_code = HRESULT_CODE(hr);
		}

		IDXGIAdapter* m_dxgiAdapter = 0;
		if (FAILED(hr = m_dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgiAdapter)))
		{
			err_code = HRESULT_CODE(hr);
		}

		IDXGIFactory* m_dxgiFactory = 0;
		if (FAILED(hr = m_dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgiFactory)))
		{
			err_code = HRESULT_CODE(hr);
		}

		if (FAILED(hr = m_dxgiFactory->CreateSwapChain(m_d3dDevice, &m_swapChainDesc, &m_swapChain)))
		{
			err_code=HRESULT_CODE(hr);
		}
		
		m_dxgiDevice->Release();
		m_dxgiAdapter->Release();
		m_dxgiFactory->Release();
		

		if (FAILED(hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_backBuffer))))	err_code = HRESULT_CODE(hr);
		if (FAILED(hr = m_d3dDevice->CreateRenderTargetView(m_backBuffer, 0, &m_renderTargetView))) err_code = HRESULT_CODE(hr);
		m_backBuffer->Release();

		m_depthStencilDesc.Width = window_width;
		m_depthStencilDesc.Height = window_height;
		m_depthStencilDesc.MipLevels = 1;
		m_depthStencilDesc.ArraySize = 1;
		m_depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		//must mach the swap chan MSAA values
		m_depthStencilDesc.SampleDesc.Count = 4;
		m_depthStencilDesc.SampleDesc.Quality = m_4MsaaQuality - 1;

		m_depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		m_depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		m_depthStencilDesc.CPUAccessFlags = 0;
		m_depthStencilDesc.MiscFlags = 0;
		m_d3dDevice->CreateTexture2D(
			&m_depthStencilDesc, //description of textures to create
			0,
			&m_depthStencilBuffer); //returns pointer to depth stencil buffer
		
		m_d3dDevice->CreateDepthStencilView(
			m_depthStencilBuffer, //Resurce we want to create a view to
			0,
			&m_depthStencilView); //return depth stencil view
		
		m_d3dImmediateContext->OMSetRenderTargets(
			1, //number of targets to bind
			&m_renderTargetView, //Pointer to the first element in a array of render target view pointers to bind to the pipe line
			m_depthStencilView); //Pointer to the depth stencil view to bind to the pipeline;

		m_viewPort.TopLeftX = 0.0f;
		m_viewPort.TopLeftY = 0.0f;
		m_viewPort.Width = static_cast<float> (window_width);
		m_viewPort.Height = static_cast<float> (window_height);
		m_viewPort.MinDepth = 0.0f;
		m_viewPort.MaxDepth = 1.0f;

		m_d3dImmediateContext->RSSetViewports(1, &m_viewPort);
	}

	bool AppPaused()
	{
		return m_timer.GetStoped();
	}

	void CalculateFrameStats()
	{
		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		if ((m_timer.TotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt;
			float mspf = 100.0f / fps;
			//Podcrtava iz nepoznatog razloga
			//std::wostringstream outs;
			//outs.precision(6);
			//outs << "Simulation" << L" " << L"FPS" << fps << L" " << L"Frame Time" << mspf << L"(ms)";
			//SetWindowText(GetHWND(), outs.str())

			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}

	int App_Run()
	{
		SetMSG({ 0 });

		m_timer.Reset();

		while (GetMSG().message != WM_QUIT)
		{
			if (PeekMessage(&GetMSG(), 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&GetMSG());
				DispatchMessage(&GetMSG());
			}

			else
			{
				m_timer.Tick();
				if (!AppPaused())
				{
					CalculateFrameStats();
					UpdateScene(m_timer.DeltaTimer());
					DrawScene();
				}
				else
					Sleep(100);
			}
		}
		return (int)GetMSG().wParam;
	}

	void UpdateScene(float dt)
	{

	};

	void DrawScene()
	{
		m_d3dImmediateContext->ClearRenderTargetView(m_renderTargetView, reinterpret_cast<const float*> D3DCOLOR_ARGB(255, 0, 0, 255));
		m_d3dImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		m_swapChain->Present(0, 0);
	}

	int RegisterMyClass(HINSTANCE hInstance, char* className)
	{
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof wc);

		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = className;

		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

		return RegisterClass(&wc);
	}

	HWND GetHWND()
	{
		return h_Window;
	}

	MSG GetMSG()
	{
		return m_message;
	}

	void SetMSG(MSG Message)
	{
		m_message = Message;
	}

	///callback funkcija za message
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

};