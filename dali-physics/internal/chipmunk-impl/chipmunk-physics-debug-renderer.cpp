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

// External Includes
#include <chipmunk/chipmunk.h>
#include <dali/devel-api/common/addon-binder.h>
#include <dali/public-api/adaptor-framework/graphics-backend.h>

// Internal Includes
#include <dali-physics/internal/chipmunk-impl/chipmunk-physics-adaptor-impl.h>
#include <dali-physics/internal/chipmunk-impl/chipmunk-physics-debug-renderer.h>

namespace Dali::Toolkit::Physics::Internal
{
namespace
{
const char* const DALI_PHYSICS_CHIPMUNK_GLES_SO("libdali2-physics-2d-gles.so");
const char* const DALI_PHYSICS_CHIPMUNK_GLES_ADDON_NAME("PhysicsChipmunkGlesAddOn");

struct PhysicsChipmunkGlesAddOn : public Dali::AddOn::AddOnBinder
{
  PhysicsChipmunkGlesAddOn()
  : Dali::AddOn::AddOnBinder(DALI_PHYSICS_CHIPMUNK_GLES_ADDON_NAME, DALI_PHYSICS_CHIPMUNK_GLES_SO)
  {
    DALI_ASSERT_ALWAYS(CreateGlesPhysicsDebugRenderer && "CreateGlesPhysicsDebugRenderer not loaded!");
    DALI_ASSERT_ALWAYS(DeleteGlesPhysicsDebugRenderer && "DeleteGlesPhysicsDebugRenderer not loaded!");
    DALI_ASSERT_ALWAYS(SetViewport && "SetViewport not loaded!");
    DALI_ASSERT_ALWAYS(Setup && "Setup not loaded!");
    DALI_ASSERT_ALWAYS(RenderLines && "RenderLines not loaded!");
  }

  ~PhysicsChipmunkGlesAddOn() = default;

