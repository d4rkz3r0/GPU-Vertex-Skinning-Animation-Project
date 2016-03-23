#pragma once
#include "../../../Shared/SharedUtils.h"
#include "../../../App/AppHelpers/Misc/Time.h"

//Forward Declarations
class Camera;

class BaseMesh
{

public:
	BaseMesh(Camera& camera);
	virtual ~BaseMesh();

	//accessors
	Camera& GetCamera();
	XMMATRIX GetWorld();
	bool IsEnabled() const;

	//Mutators
	void SetEnabled(bool enabled);
	void SetWorld(CXMMATRIX matrixVal);
	void SetScaling(CXMMATRIX matrixVal);
	void SetRotation(CXMMATRIX matrixVal);
	void SetTranslation(CXMMATRIX matrixVal);

	//Pure Virtuals
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual void Load(const string& file) = 0;
	virtual void Update(Time& deltaTime) = 0;
	virtual void Draw(float deltaTime) = 0;
	virtual void UpdateConstantBuffers(float deltaTime) = 0;

protected:
	string mFilePath;
	string mFileName;
	string mFileType;
	string mTextureFilePath;
	string mDiffuseMapFileName;
	string mNormalMapFileName;
	XMFLOAT4X4 mScalingMX;
	XMFLOAT4X4 mRotationMX;
	XMFLOAT4X4 mTranslationMX;
	XMMATRIX mWorldMX;
	Camera* mCamera;
	bool mEnabled;

private:
	BaseMesh(const BaseMesh& rhs) = default;
	BaseMesh& operator=(const BaseMesh& rhs) = default;
};