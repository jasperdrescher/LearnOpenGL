#include "GLUtility.h"

#include <cassert>
#include <string>

#include "LogUtility.h"

namespace GLUtility
{
    void CheckForGLErrors()
    {
        const GLenum error = glGetError();
        if (error == GL_NO_ERROR)
            return;

        const char* errorString;
        switch (error)
        {
            case GL_INVALID_ENUM:
            {
                errorString = "GL_INVALID_ENUM";
                break;
            }
            case GL_INVALID_VALUE:
            {
                errorString = "GL_INVALID_VALUE";
                break;
            }
            case GL_INVALID_OPERATION:
            {
                errorString = "GL_INVALID_OPERATION";
                break;
            }
            case GL_STACK_OVERFLOW:
            {
                errorString = "GL_STACK_OVERFLOW";
                break;
            }
            case GL_STACK_UNDERFLOW:
            {
                errorString = "GL_STACK_UNDERFLOW";
                break;
            }
            case GL_OUT_OF_MEMORY:
            {
                errorString = "GL_OUT_OF_MEMORY";
                break;
            }
            case GL_INVALID_FRAMEBUFFER_OPERATION:
            {
                errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            }
            default:
            {
                errorString = "Unknown error";
                break;
            }
        }

        LogUtility::PrintError(errorString);
        assert(error == GL_NO_ERROR);
    }

    void APIENTRY GLDebugMessageCallback(GLenum aSource, GLenum aType, unsigned int anIdentifier, GLenum aSeverity, int /*aLength*/, const char* aMessage, const void* /*anUserParam*/)
    {
        // Ignore insignificant error codes
        // #131169: Framebuffer detailed info: The driver allocated storage for renderbuffer 2. (severity: low)
        // #131185: Buffer detailed info: Buffer object 1 (bound to GL_ELEMENT_ARRAY_BUFFER_ARB, usage hint is GL_ENUM_88e4) will use VIDEO memory as the source for buffer object operations. (severity: low)
        // #131218: Program/shader state performance warning: Vertex shader in program 7 is being recompiled based on GL state. (severity: medium)
        // #131204: Texture state usage warning: The texture object (0) bound to texture image unit 0 does not have a defined base level and cannot be used for texture mapping. (severity: low)
        if (anIdentifier == 131169 || anIdentifier == 131185 || anIdentifier == 131218 || anIdentifier == 131204)
            return;

        std::string errorMessage;
        switch (aSource)
        {
            case GL_DEBUG_SOURCE_API:
            {
                errorMessage += "Source: API\n";
                break;
            }
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            {
                errorMessage += "Source: Window System\n";
                break;
            }
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
            {
                errorMessage += "Source: Shader Compiler\n";
                break;
            }
            case GL_DEBUG_SOURCE_THIRD_PARTY:
            {
                errorMessage += "Source: Third Party\n";
                break;
            }
            case GL_DEBUG_SOURCE_APPLICATION:
            {
                errorMessage += "Source: Application\n";
                break;
            }
            case GL_DEBUG_SOURCE_OTHER:
            {
                errorMessage += "Source: Other\n";
                break;
            }
            default:
            {
                errorMessage += "Source: Unkown\n";
                break;
            }
        }

        switch (aType)
        {
            case GL_DEBUG_TYPE_ERROR:
            {
                errorMessage += "Type: Error\n";
                break;
            }
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            {
                errorMessage += "Type: Deprecated Behaviour\n";
                break;
            }
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            {
                errorMessage += "Type: Undefined Behaviour\n";
                break;
            }
            case GL_DEBUG_TYPE_PORTABILITY:
            {
                errorMessage += "Type: Portability\n";
                break;
            }
            case GL_DEBUG_TYPE_PERFORMANCE:
            {
                errorMessage += "Type: Performance\n";
                break;
            }
            case GL_DEBUG_TYPE_MARKER:
            {
                errorMessage += "Type: Marker\n";
                break;
            }
            case GL_DEBUG_TYPE_PUSH_GROUP:
            {
                errorMessage += "Type: Push Group\n";
                break;
            }
            case GL_DEBUG_TYPE_POP_GROUP:
            {
                errorMessage += "Type: Pop Group\n";
                break;
            }
            case GL_DEBUG_TYPE_OTHER:
            {
                errorMessage += "Type: Other\n";
                break;
            }
            default:
            {
                errorMessage += "Type: Unkown\n";
                break;
            }
        }

        switch (aSeverity)
        {
            case GL_DEBUG_SEVERITY_HIGH:
            {
                errorMessage += "Severity: High\n";
                break;
            }
            case GL_DEBUG_SEVERITY_MEDIUM:
            {
                errorMessage += "Severity: Medium\n";
                break;
            }
            case GL_DEBUG_SEVERITY_LOW:
            {
                errorMessage += "Severity: Low\n";
                break;
            }
            case GL_DEBUG_SEVERITY_NOTIFICATION:
            {
                errorMessage += "Severity: Notification\n";
                break;
            }
            default:
            {
                errorMessage += "Severity: Unkown\n";
                break;
            }
        }

        errorMessage += aMessage;

        LogUtility::PrintError(errorMessage.c_str());
        assert(errorMessage.empty());
    }

    void GLFWErrorCallback(int anError, const char* aDescription)
    {
        LogUtility::PrintError("%i %s", anError, aDescription);
        assert(!aDescription);
    }
}