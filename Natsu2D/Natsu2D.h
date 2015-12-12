#pragma once

//	由于Natsu2D已经不支持生成静态库，这个宏定义即将不再支持
#ifdef Natsu2DStatic
#	ifdef N2DEXPORT
#		define N2DFUNC
#	else
#		define N2DFUNC extern
#	endif // N2DEXPORT
#	define N2DOBJECT extern
#else
#	ifdef N2DEXPORT
#		define N2DFUNC __declspec(dllexport)
#	else
#		define N2DFUNC __declspec(dllimport)
#	endif // N2DEXPORT
#	define N2DOBJECT extern N2DFUNC
#endif // Natsu2DStatic

#include "n2dEngine.h"

extern "C" nResult N2DFUNC CreateN2DEngine(
	nInt Reserved,
	ncTStr lpClassname,
	nuInt x,
	nuInt y,
	nuInt WindowWidth,
	nuInt WindowHeight,
	nuInt ScreenWidth,
	nuInt ScreenHeight,
	nuInt BitsPerPixel,
	nBool fullscreen,
	HINSTANCE hInstance,
	n2dEngine::ThreadMode threadMode,
	n2dEngineEventListener* pListener,
	n2dEngine** pOut);