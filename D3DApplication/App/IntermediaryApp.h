#pragma once
#include "../Shared/SharedUtils.h"
#include "BaseApp.h"

//This Class Trims an additional layer of fat from the CoreApp, since BaseApp was getting bloated.
//This Class manages the CoreApp and provides basic services to it such as a Camera and Input.

//Forward Declarations
class Mouse;
class Keyboard;
class Camera;
class CoreApp;

class IntermediaryApp : public BaseApp
{
public:
	IntermediaryApp(HINSTANCE instance, const wstring& windowClassName, const wstring& windowTitle, int showCMD);
	~IntermediaryApp();

	virtual void Initialize() override;
	virtual void Update(const Time& deltaTime) override;
	virtual void Draw(const Time& deltaTime) override;

protected:
	virtual void Shutdown() override;

private:
	//Managed App Class
	unique_ptr<CoreApp> mCoreApp;

	LPDIRECTINPUT8 mDirectInput;
	unique_ptr<Mouse> mMouse;
	unique_ptr<Keyboard> mKeyboard;
	unique_ptr<Camera> mCamera;

	static const XMVECTORF32 BackgroundColor; //XMVECTORF32 - allows initialization syntax
};