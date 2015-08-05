#include<windows.h>
#include<windowsx.h>
#include<iostream>
#include<assert.h>

#include<d3d11.h>

#include<d3d9.h>
#include<d3d11.h>
#include<dxgi.h>


#include"WindowClass.cpp"
#include"TimerClass.cpp"


/*
	Vecina komentara sam prepisao iz knjige kako bi se kasnije lakse snasao
*/

class D3DClass : public MainWindow
{
private:

	Time m_timer;

	UINT m_4MsaaQuality;
	LPRECT m_windowsCoordinates;
	UINT createDeviceFlags = 0;

	D3D_FEATURE_LEVEL featureLevel;
	DXGI_SWAP_CHAIN_DESC m_swapChainDesc;
	D3D11_TEXTURE2D_DESC m_depthStencilDesc;

	IDXGIDevice* m_dxgiDevice = 0;
	IDXGIAdapter* m_dxgiAdapter = 0;
	IDXGIFactory* m_dxgiFactory = 0;
	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_backBuffer;
	ID3D11Device* m_d3dDevice;
	ID3D11DeviceContext* m_d3dImmediateContext;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewPort;


public:
	D3DClass(char* ClassName, HINSTANCE hInstance) : MainWindow(ClassName, hInstance), m_timer(Time())
	{
		
		// Para 4.2.1 Creating  the device and context
		HRESULT D3D11Device = D3D11CreateDevice(
			0, //default adapter
			D3D_DRIVER_TYPE_HARDWARE,
			0, //software device we always specify null because we are using hardware for rendering.
			createDeviceFlags, //Optional device creatin flags 
			0, //array for feature level support
			0, // array for feature level support
			D3D11_SDK_VERSION,
			&m_d3dDevice, //Returns the created device
			&featureLevel, //returns the first supported feature level 
			&m_d3dImmediateContext //Returns the created devise context
			);

		//Checking if hardware supports 4x MSAA
		m_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4MsaaQuality);
		assert(m_4MsaaQuality > 0);


		if(!GetWindowRect(GetHWND(), m_windowsCoordinates))//Function used to get the coordinates of the window used to calculate width and height
			MessageBox(GetHWND(),("GetWindowREct function failed"),("error"),1);

		// Para 4.2.3 Desc the Swap Chain
		m_swapChainDesc.BufferDesc.Width = m_windowsCoordinates->right - m_windowsCoordinates->left;
		m_swapChainDesc.BufferDesc.Height = m_windowsCoordinates->bottom - m_windowsCoordinates->top;
		m_swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		m_swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		m_swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
		m_swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		m_swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		m_swapChainDesc.SampleDesc.Count = 4;
		m_swapChainDesc.SampleDesc.Quality = m_4MsaaQuality - 1;

		m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		m_swapChainDesc.BufferCount = 1;
		m_swapChainDesc.OutputWindow = GetHWND();
		m_swapChainDesc.Windowed = true;
		m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		m_swapChainDesc.Flags = 0;

		m_d3dDevice->QueryInterface(_uuidof(IDXGIDevice), (void**)&m_dxgiDevice);
		m_dxgiDevice->GetParent(_uuidof(IDXGIAdapter), (void**)&m_dxgiAdapter);
		m_dxgiAdapter->GetParent(_uuidof(IDXGIFactory), (void**)&m_dxgiFactory);
		m_dxgiFactory->CreateSwapChain(m_d3dDevice, &m_swapChainDesc, &m_swapChain);

		
		m_dxgiDevice->Release();
		m_dxgiAdapter->Release();
		m_dxgiFactory->Release();
		

		m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_backBuffer));
		m_d3dDevice->CreateRenderTargetView(m_backBuffer, 0, &m_renderTargetView);
		m_backBuffer->Release();

		m_depthStencilDesc.Width = m_windowsCoordinates->right - m_windowsCoordinates->left;
		m_depthStencilDesc.Height = m_windowsCoordinates->bottom - m_windowsCoordinates->top;
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
		m_d3dImmediateContext->OMSetRenderTargets
			(1, //number of targets to bind
			&m_renderTargetView, //Pointer to the first element in a array of render target view pointers to bind to the pipe line
			m_depthStencilView); //Pointer to the depth stencil view to bind to the pipeline;

		m_viewPort.TopLeftX = 0.0f;
		m_viewPort.TopLeftY = 0.0f;
		m_viewPort.Width = static_cast<float> (m_windowsCoordinates->right - m_windowsCoordinates->left);
		m_viewPort.Height = static_cast<float> (m_windowsCoordinates->bottom - m_windowsCoordinates->top);
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
		MainWindow::SetMSG({ 0 });

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
					/*UpdateScene(m_timer.DeltaTimer());
					DrawScene();*/
				}
				else
					Sleep(100);
			}
		}
		return (int)GetMSG().wParam;
	}

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	};
};