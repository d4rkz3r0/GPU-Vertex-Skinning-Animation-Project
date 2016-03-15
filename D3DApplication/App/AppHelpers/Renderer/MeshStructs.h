#pragma once
#include "VertexStructs.h"
#include <vector>
using namespace std;

struct MeshData
{
	MeshData() {}

	MeshData(vector<NormalVertex>& verts, vector<UINT> indices, UINT faceCount) :
		mVertices(verts),
		mIndices(indices),
		mFaceCount(faceCount) {}

	vector<NormalVertex> mVertices;
	vector<UINT> mIndices;
	UINT mFaceCount;
	UINT vbCount;
	UINT vbSize;
	UINT ibCount;
	UINT ibSize;
};

struct AnimationMeshData
{
	AnimationMeshData() {}

	AnimationMeshData(vector<AnimationVertex>& verts, vector<UINT> indices, UINT faceCount) :
		mVertices(verts),
		mIndices(indices),
		mFaceCount(faceCount) {}

	vector<AnimationVertex> mVertices;
	vector<UINT> mIndices;
	UINT mFaceCount;
	UINT vbCount;
	UINT vbSize;
	UINT ibCount;
	UINT ibSize;
};