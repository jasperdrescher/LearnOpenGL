#pragma once

#include "glad/glad.h"

namespace GLUtility
{
	void CheckForGLErrors();
	void APIENTRY GLDebugMessageCallback(GLenum aSource, GLenum aType, unsigned int anIdentifier, GLenum aSeverity, int aLength, const char* aMessage, const void* anUserParam);
	void GLFWErrorCallback(int anError, const char* aDescription);
}
