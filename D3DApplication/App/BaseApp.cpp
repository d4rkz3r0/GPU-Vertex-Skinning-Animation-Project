#include "BaseApp.h"
#include "AppHelpers/Misc/Exception.h"
#include "AppHelpers/Components/RenderableComponent.h"
#include <sstream>

//Landscape Monitor
//const UINT BaseApp::DefaultScreenWidth = 1600;
//const UINT BaseApp::DefaultScreenHeight = 900;

//Vertical Monitor
const UINT BaseApp::DefaultScreenWidth = 1078;
const UINT BaseApp::DefaultScreenHeight = 608;

const UINT BaseApp::DefaultFrameRate = 60;
const UINT BaseApp::DefaultMSAASampleCount = 4;

BaseApp::BaseApp(HINSTANCE hInstance, const wstring& WindowClassName, const wstring& windowTitle, int showCMD) :
	mClock(), mTime(), mhInstance(hInstance), mWindowClassName(WindowClassName),
	mWindowTitle(windowTitle), mShowCMD(showCMD), mWindowHandle(), mWindowClass(),
	mScreenWidth(DefaultScreenWidth), mScreenHeight(DefaultScreenHeight),
	mDirect3DDevice(nullptr), mDirect3DDeviceContext(nullptr), mDepthStencilView(nullptr), mDepthStencilBuffer(nullptr),
	mViewPort(), mFeatureLevel(D3D_FEATURE_LEVEL_9_1), mFrameRate(DefaultFrameRate), mMSAASampleCount(DefaultMSAASampleCount),
	mMSAAQualityLevels(0), mIsFullScreen(false), mIsDepthStencilBufferEnabled(true), mMSAAEnabled(false) { }

HINSTANCE BaseApp::GetInstance() const
{
	return mhInstance;
}

HWND BaseApp::GetWindowHandle() const
{
	return mWindowHandle;
}

const WNDCLASSEX& BaseApp::GetWindowClass() const
{
	return mWindowClass;
}

const wstring& BaseApp::GetWindowClassName() const
{
	return mWindowClassName;
}

const wstring& BaseApp::GetWindowTitle() const
{
	return mWindowTitle;
}

int BaseApp::GetScreenWidth() const
{
	return mScreenWidth;
}

int BaseApp::GetScreenHeight() const
{
	return mScreenHeight;
}

ID3D11Device1* BaseApp::D3DDevice() const
{
	return mDirect3DDevice;
}

ID3D11RenderTargetView* BaseApp::BackBufferRTV() const
{
	return mRenderTargetView;
}

ID3D11DepthStencilView* BaseApp::BackBufferDSV() const
{
	return mDepthStencilView;
}

ID3D11DeviceContext1* BaseApp::D3DDevCon() const
{
	return mDirect3DDeviceContext;
}

float BaseApp::GetAspectRatio() const
{
	return static_cast<float>(static_cast<float>(mScreenWidth) / static_cast<float>(mScreenHeight));
}

bool BaseApp::IsFullScreen() const
{
	return mIsFullScreen;
}

bool BaseApp::IsDepthStencilBufferEnabled() const
{
	return mMSAAEnabled;
}

const D3D11_TEXTURE2D_DESC& BaseApp::GetBackBufferDesc() const
{
	return mBackBufferDesc;
}

const D3D11_VIEWPORT& BaseApp::GetViewPort() const
{
	return mViewPort;
}

const vector<BaseComponent*>& BaseApp::GetComponents() const
{
	return mComponents;
}

const ServiceContainer& BaseApp::GetServices() const
{
	return mServices;
}

void BaseApp::Run()
{
	InitWin32();
	InitD3D11();
	Initialize();

	//Win32 Msg Loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	mClock.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mClock.UpdateTime(mTime);
			CalcFPS();
			Update(mTime);
			Draw(mTime);
		}
	}

	Shutdown();
}

void BaseApp::Initialize()
{
	for (auto aComponent : mComponents)
	{
		aComponent->Initialize();
	}
}

void BaseApp::Update(const Time& Time)
{
	for (auto aComponent : mComponents)
	{
		if (aComponent->IsEnabled())
		{
			aComponent->Update(Time);
		}
	}
}

