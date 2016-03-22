#pragma once
#include "../Renderer/PipelineState.h"
#include "RendererHelpers/AnimatedMesh.h"


class OpaquePass
{
public:
	OpaquePass(ID3D11Device* pDevice, PipelineState* pipelineState);
	~OpaquePass();

	void SetEnabled(bool enabled);
	bool IsEnabled() const;
	void PreRender();
	void Render(float deltaTime);
	void PostRender();
	void Destroy();

	void SetGeometry(vector<AnimatedMesh*> OpaqueGeometry);

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;

	PipelineState* mPipeline;
	vector<AnimatedMesh*> mOpaqueGeometry;

	//Shared Animation Time
	static float opaqueMeshTotalTime;
	bool mEnabled;
};