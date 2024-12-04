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

// External Includes
#include <GLES3/gl3.h>
#include <dali/devel-api/addons/addon-base.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/math/matrix.h>

// Internal Includes
#include <dali-physics/internal/physics-debug-renderer-gles.h>

namespace Dali::Toolkit::Physics::Internal::Gles
{
struct DebugRenderer
{
  GLint  mVertexLocation{-1};
  GLint  mVertexColourLocation{-1};
  GLint  mProjectionLocation{-1};
  GLint  mModelViewLocation{-1};
  GLuint mBufferId{0u};
  GLuint mProgramId{0u};
};
} // namespace Dali::Toolkit::Physics::Internal::Gles

namespace
{
using DebugRenderer = Dali::Toolkit::Physics::Internal::Gles::DebugRenderer;
const char* const DALI_PHYSICS_BULLET_GLES_ADDON_NAME("PhysicsBulletGlesAddOn");

void PrepareShader(DebugRenderer& renderer)
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

  renderer.mProgramId = Dali::Toolkit::Physics::Internal::Gles::CreateProgram(glVertexShader, glFragmentShader);
}

DebugRenderer* CreateGlesPhysicsDebugRenderer()
{
  return new DebugRenderer;
}

void DeleteGlesPhysicsDebugRenderer(DebugRenderer* renderer)
{
  delete renderer;
}

void SetViewport(int width, int height)
{
  glViewport(0, 0, width, height);
}

void Setup(DebugRenderer& renderer, int width, int height)
{
  PrepareShader(renderer);
  renderer.mVertexLocation       = glGetAttribLocation(renderer.mProgramId, "vertexPosition");
  renderer.mVertexColourLocation = glGetAttribLocation(renderer.mProgramId, "vertexColour");
  renderer.mProjectionLocation   = glGetUniformLocation(renderer.mProgramId, "projection");
  renderer.mModelViewLocation    = glGetUniformLocation(renderer.mProgramId, "modelView");

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, width, height);

  glGenBuffers(1, &renderer.mBufferId);
}

void RenderLines(
  DebugRenderer&      renderer,
  const void*         data,
  std::size_t         dataSize,
  int                 arrayCount,
  const Dali::Matrix& modelViewMatrix,
  const Dali::Matrix& projectionMatrix)
{
  glUseProgram(renderer.mProgramId);

  glBindBuffer(GL_ARRAY_BUFFER, renderer.mBufferId);
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(dataSize), data, GL_STATIC_DRAW);

  glVertexAttribPointer(renderer.mVertexLocation, 3, GL_FLOAT, GL_FALSE, 24, 0);
  glEnableVertexAttribArray(renderer.mVertexLocation);
  glVertexAttribPointer(renderer.mVertexColourLocation, 3, GL_FLOAT, GL_FALSE, 24, reinterpret_cast<const void*>(12));
  glEnableVertexAttribArray(renderer.mVertexColourLocation);
  glUniformMatrix4fv(renderer.mProjectionLocation, 1, GL_FALSE, projectionMatrix.AsFloat());
  glUniformMatrix4fv(renderer.mModelViewLocation, 1, GL_FALSE, modelViewMatrix.AsFloat());

  glDrawArrays(GL_LINES, 0, arrayCount);
}
} // unnamed namespace

class PhysicsBulletGlesAddOn : public Dali::AddOns::AddOnBase
{
public:
  void GetAddOnInfo(Dali::AddOnInfo& info) override
  {
    info.type    = Dali::AddOnType::GENERIC;
    info.name    = DALI_PHYSICS_BULLET_GLES_ADDON_NAME;
    info.version = Dali::DALI_ADDON_VERSION(1, 0, 0);
    info.next    = nullptr;
  }

  /**
   * Dispatch table for global functions
   * @return
   */
  Dali::AddOns::DispatchTable* GetGlobalDispatchTable() override
  {
    static Dali::AddOns::DispatchTable dispatchTable{};
    if(dispatchTable.Empty())
    {
      dispatchTable["CreateGlesPhysicsDebugRenderer"] = CreateGlesPhysicsDebugRenderer;
      dispatchTable["DeleteGlesPhysicsDebugRenderer"] = DeleteGlesPhysicsDebugRenderer;
      dispatchTable["SetViewport"]                    = SetViewport;
      dispatchTable["Setup"]                          = Setup;
      dispatchTable["RenderLines"]                    = RenderLines;
    }
    return &dispatchTable;
  }

  /**
   * Dispatch table for instance functions
   * @return
   */
  Dali::AddOns::DispatchTable* GetInstanceDispatchTable() override
  {
    return nullptr;
  }
};

REGISTER_ADDON_CLASS(PhysicsBulletGlesAddOn);
