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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/controls/gl-view/gl-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
Dali::Toolkit::GlView GlView::New(Dali::Toolkit::GlView::ColorFormat colorFormat)
{
  auto*                 impl   = new Dali::Toolkit::Internal::GlView(colorFormat);
  Dali::Toolkit::GlView handle = Dali::Toolkit::GlView(*impl);
  impl->Initialize();
  return handle;
}

GlView::GlView(Dali::Toolkit::GlView::ColorFormat colorFormat)
: Dali::Toolkit::Internal::GlViewImpl(Toolkit::GlView::BackendMode::EGL_IMAGE_OFFSCREEN_RENDERING),
  mRenderThread(nullptr),
  mNativeImageQueue(nullptr),
  mRenderingMode(Toolkit::GlView::RenderingMode::CONTINUOUS),
  mColorFormat(colorFormat),
  mDepth(false),
  mStencil(false),
  mMSAA(0)
{
}

GlView::~GlView()
{
  if(mRenderThread)
  {
    mRenderThread->Stop();
    mRenderThread->Join();
  }
}

void GlView::RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback, CallbackBase* terminateCallback)
{
  if(mRenderThread)
  {
    mRenderThread->RegisterGlCallbacks(initCallback, renderFrameCallback, terminateCallback);
  }
}

void GlView::SetResizeCallback(CallbackBase* resizeCallback)
{
  if(mRenderThread)
  {
    mRenderThread->SetResizeCallback(resizeCallback);
  }
}

bool GlView::SetGraphicsConfig(bool depth, bool stencil, int msaa, Dali::Toolkit::GlView::GraphicsApiVersion version)
{
  // Init Graphics
  mDepth   = depth;
  mStencil = stencil;
  mMSAA    = msaa;

  int rVersion;

  if(version == Dali::Toolkit::GlView::GraphicsApiVersion::GLES_VERSION_2_0)
  {
    rVersion = 20;
  }
  else
  {
    rVersion = 30;
  }

  if(mRenderThread)
  {
    return mRenderThread->SetGraphicsConfig(depth, stencil, msaa, rVersion);
  }

  return false;
}

void GlView::SetRenderingMode(Dali::Toolkit::GlView::RenderingMode mode)
{
  mRenderingMode    = mode;
  Renderer renderer = Self().GetRendererAt(0);

  if(mRenderingMode == Dali::Toolkit::GlView::RenderingMode::ON_DEMAND)
  {
    renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::IF_REQUIRED);

    if(mRenderThread)
    {
      mRenderThread->SetOnDemandRenderMode(true);
    }
  }
  else
  {
    renderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);

    if(mRenderThread)
    {
      mRenderThread->SetOnDemandRenderMode(false);
    }
  }
}

Dali::Toolkit::GlView::RenderingMode GlView::GetRenderingMode() const
{
  return mRenderingMode;
}

void GlView::RenderOnce()
{
  if(mRenderThread)
  {
    mRenderThread->RenderOnce();
  }
}

void GlView::BindTextureResources(std::vector<Dali::Texture> textures)
{
  // Not supported in the indirect mode
}

void GlView::OnInitialize()
{
  //Create NativeImageSourceQueue with the size of 1,1
  mNativeImageQueue = Dali::NativeImageSourceQueue::New(1, 1, GetColorFormat(mColorFormat));

  if(!mNativeImageQueue)
  {
    DALI_LOG_ERROR("NativeImageSourceQueue is NULL");
    return;
  }

  AddRenderer();

  Actor self = Self();

  //Create a RenderThread
  mRenderThread = std::unique_ptr<GlViewRenderThread>(new GlViewRenderThread(mNativeImageQueue));
  if(!mRenderThread)
  {
    DALI_LOG_ERROR("Fail to create GlView Render Thread!!!!\n");
    return;
  }

  //Adding VisibilityChange Signal.
  self.InheritedVisibilityChangedSignal().Connect(this, &GlView::OnControlInheritedVisibilityChanged);
}

void GlView::OnSizeSet(const Vector3& targetSize)
{
  Control::OnSizeSet(targetSize);

  if(mRenderThread)
  {
    if(mNativeImageQueue)
    {
      mRenderThread->AcquireSurface();
      mNativeImageQueue->SetSize(static_cast<uint32_t>(targetSize.x), static_cast<uint32_t>(targetSize.y));
      mRenderThread->SetSurfaceSize(Vector2(targetSize.x, targetSize.y));
      mRenderThread->ReleaseSurface();
    }
  }
}

Shader GlView::CreateShader()
{
  std::string fragmentShader = std::string(SHADER_GL_VIEW_FRAG);

  if(mNativeImageQueue)
  {
    mNativeImageQueue->ApplyNativeFragmentShader(fragmentShader);
  }

  return Shader::New(SHADER_GL_VIEW_VERT, fragmentShader, Shader::Hint::NONE, "GL_VIEW");
}

void GlView::OnControlInheritedVisibilityChanged(Dali::Actor actor, bool visible)
{
  Actor self = Self();
  if(mRenderThread)
  {
    if(visible && DevelWindow::Get(self).IsVisible())
    {
      mRenderThread->Resume();
    }
    else
    {
      mRenderThread->Pause();
    }
  }
}

void GlView::OnSceneConnection(int depth)
{
  Control::OnSceneConnection(depth);
}

void GlView::OnSceneDisconnection()
{
  Control::OnSceneDisconnection();
}

Dali::Geometry GlView::CreateTexturedQuad()
{
  struct Vertex
  {
    Dali::Vector2 position;
  };

  static const Vertex data[] = {{Dali::Vector2(-0.5f, -0.5f)},
                                {Dali::Vector2(0.5f, -0.5f)},
                                {Dali::Vector2(-0.5f, 0.5f)},
                                {Dali::Vector2(0.5f, 0.5f)}};

  uint32_t numberOfVertices = sizeof(data) / sizeof(Vertex);

  Dali::VertexBuffer  vertexBuffer;
  Dali::Property::Map vertexFormat;
  vertexFormat["aPosition"] = Dali::Property::VECTOR2;

  //Create a vertex buffer for vertex positions and texture coordinates
  vertexBuffer = Dali::VertexBuffer::New(vertexFormat);
  vertexBuffer.SetData(data, numberOfVertices);

  //Create the geometry
  Dali::Geometry geometry = Dali::Geometry::New();
  geometry.AddVertexBuffer(vertexBuffer);
  geometry.SetType(Dali::Geometry::TRIANGLE_STRIP);

  return geometry;
}

void GlView::AddRenderer()
{
  if(!mNativeImageQueue)
  {
    DALI_LOG_ERROR("Target Surface is NULL");
    return;
  }

  Actor    self     = Self();
  Geometry geometry = CreateTexturedQuad();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Texture    nativeTexture = Texture::New(*mNativeImageQueue);
  TextureSet textureSet    = TextureSet::New();
  textureSet.SetTexture(0u, nativeTexture);

  renderer.SetTextures(textureSet);

  self.AddRenderer(renderer);
}

Dali::NativeImageSourceQueue::ColorFormat GlView::GetColorFormat(Dali::Toolkit::GlView::ColorFormat format)
{
  switch(format)
  {
    case Toolkit::GlView::ColorFormat::RGBA8888:
    {
      return Dali::NativeImageSourceQueue::ColorFormat::RGBA8888;
    }

    case Toolkit::GlView::ColorFormat::RGB888:
    default:
    {
      return Dali::NativeImageSourceQueue::ColorFormat::RGBX8888;
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
