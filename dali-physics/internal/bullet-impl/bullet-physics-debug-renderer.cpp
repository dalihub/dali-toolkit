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
 */

// Class header
#include <dali-physics/internal/bullet-impl/bullet-physics-debug-renderer.h>

// External Includes
#include <dali/dali.h>

// Internal Includes
#include <dali-physics/internal/physics-adaptor-impl.h>

using Dali::Degree;
using Dali::Matrix;
using Dali::Quaternion;
using Dali::Radian;
using Dali::Vector3;
using Dali::Vector4;

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
} // namespace

namespace Dali::Toolkit::Physics::Internal
{
std::unique_ptr<PhysicsDebugRenderer> PhysicsDebugRenderer::New(uint32_t width, uint32_t height, Dali::CameraActor camera, PhysicsAdaptor* adaptor)
{
  auto renderer             = std::make_unique<PhysicsDebugRenderer>(width, height, camera, adaptor);
  renderer->mRenderCallback = Dali::RenderCallback::New<PhysicsDebugRenderer>(renderer.get(), &PhysicsDebugRenderer::OnRender);
  return renderer;
}

PhysicsDebugRenderer::PhysicsDebugRenderer(uint32_t width, uint32_t height, Dali::CameraActor camera, PhysicsAdaptor* adaptor)
: mCamera(camera),
  mWidth(width),
  mHeight(height),
  mAdaptor(*adaptor)
{
}

bool PhysicsDebugRenderer::OnRender(const Dali::RenderCallbackInput& input)
{
  if(mState == State::INIT)
  {
    Setup();
    mState = State::RENDER;
  }
  glViewport(0, 0, mWidth, mHeight);

  RenderLines(input);

  return false;
}

// Run on first invocation of callback
void PhysicsDebugRenderer::Setup()
{
  PrepareShader();
  mVertexLocation       = glGetAttribLocation(mProgramId, "vertexPosition");
  mVertexColourLocation = glGetAttribLocation(mProgramId, "vertexColour");
  mProjectionLocation   = glGetUniformLocation(mProgramId, "projection");
  mModelViewLocation    = glGetUniformLocation(mProgramId, "modelView");

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, mWidth, mHeight);

  glGenBuffers(1, &mBufferId);
}

void PhysicsDebugRenderer::UpdateWindowSize(Dali::Vector2 size)
{
  mWidth  = size.width;
  mHeight = size.height;
}

void PhysicsDebugRenderer::PrepareShader()
{
  static const char glVertexShader[] =
    "attribute vec4 vertexPosition;\n"
    "attribute vec3 vertexColour;\n"
    "varying vec3 fragColour;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 modelView;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * modelView * vertexPosition;\n"
    "    fragColour = vertexColour;\n"
    "}\n";

  static const char glFragmentShader[] =
    "precision mediump float;\n"
    "varying vec3 fragColour;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(fragColour, 1.0);\n"
    "}\n";

  mProgramId = CreateProgram(glVertexShader, glFragmentShader);
}

void PhysicsDebugRenderer::RenderLines(const Dali::RenderCallbackInput& input)
{
  mModelViewMatrix.SetIdentity();
  mProjectionMatrix = input.projection;

  Matrix::Multiply(mModelViewMatrix, mModelViewMatrix, input.view);
  glUseProgram(mProgramId);

  // In theory, input.clippingBox should tell us the actor position in clip-space.
  // But, it appears to be bugged.

  glBindBuffer(GL_ARRAY_BUFFER, mBufferId);
  glBufferData(GL_ARRAY_BUFFER, mLines.size() * sizeof(VertexLine), &mLines[0], GL_STATIC_DRAW);

  glVertexAttribPointer(mVertexLocation, 3, GL_FLOAT, GL_FALSE, 24, 0);
  glEnableVertexAttribArray(mVertexLocation);
  glVertexAttribPointer(mVertexColourLocation, 3, GL_FLOAT, GL_FALSE, 24, reinterpret_cast<const void*>(12));
  glEnableVertexAttribArray(mVertexColourLocation);
  glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, mProjectionMatrix.AsFloat());
  glUniformMatrix4fv(mModelViewLocation, 1, GL_FALSE, mModelViewMatrix.AsFloat());

  glDrawArrays(GL_LINES, 0, mLines.size());
  mLines.clear();
}

void PhysicsDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
  mLines.push_back(VertexLine{mAdaptor.TranslateFromPhysicsSpace(Vector3(from.x(), from.y(), from.z())),
                              Vector3(color.x(), color.y(), color.z())});
  mLines.push_back(VertexLine{mAdaptor.TranslateFromPhysicsSpace(Vector3(to.x(), to.y(), to.z())), Vector3(color.x(), color.y(), color.z())});
}

void PhysicsDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
  mLines.push_back(VertexLine{mAdaptor.TranslateFromPhysicsSpace(Vector3(from.x(), from.y(), from.z())), Vector3(fromColor.x(), fromColor.y(), fromColor.z())});
  mLines.push_back(VertexLine{mAdaptor.TranslateFromPhysicsSpace(Vector3(to.x(), to.y(), to.z())), Vector3(toColor.x(), toColor.y(), toColor.z())});
}

void PhysicsDebugRenderer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
}

void PhysicsDebugRenderer::reportErrorWarning(const char* warningString)
{
}

void PhysicsDebugRenderer::draw3dText(const btVector3& location, const char* textString)
{
}

void PhysicsDebugRenderer::setDebugMode(int debugMode)
{
}

int PhysicsDebugRenderer::getDebugMode() const
{
  return true;
}

} // namespace Dali::Toolkit::Physics::Internal
