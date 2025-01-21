/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/addon-binder.h>
#include <dali/public-api/adaptor-framework/graphics-backend.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

// Internal Includes
#include <dali-physics/internal/physics-adaptor-impl.h>

using Dali::Degree;
using Dali::Matrix;
using Dali::Quaternion;
using Dali::Radian;
using Dali::Vector3;
using Dali::Vector4;

namespace Dali::Toolkit::Physics::Internal
{
namespace
{
const char* const DALI_PHYSICS_BULLET_GLES_SO("libdali2-physics-3d-gles.so");
const char* const DALI_PHYSICS_BULLET_GLES_ADDON_NAME("PhysicsBulletGlesAddOn");

struct PhysicsBulletGlesAddOn : public Dali::AddOn::AddOnBinder
{
  PhysicsBulletGlesAddOn()
  : Dali::AddOn::AddOnBinder(DALI_PHYSICS_BULLET_GLES_ADDON_NAME, DALI_PHYSICS_BULLET_GLES_SO)
  {
    DALI_ASSERT_ALWAYS(CreateGlesPhysicsDebugRenderer && "CreateGlesPhysicsDebugRenderer not loaded!");
    DALI_ASSERT_ALWAYS(DeleteGlesPhysicsDebugRenderer && "DeleteGlesPhysicsDebugRenderer not loaded!");
    DALI_ASSERT_ALWAYS(SetViewport && "SetViewport not loaded!");
    DALI_ASSERT_ALWAYS(Setup && "Setup not loaded!");
    DALI_ASSERT_ALWAYS(RenderLines && "RenderLines not loaded!");
  }

  ~PhysicsBulletGlesAddOn() = default;

  ADDON_BIND_FUNCTION(CreateGlesPhysicsDebugRenderer, Gles::DebugRenderer*());
  ADDON_BIND_FUNCTION(DeleteGlesPhysicsDebugRenderer, void(Gles::DebugRenderer*));
  ADDON_BIND_FUNCTION(SetViewport, void(int, int));
  ADDON_BIND_FUNCTION(Setup, void(Gles::DebugRenderer&, int, int));
  ADDON_BIND_FUNCTION(RenderLines, void(Gles::DebugRenderer&, const void*, std::size_t, int, const Dali::Matrix&, const Dali::Matrix&));
};

std::unique_ptr<PhysicsBulletGlesAddOn> gPhysicsBulletGlesAddOn;
} // namespace

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
  if(Graphics::GetCurrentGraphicsBackend() == Graphics::Backend::GLES)
  {
    if(!gPhysicsBulletGlesAddOn)
    {
      gPhysicsBulletGlesAddOn.reset(new PhysicsBulletGlesAddOn);
    }
    DALI_ASSERT_ALWAYS(gPhysicsBulletGlesAddOn && "Cannot load the Bullet Debug Renderer Gles Addon\n");
    mImpl = gPhysicsBulletGlesAddOn->CreateGlesPhysicsDebugRenderer();
  }
}

PhysicsDebugRenderer::~PhysicsDebugRenderer()
{
  if(gPhysicsBulletGlesAddOn && gPhysicsBulletGlesAddOn->DeleteGlesPhysicsDebugRenderer)
  {
    gPhysicsBulletGlesAddOn->DeleteGlesPhysicsDebugRenderer(mImpl);
    mImpl = nullptr;
  }
}

bool PhysicsDebugRenderer::OnRender(const Dali::RenderCallbackInput& input)
{
  if(gPhysicsBulletGlesAddOn && mImpl)
  {
    if(mState == State::INIT)
    {
      gPhysicsBulletGlesAddOn->Setup(*mImpl, mWidth, mHeight);
      mState = State::RENDER;
    }
    gPhysicsBulletGlesAddOn->SetViewport(mWidth, mHeight);

    Render(input);
  }

  return false;
}

void PhysicsDebugRenderer::UpdateWindowSize(Dali::Vector2 size)
{
  mWidth  = size.width;
  mHeight = size.height;
}

void PhysicsDebugRenderer::Render(const Dali::RenderCallbackInput& input)
{
  mModelViewMatrix.SetIdentity();
  mProjectionMatrix = input.projection;

  Matrix::Multiply(mModelViewMatrix, mModelViewMatrix, input.view);

  // In theory, input.clippingBox should tell us the actor position in clip-space.
  // But, it appears to be bugged.

  if(gPhysicsBulletGlesAddOn && mImpl)
  {
    gPhysicsBulletGlesAddOn->RenderLines(
      *mImpl,
      &mLines[0],
      mLines.size() * sizeof(VertexLine),
      mLines.size(),
      mModelViewMatrix,
      mProjectionMatrix);
  }

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
