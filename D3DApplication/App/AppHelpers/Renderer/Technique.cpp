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

void Technique::Render()
{
	//Might have to group All Pre's, Render's and Post's together

	for (auto oPass : mOpaquePasses)
	{
		if (oPass->IsEnabled())
		{
			oPass->PreRender();
			//oPass->Render();
			oPass->PostRender();
		}
	}

	for (auto tPass : mTransPasses)
	{
		if (tPass->IsEnabled())
		{
			tPass->PreRender();
			//tPass->Render();
			tPass->PostRender();
		}
	}

	for (auto cPass : mClearPasses)
	{
		if (cPass->IsEnabled())
		{
			cPass->Render();
		}
	}
}