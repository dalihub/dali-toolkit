#pragma once

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

#include <chipmunk/chipmunk.h>
#include <dali/dali.h>

using Dali::Actor;
using Dali::CameraActor;
using Dali::Geometry;
using Dali::Renderer;
using Dali::Shader;
using Dali::TextureSet;

namespace Dali::Toolkit::Physics::Internal
{
namespace Gles
{
struct DebugRenderer;
}

class PhysicsAdaptor;

class PhysicsDebugRenderer
{
public:
  // Creates and initializes a new renderer
  static std::unique_ptr<PhysicsDebugRenderer> New(uint32_t width, uint32_t height, Dali::CameraActor camera, PhysicsAdaptor* adaptor);
  /**
   * Get the callback (for actor creation)
   */

  std::unique_ptr<Dali::RenderCallback>& GetCallback()
  {
    return mRenderCallback;
  }

  void UpdateWindowSize(Dali::Vector2 size);

  /**
   * Constructor.
   * @param[in] width Width of the renderer - viewport
   * @param[in] height Height of the renderer - viewport
   */
  PhysicsDebugRenderer(uint32_t width, uint32_t height, Dali::CameraActor camera, PhysicsAdaptor* adaptor);

  /**
   * Destructor
   */
  ~PhysicsDebugRenderer();

  /**
   * Get the drawing options struct ( construct only )
   */
  const cpSpaceDebugDrawOptions& GetDebugDrawOptions()
  {
    return mDebugDrawOptions;
  }

  struct Vertex
  {
    Dali::Vector2 position;
    Dali::Vector2 uvs;
    float         radius;
    Dali::Vector4 fillColor;
    Dali::Vector4 outlineColor;
  };

public: // Debug functions (Creates indices & verts)
  void              DrawCircle(cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor);
  void              DrawSegment(cpVect a, cpVect b, cpSpaceDebugColor color);
  void              DrawFatSegment(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor);
  void              DrawPolygon(int count, const cpVect* verts, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor);
  void              DrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor color);
  cpSpaceDebugColor DrawColorForShape(cpShape* shape);

private:
  bool OnRender(const Dali::RenderCallbackInput& input);
  void Setup();
  void PrepareShader();
  void Render(const Dali::RenderCallbackInput& input);

  Vertex* PushVertices(uint32_t vertexCount, uint32_t indexCount, const uint16_t* indices);
  Vertex  MakeVertex(cpVect pos, float u, float v, float r, Vector4 fill, Vector4 outline);

private:
  CameraActor                           mCamera;
  cpSpaceDebugDrawOptions               mDebugDrawOptions;
  Renderer                              mDebugRenderer;
  std::unique_ptr<Dali::RenderCallback> mRenderCallback;

  enum class State
  {
    INIT,
    RENDER
  } mState{State::INIT};

  std::vector<Vertex>   mVertices;
  std::vector<uint16_t> mIndices;

  Dali::Matrix    mModelViewMatrix;
  Dali::Matrix    mViewMatrix;
  Dali::Matrix    mProjectionMatrix;
  int             mWidth;
  int             mHeight;
  PhysicsAdaptor& mAdaptor;

  float mPointLineScale{2.0f};

  Gles::DebugRenderer* mImpl{nullptr};
};

} // namespace Dali::Toolkit::Physics::Internal