void BaseApp::Draw(const Time& deltaTime)
{
	for (auto aComponent : mComponents)
	{
		//RTTI Casting to call Draw()for Renderables
		//As Usage -> If I am a RenderableComponent, then return me as a valid pointer,
		//otherwise return nullptr since I have no Draw() functionality. 
		RenderableComponent* renderableComponent = aComponent->As<RenderableComponent>();

		if (renderableComponent != nullptr && renderableComponent->IsVisible())
		{
			renderableComponent->Draw(deltaTime);
		}
	}
}

void BaseApp::InitWin32()
{
	ZeroMemory(&mWindowClass, sizeof(mWindowClass));
	mWindowClass.cbSize = sizeof(WNDCLASSEX);
	mWindowClass.style = CS_CLASSDC;
	mWindowClass.lpfnWndProc = WndProc;
	mWindowClass.hInstance = mhInstance;
	mWindowClass.lpszClassName = mWindowClassName.c_str();
	mWindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	mWindowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	mWindowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	mWindowClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);

	RECT windowRectangle = { 0, 0, static_cast<LONG>(mScreenWidth), static_cast<LONG>(mScreenHeight) };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);
	RegisterClassEx(&mWindowClass);

	POINT centerOfWindow = CenterOfWindow(mScreenWidth, mScreenHeight);
	mWindowHandle = CreateWindow(mWindowClassName.c_str(), mWindowTitle.c_str(), WS_OVERLAPPEDWINDOW,
		-7, centerOfWindow.y, windowRectangle.right - windowRectangle.left,
		windowRectangle.bottom - windowRectangle.top, nullptr, nullptr, mhInstance, nullptr);

	ShowWindow(mWindowHandle, mShowCMD);
	UpdateWindow(mWindowHandle);
}

void BaseApp::InitD3D11()
{
	HRESULT hr;
	UINT deviceCreationFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dDevCon = nullptr;

	if (FAILED(hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceCreationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &d3dDevice, &mFeatureLevel, &d3dDevCon)))
	{
		throw Exception("BaseApp::InitD3D11()->D3D11CreateDevice() failed.", hr);
	}

	if (FAILED(hr = d3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&mDirect3DDevice))))
	{
		throw Exception("BaseApp::InitD3D11()BaseApp::D3DDevice->QueryInterface() failed.", hr);
	}

	if (FAILED(hr = d3dDevCon->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&mDirect3DDeviceContext))))
	{
		throw Exception("BaseApp::InitD3D11()BaseApp::D3DDeviceCon->QueryInterface() failed.", hr);
	}

	ReleaseObject(d3dDevice);
	ReleaseObject(d3dDevCon);

	mDirect3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, mMSAASampleCount, &mMSAAQualityLevels);

	if (mMSAAQualityLevels == 0)
	{
		throw Exception("BaseApp::InitD3D11(), MSAA not supported.", hr);
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMem(swapChainDesc);
	swapChainDesc.Width = mScreenWidth;
	swapChainDesc.Height = mScreenHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (mMSAAEnabled)
	{
		swapChainDesc.SampleDesc.Count = mMSAASampleCount;
		swapChainDesc.SampleDesc.Quality = mMSAAQualityLevels - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGIDevice* dxgiDevice = nullptr;
	if (FAILED(hr = mDirect3DDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice))))
	{
		throw Exception("BaseApp::InitD3D11()BaseApp::D3DDevice->QueryInterface() failed.", hr);
	}

	IDXGIAdapter* dxgiAdapter = nullptr;
	if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter))))
	{
		ReleaseObject(dxgiDevice);
		throw Exception("BaseApp::InitD3D11()BaseApp::D3DDevice->GetParent() failed.", hr);
	}

	IDXGIFactory2* dxgiFactory = nullptr;
	if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory))))
	{
		ReleaseObject(dxgiDevice);
		ReleaseObject(dxgiAdapter);
		throw Exception("BaseApp::InitD3D11()BaseApp::D3DDevice->GetParent() failed.", hr);
	}

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc;
	ZeroMemory(&fullScreenDesc, sizeof(fullScreenDesc));
	fullScreenDesc.RefreshRate.Numerator = mFrameRate;
	fullScreenDesc.RefreshRate.Denominator = 1;
	fullScreenDesc.Windowed = !mIsFullScreen;

	//DXGI v1.2 Function
	if (FAILED(hr = dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, mWindowHandle, &swapChainDesc, &fullScreenDesc, nullptr, &mSwapChain)))
	{
		ReleaseObject(dxgiDevice);
		ReleaseObject(dxgiAdapter);
		ReleaseObject(dxgiFactory);
		throw Exception("IDXGIDeviceBaseApp::CreateSwapChainForHwnd() failed.", hr);
	}

	ReleaseObject(dxgiDevice);
	ReleaseObject(dxgiAdapter);
	ReleaseObject(dxgiFactory);

	ID3D11Texture2D* backBuffer;
	if (FAILED(hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
	{
		throw Exception("IDXGISwapChainBaseApp::GetBuffer() failed.", hr);
	}

	backBuffer->GetDesc(&mBackBufferDesc);

	if (FAILED(hr = mDirect3DDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView)))
	{
		ReleaseObject(backBuffer);
		throw Exception("IDXGIDeviceBaseApp::CreateRenderTargetView() failed.", hr);
	}

	ReleaseObject(backBuffer);

	if (mIsDepthStencilBufferEnabled)
	{
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.Width = mScreenWidth;
		depthStencilDesc.Height = mScreenHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;

		if (mMSAAEnabled)
		{
			depthStencilDesc.SampleDesc.Count = mMSAASampleCount;
			depthStencilDesc.SampleDesc.Quality = mMSAAQualityLevels - 1;
		}
		else
		{
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}

		if (FAILED(hr = mDirect3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, &mDepthStencilBuffer)))
		{
			throw Exception("IDXGIDeviceBaseApp::CreateTexture2D() failed.", hr);
		}

		if (FAILED(hr = mDirect3DDevice->CreateDepthStencilView(mDepthStencilBuffer, nullptr, &mDepthStencilView)))
		{
			throw Exception("IDXGIDeviceBaseApp::CreateDepthStencilView() failed.", hr);
		}
	}

	//ViewPort
	mViewPort.TopLeftX = 0.0f;
	mViewPort.TopLeftY = 0.0f;
	mViewPort.Width = static_cast<float>(mScreenWidth);
	mViewPort.Height = static_cast<float>(mScreenHeight);
	mViewPort.MinDepth = 0.0f;
	mViewPort.MaxDepth = 1.0f;

	//RTV(s) and above ViewPort Set Here.
	Begin();
}

