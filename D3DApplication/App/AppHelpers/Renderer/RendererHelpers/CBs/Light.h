#pragma once
#include <cstdint>
#include <DirectXMath.h>
using namespace DirectX;

enum class LightType : uint32_t
{
	Point = 0,
	Spot = 1,
	Directional = 2
};

struct Light
{
	Light::Light() :
		mPositionWS(0, 0, 0, 1),
		mDirectionWS(0, 0, -1, 0),
		mPositionVS(0, 0, 0, 1),
		mDirectionVS(0, 0, 1, 0),
		mColor(1, 1, 1, 1),
		mSpotlightAngle(45.0f),
		mRange(100.0f),
		mIntensity(1.0f),
		mEnabled(true),
		mSelected(false),
		mType(LightType::Point) { }

	XMFLOAT4   mPositionWS;
	XMFLOAT4   mDirectionWS;
	XMFLOAT4   mPositionVS;
	XMFLOAT4   mDirectionVS;
	XMFLOAT4   mColor;
	float       mSpotlightAngle;
	float       mRange;
	float       mIntensity;
	uint32_t    mEnabled;
	uint32_t    mSelected;
	LightType   mType;
	XMFLOAT2   mPadding;
};