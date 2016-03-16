#include "IntermediaryApp.h"
#include "AppHelpers/Misc/Exception.h"
#include "AppHelpers/Input/Keyboard.h"
#include "AppHelpers/Input/Mouse.h"
#include "AppHelpers/Cameras/FPSCamera.h"
#include "CoreApp.h"

const XMVECTORF32 IntermediaryApp::BackgroundColor = { 0.6f, 0.1f, 0.1f, 1.0f };

IntermediaryApp::IntermediaryApp(HINSTANCE instance, const wstring& windowClassName, const wstring& windowTitle, int showCommand) :
				   BaseApp(instance, windowClassName, windowTitle, showCommand), mCoreApp(nullptr), mDirectInput(nullptr), mMouse(nullptr),
					mKeyboard(nullptr), mCamera(nullptr)
{
	mIsDepthStencilBufferEnabled = true;
	mMSAAEnabled = true;
}

IntermediaryApp::~IntermediaryApp()
{
	
}

void IntermediaryApp::Initialize()
{
	if (FAILED(DirectInput8Create(mhInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<LPVOID*>(&mDirectInput), nullptr)))
	{
		throw Exception("IntermediaryApp::Initialze()::DirectInput8Create() failed.");
	}

	mKeyboard = make_unique<Keyboard>(*this, mDirectInput);
	mComponents.push_back(mKeyboard.get());
	mServices.AddService(Keyboard::TypeIdClass(), mKeyboard.get());

	mMouse = make_unique<Mouse>(*this, mDirectInput);
	mComponents.push_back(mMouse.get());
	mServices.AddService(Mouse::TypeIdClass(), mMouse.get());

	mCamera = make_unique<FPSCamera>(*this);
	mComponents.push_back(mCamera.get());
	mServices.AddService(BaseCamera::TypeIdClass(), mCamera.get());

	mCoreApp = make_unique<CoreApp>(*this, *mCamera);
	mComponents.push_back(mCoreApp.get());

	BaseApp::Initialize();

	mCamera->SetPosition(-2.0f, 5.0f, 20.0f);
}

void IntermediaryApp::Update(const Time& deltaTime)
{
	if (mKeyboard->WasKeyPressedThisFrame(DIK_ESCAPE))
	{
		Exit();
	}

	BaseApp::Update(deltaTime);
}

void IntermediaryApp::Draw(const Time& deltaTime)
{
	BaseApp::Draw(deltaTime);

	HRESULT hr = mSwapChain->Present(0, 0);
	if (FAILED(hr))
	{
		throw Exception("IDXGISwapChain::Present() failed.", hr);
	}
}

void IntermediaryApp::Shutdown()
{
	ReleaseObject(mDirectInput);
	BaseApp::Shutdown();
}