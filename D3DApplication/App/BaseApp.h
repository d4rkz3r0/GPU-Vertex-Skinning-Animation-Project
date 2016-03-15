#pragma once
#include "../Shared/SharedUtils.h"
#include "AppHelpers/Misc/Clock.h"
#include "AppHelpers/Misc/Time.h"
#include "AppHelpers/Misc/ServiceContainer.h"
#include "AppHelpers/Components/BaseComponent.h"
#include "AppHelpers/Renderer/RendererHelpers/RenderTargetStack.h"

class BaseApp : public RenderTargetStack
{
public:
	BaseApp(HINSTANCE hInstance, const wstring& WindowClassName, const wstring& windowTitle, int showCMD);
	virtual ~BaseApp() = default;

	//Win32
	HINSTANCE GetInstance() const;
	HWND GetWindowHandle() const;
	const WNDCLASSEX& GetWindowClass() const;
	const wstring& GetWindowClassName() const;
	const wstring& GetWindowTitle() const;
	int GetScreenWidth() const;
	int GetScreenHeight() const;

	//D3D 11.1
	ID3D11Device1* D3DDevice() const;
	ID3D11DeviceContext1* D3DDevCon() const;
	ID3D11RenderTargetView* BackBufferRTV() const;
	ID3D11DepthStencilView* BackBufferDSV() const;
	const D3D11_VIEWPORT& GetViewPort() const;
	const D3D11_TEXTURE2D_DESC& GetBackBufferDesc() const;

	//BaseApp
	bool IsFullScreen() const;
	bool IsDepthStencilBufferEnabled() const;
	float GetAspectRatio() const;

	virtual void Run();
	virtual void Initialize();
	virtual void Update(const Time& deltaTime);
	virtual void Draw(const Time& deltaTime);
	virtual void Exit();

	const vector<BaseComponent*>& GetComponents() const;
	const ServiceContainer& GetServices() const;

protected:
	//App Components
	vector<BaseComponent*> mComponents;

	//App Services
	ServiceContainer mServices;

	//Misc
	Clock mClock;
	Time mTime;

	virtual void InitWin32();
	virtual void InitD3D11();
	virtual void Shutdown();
	//RenderTargetStack Overrides
	virtual void Begin() override;
	virtual void End() override;

	//Defaults
	static const UINT DefaultScreenWidth;
	static const UINT DefaultScreenHeight;
	static const UINT DefaultFrameRate;
	static const UINT DefaultMSAASampleCount;

	//Win32
	HINSTANCE mhInstance;
	wstring mWindowClassName;
	wstring mWindowTitle;
	int mShowCMD;

	HWND mWindowHandle;
	WNDCLASSEX mWindowClass;

	UINT mScreenWidth;
	UINT mScreenHeight;

	//D3D
	ID3D11Device1* mDirect3DDevice;
	ID3D11DeviceContext1* mDirect3DDeviceContext;

public:
	IDXGISwapChain1* mSwapChain;
	
	D3D11_TEXTURE2D_DESC mBackBufferDesc;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11Texture2D* mDepthStencilBuffer;
	D3D11_VIEWPORT mViewPort;
	D3D_FEATURE_LEVEL mFeatureLevel;

protected:

	//BaseApp
	UINT mFrameRate;
	UINT mMSAASampleCount;
	UINT mMSAAQualityLevels;
	bool mIsFullScreen;
	bool mIsDepthStencilBufferEnabled;
	bool mMSAAEnabled;

private:
	//No Copy
	BaseApp(const BaseApp& rhs) = delete;
	BaseApp& operator=(const BaseApp& rhs) = delete;

	void CalcFPS();
	POINT CenterOfWindow(int windowWidth, int windowHeight);
	static LRESULT WINAPI WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
};