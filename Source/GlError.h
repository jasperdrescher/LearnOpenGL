#pragma once

#include <glad/glad.h>

namespace GlError
{
	static inline void GLAPIENTRY GlErrorCallback(const GLenum aSource, const GLenum aType, const GLuint aID, const GLenum aSeverity, const GLsizei /*aLength*/, const GLchar* aMessage, const void* /*aUserParam*/)
	{
		const char* source;
		const char* type;
		const char* severity;

		switch (aSource)
		{
			case GL_DEBUG_SOURCE_API:
			{
				source = "API";
				break;
			}
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			{
				source = "WINDOW SYSTEM";
				break;
			}
			case GL_DEBUG_SOURCE_SHADER_COMPILER:
			{
				source = "SHADER COMPILER";
				break;
			}
			case GL_DEBUG_SOURCE_THIRD_PARTY:
			{
				source = "THIRD PARTY";
				break;
			}
			case GL_DEBUG_SOURCE_APPLICATION:
			{
				source = "APPLICATION";
				break;
			}
			case GL_DEBUG_SOURCE_OTHER:
			{
				source = "OTHER";
				break;
			}
			default:
			{
				source = "UNKNOWN";
				break;
			}
		}

		switch (aType)
		{
			case GL_DEBUG_TYPE_ERROR:
			{
				type = "ERROR";
				break;
			}
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			{
				type = "DEPRECATED BEHAVIOR";
				break;
			}
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			{
				type = "UDEFINED BEHAVIOR";
				break;
			}
			case GL_DEBUG_TYPE_PORTABILITY:
			{
				type = "PORTABILITY";
				break;
			}
			case GL_DEBUG_TYPE_PERFORMANCE:
			{
				type = "PERFORMANCE";
				break;
			}
			case GL_DEBUG_TYPE_OTHER:
			{
				type = "OTHER";
				break;
			}
			case GL_DEBUG_TYPE_MARKER:
			{
				type = "MARKER";
				break;
			}
			default:
			{
				type = "UNKNOWN";
				break;
			}
		}

		switch (aSeverity)
		{
			case GL_DEBUG_SEVERITY_HIGH:
			{
				severity = "HIGH";
				break;
			}
			case GL_DEBUG_SEVERITY_MEDIUM:
			{
				severity = "MEDIUM";
				break;
			}
			case GL_DEBUG_SEVERITY_LOW:
			{
				severity = "LOW";
				break;
			}
			case GL_DEBUG_SEVERITY_NOTIFICATION:
			{
				severity = "NOTIFICATION";
				break;
			}
			default:
			{
				severity = "UNKNOWN";
				break;
			}
		}

		printf("Severity: %s Type: %s Source: %s Id: 0x%x", severity, type, source, aID);
		printf(aMessage);
	}

	static inline void CheckGlError()
	{
		while (true)
		{
			const GLenum error = glGetError();
			if (error == GL_NO_ERROR)
				return;

			const char* errorMessage;
			switch (error)
			{
				case GL_NO_ERROR:
				{
					errorMessage = "No error";
					break;
				}
				case GL_INVALID_ENUM:
				{
					errorMessage = "Invalid enum";
					break;
				}
				case GL_INVALID_VALUE:
				{
					errorMessage = "Invalid value";
					break;
				}
				case GL_INVALID_OPERATION:
				{
					errorMessage = "Invalid operation";
					break;
				}
				case GL_STACK_OVERFLOW:
				{
					errorMessage = "Stack overflow";
					break;
				}
				case GL_STACK_UNDERFLOW:
				{
					errorMessage = "Stack underflow";
					break;
				}
				case GL_OUT_OF_MEMORY:
				{
					errorMessage = "Out of memory";
					break;
				}
				default:
				{
					errorMessage = "Unknown error";
					break;
				}
			}

			printf(errorMessage);
		}
	}
}
