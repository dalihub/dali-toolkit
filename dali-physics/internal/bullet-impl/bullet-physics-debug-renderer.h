#pragma once

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

// External includes
#include <GLES3/gl3.h>
#include <LinearMath/btIDebugDraw.h>
#include <dali/dali.h>

using Dali::Actor;
using Dali::CameraActor;
using Dali::Geometry;
using Dali::Renderer;
using Dali::Shader;
using Dali::TextureSet;

namespace Dali::Toolkit::Physics::Internal
{
class PhysicsAdaptor;

class PhysicsDebugRenderer : public btIDebugDraw
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

public: // Inherited from btIDebugDraw
  // Assume this is called during FrameCallback (i.e. in update manager, rather than during render...)
  // Generate stack of lines... render, then clear stack.
  void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
  void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
  void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
  void reportErrorWarning(const char* warningString) override;
  void draw3dText(const btVector3& location, const char* textString) override;
  void setDebugMode(int debugMode) override;
  int  getDebugMode() const override;

private:
  bool OnRender(const Dali::RenderCallbackInput& input);
  void Setup();
  void PrepareShader();
  void RenderLines(const Dali::RenderCallbackInput& input);

private:
  CameraActor                           mCamera;
  Renderer                              mDebugRenderer;
  std::unique_ptr<Dali::RenderCallback> mRenderCallback;

  enum class State
  {
    INIT,
    RENDER
  } mState{State::INIT};

  struct VertexLine
  {
    Dali::Vector3 position;
    Dali::Vector3 color;
  };
  std::vector<VertexLine> mLines;

  Dali::Matrix    mModelViewMatrix;
  Dali::Matrix    mViewMatrix;
  Dali::Matrix    mProjectionMatrix;
  int             mWidth;
  int             mHeight;
  PhysicsAdaptor& mAdaptor;
  GLint           mVertexLocation;
  GLint           mVertexColourLocation;
  GLint           mProjectionLocation;
  GLint           mModelViewLocation;
  GLuint          mBufferId;
  GLuint          mProgramId;
};

} // namespace Dali::Toolkit::Physics::Internal
