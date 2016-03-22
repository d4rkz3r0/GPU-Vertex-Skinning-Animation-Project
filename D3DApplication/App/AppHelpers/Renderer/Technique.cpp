#include "Technique.h"

Technique::Technique()
{

}

void Technique::Destroy()
{
	mOpaquePasses.clear();
	mTransPasses.clear();
	mClearPasses.clear();
}

Technique::~Technique()
{
	Destroy();
}

void Technique::AddOpaquePass(OpaquePass* oPass)
{
	mOpaquePasses.push_back(oPass);
}

void Technique::AddTransparentPass(TransparentPass* tPass)
{
	mTransPasses.push_back(tPass);
}

void Technique::AddClearPass(ClearPass* cPass)
{
	mClearPasses.push_back(cPass);
}

OpaquePass* Technique::GetOpaquePass(UINT ID) const
{
	return mOpaquePasses.at(ID);
}

TransparentPass* Technique::GetTransPass(UINT ID) const
{
	return mTransPasses.at(ID);
}

ClearPass* Technique::GetClearPass(UINT ID) const
{
	return mClearPasses.at(ID);
}

void Technique::Render(float deltaTime)
{
	for (auto cPass : mClearPasses)
	{
		if (cPass->IsEnabled())
		{
			cPass->Render();
		}
	}

	for (auto oPass : mOpaquePasses)
	{
		if (oPass->IsEnabled())
		{
			oPass->PreRender();
			oPass->Render(deltaTime);
			//oPass->PostRender();
		}
	}

	for (auto tPass : mTransPasses)
	{
		if (tPass->IsEnabled())
		{
			tPass->PreRender();
			tPass->Render(deltaTime);
			//tPass->PostRender();
		}
	}
}