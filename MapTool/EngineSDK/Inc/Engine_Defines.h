#pragma once

#define VK_MAX	0xff
#define OFFSET	0x0000

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <time.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Effects11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"

#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\Effects.h"

#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"

#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <fstream>

using namespace DirectX;

/* d3dx11에 포함되어있었지만 너무 무겁다. */
/* 기존 d3dx11포함되어있었던 다양한 기능을 제공하는 헤더 + 라이브러리 파일을 선택적으로 추가한다. */
//#include <d3dx11.h>

#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>


using namespace std;

#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"

namespace Engine
{
	const _wstring		g_strTransformTag = TEXT("Com_Transform");
	const _uint			g_iMaxMeshBones = 512;

	enum MOUSEKEYSTATE { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };

    enum aiTextureType {
        aiTextureType_NONE = 0,
        aiTextureType_DIFFUSE = 1,
        aiTextureType_SPECULAR = 2,
        aiTextureType_AMBIENT = 3,
        aiTextureType_EMISSIVE = 4,
        aiTextureType_HEIGHT = 5,
        aiTextureType_NORMALS = 6,
        aiTextureType_SHININESS = 7,
        aiTextureType_OPACITY = 8,
        aiTextureType_DISPLACEMENT = 9,
        aiTextureType_LIGHTMAP = 10,
        aiTextureType_REFLECTION = 11,
        aiTextureType_BASE_COLOR = 12,
        aiTextureType_NORMAL_CAMERA = 13,
        aiTextureType_EMISSION_COLOR = 14,
        aiTextureType_METALNESS = 15,
        aiTextureType_DIFFUSE_ROUGHNESS = 16,
        aiTextureType_AMBIENT_OCCLUSION = 17,
        aiTextureType_UNKNOWN = 18,

#ifndef SWIG
        _aiTextureType_Force32Bit = INT_MAX
#endif
    };
}
#define AI_TEXTURE_TYPE_MAX aiTextureType_UNKNOWN

#include "Engine_Struct.h"

using namespace Engine;



#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG


