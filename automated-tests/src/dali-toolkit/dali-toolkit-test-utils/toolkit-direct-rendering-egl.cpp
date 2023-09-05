/*
* Copyright (c) 2023 Samsung Electronics Co., Ltd.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include <EGL/egl.h>
#include <GLES3/gl3.h>
extern "C"
{
  // Flag to be set when we want shader compilation fail
  bool gDirectRenderingFailCreateShader = false;

  // Flag to be set when we want program linking fail
  bool gDirectRenderingFailCreateProgram = false;

  /**
 * To test the multithreaded variant we need override EGL api
 *
 * The Direct Rendering uses GL directly and it's needed to override certain funtions in order
 * to force code execution.
 */
  EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint* attrib_list)
  {
    return EGLContext(0x12345678);
  }

  EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config)
  {
    static EGLConfig config;
    if(num_config)
    {
      *num_config = 1;
    }
    if(configs)
    {
      configs[0] = config;
    }

    return EGL_TRUE;
  }

  GLuint glCreateProgram(void)
  {
    static uint32_t programId = 1;
    return programId++;
  }

  GLuint glCreateShader(GLenum type)
  {
    static uint32_t shaderId = 1;
    return shaderId++;
  }

  void glCompileShader(GLuint shader)
  {
  }

  void glLinkProgram(GLuint program)
  {
  }

  void glGenTextures(GLsizei n, GLuint* textures)
  {
    static GLuint texId = 1u;
    for(auto i = 0; i < n; ++i)
    {
      textures[i] = texId++;
    }
  }

  void glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
  {
    if(pname == GL_COMPILE_STATUS)
    {
      params[0] = gDirectRenderingFailCreateShader ? GL_FALSE : GL_TRUE;
    }
    else if(pname == GL_INFO_LOG_LENGTH)
    {
      params[0] = 4;
    }
  }

  void glGetProgramiv(GLuint shader, GLenum pname, GLint* params)
  {
    if(pname == GL_LINK_STATUS)
    {
      params[0] = gDirectRenderingFailCreateProgram ? GL_FALSE : GL_TRUE;
    }
    else if(pname == GL_INFO_LOG_LENGTH)
    {
      params[0] = 4;
    }
  }

  void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
  {
    infoLog[0] = '0';
    infoLog[1] = '\n';
  }

  void glGetProgramInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
  {
    infoLog[0] = '0';
    infoLog[1] = '\n';
  }

  void glDeleteSync(GLsync sync)
  {
  }

  GLenum glClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
  {
    return GL_CONDITION_SATISFIED;
  }

  GLsync glFenceSync(GLenum condition, GLbitfield flags)
  {
    static uint32_t syncId = 0;
    return reinterpret_cast<GLsync>(++syncId);
  }

  GLenum glCheckFramebufferStatus(GLenum target)
  {
    return GL_FRAMEBUFFER_COMPLETE;
  }
}