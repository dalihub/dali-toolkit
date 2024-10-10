/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 */

// Header
#include <dali-physics/internal/physics-debug-renderer-gles.h>

// External Includes
#include <GLES3/gl3.h>
#include <cstdio>
#include <vector>

namespace Dali::Toolkit::Physics::Internal::Gles
{
namespace
{
GLuint LoadShader(GLenum shaderType, const char* shaderSource)
{
  GLuint shader = glCreateShader(shaderType);
  if(shader != 0)
  {
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(compiled != GL_TRUE)
    {
      GLint infoLen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

      if(infoLen > 0)
      {
        std::vector<char> logBuffer;
        logBuffer.resize(infoLen + 1);
        glGetShaderInfoLog(shader, infoLen, NULL, &logBuffer[0]);
        fprintf(stderr, "%s\n", &logBuffer[0]);
        fflush(stderr);

        glDeleteShader(shader);
        shader = 0;
      }
    }
  }
  return shader;
}
} // unnamed namespace

GLuint CreateProgram(const char* vertexSource, const char* fragmentSource)
{
  GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, vertexSource);
  if(!vertexShader)
  {
    return 0;
  }
  GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentSource);
  if(!fragmentShader)
  {
    return 0;
  }
  GLuint program = glCreateProgram();
  if(program)
  {
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if(linkStatus != GL_TRUE)
    {
      GLint bufLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
      if(bufLength)
      {
        std::vector<char> logBuffer;
        logBuffer.resize(bufLength + 1);
        glGetProgramInfoLog(program, bufLength, NULL, &logBuffer[0]);
        fprintf(stderr, "%s\n", &logBuffer[0]);
        fflush(stderr);
      }
      glDeleteProgram(program);
      program = 0;
    }
  }
  return program;
}

} // namespace Dali::Toolkit::Physics::Internal::Gles