  ADDON_BIND_FUNCTION(CreateGlesPhysicsDebugRenderer, Gles::DebugRenderer*());
  ADDON_BIND_FUNCTION(DeleteGlesPhysicsDebugRenderer, void(Gles::DebugRenderer*));
  ADDON_BIND_FUNCTION(SetViewport, void(int, int));
  ADDON_BIND_FUNCTION(Setup, void(Gles::DebugRenderer&, int, int));
  ADDON_BIND_FUNCTION(RenderLines, void(Gles::DebugRenderer&, const void*, std::size_t, int, const void*, std::size_t, const Dali::Matrix&, const Dali::Matrix&));
};

std::unique_ptr<PhysicsChipmunkGlesAddOn> gPhysicsChipmunkGlesAddOn;
} // namespace

static void DebugDrawCircleImpl(cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  debugRenderer->DrawCircle(pos, angle, radius, outlineColor, fillColor);
}

static void DebugDrawSegmentImpl(cpVect a, cpVect b, cpSpaceDebugColor color, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  debugRenderer->DrawSegment(a, b, color);
}

void DebugDrawFatSegmentImpl(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  debugRenderer->DrawFatSegment(a, b, radius, outlineColor, fillColor);
}

void DebugDrawPolygonImpl(int count, const cpVect* verts, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  debugRenderer->DrawPolygon(count, verts, radius, outlineColor, fillColor);
}

void DebugDrawDotImpl(cpFloat size, cpVect pos, cpSpaceDebugColor color, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  debugRenderer->DrawDot(size, pos, color);
}

cpSpaceDebugColor DebugDrawColorForShapeImpl(cpShape* shape, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  return debugRenderer->DrawColorForShape(shape);
}

std::unique_ptr<PhysicsDebugRenderer> PhysicsDebugRenderer::New(uint32_t width, uint32_t height, Dali::CameraActor camera, PhysicsAdaptor* adaptor)
{
  auto renderer = std::make_unique<PhysicsDebugRenderer>(width, height, camera, adaptor);

  renderer->mRenderCallback = Dali::RenderCallback::New<PhysicsDebugRenderer>(renderer.get(), &PhysicsDebugRenderer::OnRender);
  return renderer;
}

PhysicsDebugRenderer::PhysicsDebugRenderer(uint32_t width, uint32_t height, Dali::CameraActor camera, PhysicsAdaptor* adaptor)
: mCamera(camera),
  mWidth(width),
  mHeight(height),
  mAdaptor(*adaptor)
{
  mDebugDrawOptions.drawCircle     = DebugDrawCircleImpl;
  mDebugDrawOptions.drawSegment    = DebugDrawSegmentImpl;
  mDebugDrawOptions.drawFatSegment = DebugDrawFatSegmentImpl;
  mDebugDrawOptions.drawPolygon    = DebugDrawPolygonImpl;
  mDebugDrawOptions.drawDot        = DebugDrawDotImpl;

  mDebugDrawOptions.flags               = static_cast<cpSpaceDebugDrawFlags>(CP_SPACE_DEBUG_DRAW_SHAPES |
                                                               CP_SPACE_DEBUG_DRAW_COLLISION_POINTS |
                                                               CP_SPACE_DEBUG_DRAW_CONSTRAINTS);
  mDebugDrawOptions.colorForShape       = DebugDrawColorForShapeImpl;
  mDebugDrawOptions.shapeOutlineColor   = cpSpaceDebugColor{0.0f, 1.0f, 1.0f, 0.9f};
  mDebugDrawOptions.constraintColor     = cpSpaceDebugColor{0.5f, 0.5f, 0.5f, 0.9f};
  mDebugDrawOptions.collisionPointColor = cpSpaceDebugColor{1.0f, 0.0f, 0.0f, 1.0f};
  mDebugDrawOptions.data                = this;

  if(Graphics::GetCurrentGraphicsBackend() == Graphics::Backend::GLES)
  {
    if(!gPhysicsChipmunkGlesAddOn)
    {
      gPhysicsChipmunkGlesAddOn.reset(new PhysicsChipmunkGlesAddOn);
    }
    DALI_ASSERT_ALWAYS(gPhysicsChipmunkGlesAddOn && "Cannot load the Chipmunk Debug Renderer Gles Addon\n");
    mImpl = gPhysicsChipmunkGlesAddOn->CreateGlesPhysicsDebugRenderer();
  }
}

PhysicsDebugRenderer::~PhysicsDebugRenderer()
{
  if(gPhysicsChipmunkGlesAddOn && gPhysicsChipmunkGlesAddOn->DeleteGlesPhysicsDebugRenderer)
  {
    gPhysicsChipmunkGlesAddOn->DeleteGlesPhysicsDebugRenderer(mImpl);
    mImpl = nullptr;
  }
}

bool PhysicsDebugRenderer::OnRender(const Dali::RenderCallbackInput& input)
{
  if(mState == State::INIT)
  {
    if(gPhysicsChipmunkGlesAddOn && mImpl)
    {
      gPhysicsChipmunkGlesAddOn->Setup(*mImpl, mWidth, mHeight);
    }
    mState = State::RENDER;
  }

  if(gPhysicsChipmunkGlesAddOn)
  {
    gPhysicsChipmunkGlesAddOn->SetViewport(mWidth, mHeight);
  }

  Render(input);
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

  if(gPhysicsChipmunkGlesAddOn && mImpl)
  {
    gPhysicsChipmunkGlesAddOn->RenderLines(
      *mImpl,
      &mIndices[0],
      mIndices.size() * sizeof(uint16_t),
      mIndices.size(),
      &mVertices[0],
      mVertices.size() * sizeof(Vertex),
      mModelViewMatrix,
      mProjectionMatrix);
  }

  mIndices.clear();
  mVertices.clear();
}

PhysicsDebugRenderer::Vertex* PhysicsDebugRenderer::PushVertices(uint32_t vertexCount, uint32_t indexCount, const uint16_t* indices)
{
  auto base = mVertices.size();
  mVertices.resize(mVertices.size() + vertexCount);
  mIndices.reserve(mIndices.size() + indexCount);
  for(uint32_t i = 0; i < indexCount; ++i)
  {
    mIndices.push_back(base + indices[i]);
  }

  return &mVertices[base];
}

PhysicsDebugRenderer::Vertex PhysicsDebugRenderer::MakeVertex(cpVect pos, float u, float v, float r, Vector4 fill, Vector4 outline)
{
  auto daliPos = mAdaptor.TranslateFromPhysicsSpace(Vector3((float)pos.x, (float)pos.y, 0.0f));
  return Vertex{Vector2(daliPos.x, daliPos.y), Vector2(u, v), r, fill, outline};
}

void PhysicsDebugRenderer::DrawCircle(cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
{
  float                 r = (float)radius + mPointLineScale;
  Vector4               fill(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
  Vector4               outline(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
  static const uint16_t indices[] = {0, 1, 2, 0, 2, 3};

  Vertex* vertices = PushVertices(4, 6, indices);

  vertices[0] = MakeVertex(pos, -1, -1, r, fill, outline);
  vertices[1] = MakeVertex(pos, -1, 1, r, fill, outline);
  vertices[2] = MakeVertex(pos, 1, 1, r, fill, outline);
  vertices[3] = MakeVertex(pos, 1, -1, r, fill, outline);

  DrawSegment(pos, cpvadd(pos, cpvmult(cpvforangle(angle), 0.75f * radius)), outlineColor);
}

void PhysicsDebugRenderer::DrawSegment(cpVect a, cpVect b, cpSpaceDebugColor color)
{
  DrawFatSegment(a, b, 0.0f, color, color);
}

void PhysicsDebugRenderer::DrawFatSegment(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
{
  static const uint16_t indices[] = {0, 1, 2, 1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7};
  Vertex*               vertices  = PushVertices(8, 18, indices);

  cpVect t = cpvnormalize(cpvsub(b, a));

  float   r = (float)radius * mPointLineScale;
  Vector4 fill(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
  Vector4 outline(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);

  vertices[0] = MakeVertex(a, (-t.x + t.y), (-t.x - t.y), r, fill, outline);
  vertices[1] = MakeVertex(a, (-t.x - t.y), (+t.x - t.y), r, fill, outline);
  vertices[2] = MakeVertex(a, (-0.0 + t.y), (-t.x + 0.0), r, fill, outline);
  vertices[3] = MakeVertex(a, (-0.0 - t.y), (+t.x + 0.0), r, fill, outline);
  vertices[4] = MakeVertex(a, (+0.0 + t.y), (-t.x - 0.0), r, fill, outline);
  vertices[5] = MakeVertex(a, (+0.0 - t.y), (+t.x - 0.0), r, fill, outline);
  vertices[6] = MakeVertex(a, (+t.x + t.y), (-t.x + t.y), r, fill, outline);
  vertices[7] = MakeVertex(a, (+t.x - t.y), (+t.x + t.y), r, fill, outline);
}

void PhysicsDebugRenderer::DrawPolygon(int count, const cpVect* verts, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
{
  Vector4 fill(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
  Vector4 outline(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);

  std::vector<uint16_t> indices;
  for(int i = 0; i < count - 2; i++)
  {
    indices.push_back(0);
    indices.push_back(4 * (i + 1));
    indices.push_back(4 * (i + 2));
  }

  // Polygon outline triangles.
  for(int i0 = 0; i0 < count; i0++)
  {
    int i1 = (i0 + 1) % count;
    indices.push_back(4 * i0 + 0);
    indices.push_back(4 * i0 + 1);
    indices.push_back(4 * i0 + 2);
    indices.push_back(4 * i0 + 0);
    indices.push_back(4 * i0 + 2);
    indices.push_back(4 * i0 + 3);
    indices.push_back(4 * i0 + 0);
    indices.push_back(4 * i0 + 3);
    indices.push_back(4 * i1 + 0);
    indices.push_back(4 * i0 + 3);
    indices.push_back(4 * i1 + 0);
    indices.push_back(4 * i1 + 1);
  }

  float inset  = (float)-cpfmax(0, 2 * mPointLineScale - radius);
  float outset = (float)radius + mPointLineScale;
  float r      = outset - inset;

  Vertex* vertices = PushVertices(4 * count, 3 * (5 * count - 2), &indices[0]);
  for(int i = 0; i < count; i++)
  {
    cpVect v0     = verts[i];
    cpVect v_prev = verts[(i + (count - 1)) % count];
    cpVect v_next = verts[(i + (count + 1)) % count];

    cpVect n1 = cpvnormalize(cpvrperp(cpvsub(v0, v_prev)));
    cpVect n2 = cpvnormalize(cpvrperp(cpvsub(v_next, v0)));
    cpVect of = cpvmult(cpvadd(n1, n2), 1.0 / (cpvdot(n1, n2) + 1.0f));
    cpVect v  = cpvadd(v0, cpvmult(of, inset));

    vertices[4 * i + 0] = MakeVertex(v, 0.0f, 0.0f, 0.0f, fill, outline);
    vertices[4 * i + 1] = MakeVertex(v, (float)n1.x, (float)n1.y, r, fill, outline);
    vertices[4 * i + 2] = MakeVertex(v, (float)of.x, (float)of.y, r, fill, outline);
    vertices[4 * i + 3] = MakeVertex(v, (float)n2.x, (float)n2.y, r, fill, outline);
  }
}

void PhysicsDebugRenderer::DrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor color)
{
  float                 r = (float)(size * 0.5f * mPointLineScale);
  Vector4               fill(color.r, color.g, color.b, color.a);
  static const uint16_t indices[] = {0, 1, 2, 0, 2, 3};
  Vertex*               vertex    = PushVertices(4, 6, indices);
  vertex[0]                       = MakeVertex(pos, -1, -1, r, fill, fill);
  vertex[1]                       = MakeVertex(pos, -1, 1, r, fill, fill);
  vertex[2]                       = MakeVertex(pos, 1, 1, r, fill, fill);
  vertex[3]                       = MakeVertex(pos, 1, -1, r, fill, fill);
}

cpSpaceDebugColor PhysicsDebugRenderer::DrawColorForShape(cpShape* shape)
{
  static cpSpaceDebugColor Colors[] = {
    {0xb5 / 255.0f, 0x89 / 255.0f, 0x00 / 255.0f, 1.0f},
    {0xcb / 255.0f, 0x4b / 255.0f, 0x16 / 255.0f, 1.0f},
    {0xdc / 255.0f, 0x32 / 255.0f, 0x2f / 255.0f, 1.0f},
    {0xd3 / 255.0f, 0x36 / 255.0f, 0x82 / 255.0f, 1.0f},
    {0x6c / 255.0f, 0x71 / 255.0f, 0xc4 / 255.0f, 1.0f},
    {0x26 / 255.0f, 0x8b / 255.0f, 0xd2 / 255.0f, 1.0f},
    {0x2a / 255.0f, 0xa1 / 255.0f, 0x98 / 255.0f, 1.0f},
    {0x85 / 255.0f, 0x99 / 255.0f, 0x00 / 255.0f, 1.0f},
  };

  if(cpShapeGetSensor(shape))
  {
    return cpSpaceDebugColor{1.0f, 1.0f, 1.0f, 0.1f};
  }
  else
  {
    cpBody* body = cpShapeGetBody(shape);

    if(cpBodyIsSleeping(body))
    {
      return cpSpaceDebugColor{0x58 / 255.0f, 0x6e / 255.0f, 0x75 / 255.0f, 1.0f};
    }
    else if(cpBodyIsSleepThresholdExceeded(body, shape))
    {
      return cpSpaceDebugColor{0x93 / 255.0f, 0xa1 / 255.0f, 0xa1 / 255.0f, 1.0f};
    }
    else
    {
      uint32_t val = (uint32_t)cpShapeGetHashId(shape);

      // scramble the bits up using Robert Jenkins' 32 bit integer hash function
      val = (val + 0x7ed55d16) + (val << 12);
      val = (val ^ 0xc761c23c) ^ (val >> 19);
      val = (val + 0x165667b1) + (val << 5);
      val = (val + 0xd3a2646c) ^ (val << 9);
      val = (val + 0xfd7046c5) + (val << 3);
      val = (val ^ 0xb55a4f09) ^ (val >> 16);

      return Colors[val & 0x7];
    }
  }
}

} // namespace Dali::Toolkit::Physics::Internal
