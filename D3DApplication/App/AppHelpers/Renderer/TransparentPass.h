#pragma once
#include "RendererHelpers/CBs/ConstantBuffer.h"
#include "RendererHelpers/AnimatedMesh.h"

//Forward Declarations
class RasterizerStateManager;
class PipelineState;
class Camera;

class TransparentPass
{
public:
	TransparentPass(ID3D11Device* pDevice, PipelineState* pipelineState, Camera& camera, RasterizerStateManager* rasterManager = nullptr);
	~TransparentPass();

	void SetEnabled(bool enabled);
	bool IsEnabled() const;
	void PreRender();
	void Render(float deltaTime);
	void PostRender();
	void Destroy();
	
	//Utility
	void UpdateSortDistances();
	void Sort();
	void SetGeometry(vector<AnimatedMesh*> TransparentGeometry);

	//Shared Animation Time
	static float transparentMeshTotalTime;

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	Camera* mCamera;

	PipelineState* mPipeline;
	RasterizerStateManager* mRasterizerStateManager;
	vector<AnimatedMesh*> mTransparentGeometry;
	vector<float> mTransparentGeometryDistances;
	static unsigned int gCurrentObject;

	bool mEnabled;
};