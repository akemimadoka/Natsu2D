////////////////////////////////////////////////////////////////////////////////
///	@file	n2dCommon.h
///	@brief	通用头文件
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <tchar.h>

//#define GLEW_MX
#define GLEW_STATIC 1

#include "extern\glew\include\GL\glew.h"
/*#if defined(_WIN32)
#	include "include\GL\wglew.h"
#elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX)
#	include <GL/glxew.h>
#endif*/

#include <natException.h>
//#include "n2dUtil.h"
#include <natUtil.h>

#include <natType.h>