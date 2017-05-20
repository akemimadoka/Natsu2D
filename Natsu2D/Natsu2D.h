#pragma once
#include "n2dEngine.h"

extern "C" nResult N2DFUNC CreateN2DEngine(
	nInt Reserved,
	nStrView lpClassname,
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
	natRefPointer<n2dEngine>& pOut);