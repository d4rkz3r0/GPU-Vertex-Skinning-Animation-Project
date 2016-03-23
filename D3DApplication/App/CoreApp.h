#pragma once
#include "AppHelpers/Components/RenderableComponent.h"

//Forward Declarations
class Camera;
class Renderer;
class Keyboard;
class Mouse;

namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}

class CoreApp : public RenderableComponent
{
	RTTI_DECLARATIONS(CoreApp, RenderableComponent)

public:
	CoreApp(BaseApp& baseApp, Camera& camera);
	~CoreApp();

	//Core Overrides
	virtual void Initialize() override;
	virtual void Update(Time& deltaTime) override;
	virtual void Draw(const Time& deltaTime) override;

	//Auxiliary
	void RenderUI();

private:
	CoreApp() = delete;
	CoreApp(const CoreApp& rhs) = delete;
	CoreApp& operator=(const CoreApp& rhs) = delete;

	//DirectInput
	Mouse* mMouse;
	Keyboard* mKeyboard;

	//DirectXTK Font Rendering
	unique_ptr<SpriteBatch> mSpriteBatch;
	unique_ptr<SpriteFont> mSpriteFont;
	XMFLOAT2 mTextPosition;

	//Renderer
	Renderer* mRenderer;
};