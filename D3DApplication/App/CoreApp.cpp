#include "CoreApp.h"
#include "BaseApp.h"
#include "../Shared/SharedUtils.h"
#include "AppHelpers/Input/Mouse.h"
#include "AppHelpers/Input/Keyboard.h"
#include "AppHelpers/Cameras/Camera.h"
#include "AppHelpers/Renderer/Renderer.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>


RTTI_DEFINITIONS(CoreApp)

CoreApp::CoreApp(BaseApp& baseApp, Camera& camera) : RenderableComponent(baseApp, camera), mMouse(nullptr), mKeyboard(nullptr), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(XMFLOAT2(5.0f, 5.0f))
{ }

CoreApp::~CoreApp()
{

}

void CoreApp::Initialize()
{
	//Retrieve Input Services
	mKeyboard = static_cast<Keyboard*>(mBaseApp->GetServices().GetService(Keyboard::TypeIdClass()));
	assert(mKeyboard != nullptr);
	mMouse = static_cast<Mouse*>(mBaseApp->GetServices().GetService(Mouse::TypeIdClass()));
	assert(mMouse != nullptr);

	//Load Font
	mSpriteBatch = make_unique<SpriteBatch>(mBaseApp->D3DDevCon());
	mSpriteFont = make_unique<SpriteFont>(mBaseApp->D3DDevice(), L"Assets\\Fonts\\appFont.spritefont");

	//Start Renderer
	mRenderer = new Renderer(mBaseApp->D3DDevice(), *mCamera, mBaseApp->BackBufferRTV(), mBaseApp->BackBufferDSV());
}

void CoreApp::Update(const Time& deltaTime)
{
	UpdateUserInput();
}

void CoreApp::Draw(const Time& deltaTime)
{
	mRenderer->Draw(static_cast<float>(deltaTime.ElapsedTime()));
	RenderUI();
}

void CoreApp::UpdateUserInput()
{

}

void CoreApp::RenderUI()
{
	mSpriteBatch->Begin();
	wostringstream textLabel;
	textLabel << "W/A/S/D->Move\nQ/E->Fly\nEsc->Quit";
	mSpriteFont->DrawString(mSpriteBatch.get(), textLabel.str().c_str(), mTextPosition);
	mSpriteBatch->End();
}