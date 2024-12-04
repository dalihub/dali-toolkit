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
  GLint  mPositionLocation{-1};
  GLint  mUvsLocation{-1};
  GLint  mRadiusLocation{-1};
  GLint  mFillColourLocation{-1};
  GLint  mOutlineColourLocation{-1};
  GLint  mProjectionLocation{-1};
  GLint  mModelViewLocation{-1};
  GLuint mIndexBufferId{0u};
  GLuint mVertexBufferId{0u};
  GLuint mProgramId{0u};
};
} // namespace Dali::Toolkit::Physics::Internal::Gles

namespace
{
using DebugRenderer = Dali::Toolkit::Physics::Internal::Gles::DebugRenderer;
const char* const DALI_PHYSICS_CHIPMUNK_GLES_ADDON_NAME("PhysicsChipmunkGlesAddOn");

void PrepareShader(DebugRenderer& renderer)
{
  static const char glVertexShader[] =
    "#version 300 es\n"
    "in vec2 position;\n"
    "in vec2 uvs;\n"
    "in float radius;\n"
    "in vec4 fillColor;\n"
    "in vec4 outlineColor;\n"
    "out vec2 v_uvs;\n"
    "out vec4 v_fill;\n"
    "out vec4 v_outline;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 modelView;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * modelView * vec4(position.xy+radius*uvs, 0.0, 1.0);\n"
    "    v_uvs=uvs;\n"
    "    v_fill = fillColor;\n"
    "    v_fill.rgb *= v_fill.a;\n"
    "    v_outline = outlineColor;\n"
    "    v_outline.a *= v_outline.a;\n"
    "}\n";

  static const char glFragmentShader[] =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_uvs;\n"
    "in vec4 v_fill;\n"
    "in vec4 v_outline;\n"
    "out vec4 fragColor;\n"
    "void main()\n"
    "{\n"
    "    float len=length(v_uvs);\n"
    "    float fw = length(vec2(dFdx(len), dFdy(len)));\n"
    "    float mask=smoothstep(-1.0, fw-1.0, -len);\n"
    "    float outline=1.0-fw;\n"
    "    float outline_mask=smoothstep(outline-fw, outline, len);\n"
    "    vec4 color = v_fill + (v_outline - v_fill*v_outline.a)*outline_mask;\n"
    "    fragColor = color*mask;\n"
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
  renderer.mPositionLocation      = glGetAttribLocation(renderer.mProgramId, "position");
  renderer.mUvsLocation           = glGetAttribLocation(renderer.mProgramId, "uvs");
  renderer.mRadiusLocation        = glGetAttribLocation(renderer.mProgramId, "radius");
  renderer.mFillColourLocation    = glGetAttribLocation(renderer.mProgramId, "fillColor");
  renderer.mOutlineColourLocation = glGetAttribLocation(renderer.mProgramId, "outlineColor");

  renderer.mProjectionLocation = glGetUniformLocation(renderer.mProgramId, "projection");
  renderer.mModelViewLocation  = glGetUniformLocation(renderer.mProgramId, "modelView");

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, width, height);

  glGenBuffers(1, &renderer.mIndexBufferId);
  glGenBuffers(1, &renderer.mVertexBufferId);
}

void RenderLines(
  DebugRenderer&      renderer,
  const void*         indicesData,
  std::size_t         indicesDataSize,
  int                 indicesCount,
  const void*         verticesData,
  std::size_t         verticesDataSize,
  const Dali::Matrix& modelViewMatrix,
  const Dali::Matrix& projectionMatrix)
{
  glUseProgram(renderer.mProgramId);

  // In theory, input.clippingBox should tell us the actor position in clip-space.
  // But, it appears to be bugged.
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.mIndexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indicesDataSize), indicesData, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, renderer.mVertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(verticesDataSize), verticesData, GL_STATIC_DRAW);

  GLint stride = 52; // 4*(2 + 2 + 1 + 4 + 4) = 4*13=52
  glVertexAttribPointer(renderer.mPositionLocation, 2, GL_FLOAT, GL_FALSE, stride, 0);
  glEnableVertexAttribArray(renderer.mPositionLocation);

  glVertexAttribPointer(renderer.mUvsLocation, 2, GL_FLOAT, GL_FALSE, stride, (const void*)8);
  glEnableVertexAttribArray(renderer.mUvsLocation);

  glVertexAttribPointer(renderer.mRadiusLocation, 1, GL_FLOAT, GL_FALSE, stride, (const void*)16);
  glEnableVertexAttribArray(renderer.mRadiusLocation);

  glVertexAttribPointer(renderer.mFillColourLocation, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(20));
  glEnableVertexAttribArray(renderer.mFillColourLocation);
  glVertexAttribPointer(renderer.mOutlineColourLocation, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(36));
  glEnableVertexAttribArray(renderer.mOutlineColourLocation);

  glUniformMatrix4fv(renderer.mProjectionLocation, 1, GL_FALSE, projectionMatrix.AsFloat());
  glUniformMatrix4fv(renderer.mModelViewLocation, 1, GL_FALSE, modelViewMatrix.AsFloat());

  glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, 0);
}

} // unnamed namespace

class PhysicsChipmunkGlesAddOn : public Dali::AddOns::AddOnBase
{
public:
  void GetAddOnInfo(Dali::AddOnInfo& info) override
  {
    info.type    = Dali::AddOnType::GENERIC;
    info.name    = DALI_PHYSICS_CHIPMUNK_GLES_ADDON_NAME;
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

REGISTER_ADDON_CLASS(PhysicsChipmunkGlesAddOn);