void BaseApp::Exit()
{
	PostQuitMessage(NULL);
}

void BaseApp::Shutdown()
{
	ReleaseObject(mRenderTargetView);
	ReleaseObject(mDepthStencilView);
	ReleaseObject(mSwapChain);
	ReleaseObject(mDepthStencilBuffer);

	if (mDirect3DDeviceContext != nullptr)
	{
		mDirect3DDeviceContext->ClearState();
	}

#if 1
	ID3D11Debug* debugDev;
	mDirect3DDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDev));
	debugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	ReleaseObject(debugDev);
#endif

	ReleaseObject(mDirect3DDeviceContext);
	ReleaseObject(mDirect3DDevice);

	UnregisterClass(mWindowClassName.c_str(), mWindowClass.hInstance);
}

void BaseApp::Begin()
{
	//Takes in an array of renderTargets, and one Depth Stencil view, to be used in a stack like fashion.
	RenderTargetStack::Begin(mDirect3DDeviceContext, 1, &mRenderTargetView, mDepthStencilView, mViewPort);
}

void BaseApp::End()
{
	RenderTargetStack::End(mDirect3DDeviceContext);
}

POINT BaseApp::CenterOfWindow(int windowWidth, int windowHeight)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	POINT centerOfWindow;
	centerOfWindow.x = (screenWidth - windowWidth) / 2;
	centerOfWindow.y = (screenHeight - windowHeight) / 2;

	return centerOfWindow;
}

LRESULT WINAPI BaseApp::WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(NULL);
		return EXIT_SUCCESS;
	}
	}

	return DefWindowProc(windowHandle, message, wParam, lParam);
}

void BaseApp::CalcFPS()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	if ((mTime.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frameCount);
		wostringstream OSS;
		OSS << mWindowTitle << L"   " << L"FPS: " << fps;
		SetWindowText(mWindowHandle, OSS.str().c_str());

		//Reset
		frameCount = 0;
		timeElapsed += 1.0f;
	}
}