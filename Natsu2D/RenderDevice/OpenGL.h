#pragma once

#ifndef GLEW_STATIC
#	define GLEW_STATIC 1
#endif

#include "glew.h"

#if defined(_WIN32)
#	include "wglew.h"
#elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX)
#	include "glxew.h"
#endif