#pragma once
#include "OpaquePass.h"
#include "TransparentPass.h"
#include "ClearPass.h"
#include <vector>

class Technique
{
public:
	Technique();
	virtual ~Technique();
	void Destroy();
	void AddOpaquePass(OpaquePass* oPass);
	void AddTransparentPass(TransparentPass* tPass);
	void AddClearPass(ClearPass* cPass);
	OpaquePass* GetOpaquePass(UINT ID) const;
	TransparentPass* GetTransPass(UINT ID) const;
	ClearPass* GetClearPass(UINT ID) const;

	void Render(float deltaTime);

private:
	vector<OpaquePass*> mOpaquePasses;
	vector<TransparentPass*> mTransPasses;
	vector<ClearPass*> mClearPasses;
};