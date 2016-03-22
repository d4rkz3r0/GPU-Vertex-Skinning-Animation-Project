#pragma once

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <windows.h>
#include <string>
#include <cassert>
#include <vector>
#include <map>
#include <memory>
#include <stack>
#include <WICTextureLoader.h>
#include <algorithm>
#include <sys/stat.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include "RTTI.h"

#include <dinput.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace std;
using namespace DirectX;
using namespace PackedVector;

//Shared  Macros
#define DeleteObject(object)   if((object)  != nullptr)  { delete object; object = nullptr; }
#define DeleteObjects(objects) if((objects) != nullptr)  { delete[] objects; objects = nullptr; }
#define ReleaseObject(object)  if((object)  != nullptr)  { object->Release(); object = nullptr; }
#define ZeroMem(object)  ZeroMemory(&object, sizeof(object));

//Shared TypeDefs
typedef unsigned char byte;