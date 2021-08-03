/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "canvas-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/internal/controls/canvas-view/canvas-view-rasterize-thread.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  return BaseHandle();
}
// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::CanvasView, Toolkit::Control, Create);
DALI_PROPERTY_REGISTRATION(Toolkit, CanvasView, "viewBox", VECTOR2, VIEW_BOX)
DALI_TYPE_REGISTRATION_END()
} // anonymous namespace

using namespace Dali;

CanvasView::CanvasView(const Vector2& viewBox)
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mCanvasRenderer(CanvasRenderer::New(viewBox)),
  mTexture(),
  mTextureSet(),
  mSize(viewBox),
  mCanvasViewRasterizeThread(nullptr)
{
}

CanvasView::~CanvasView()
{
  if(mCanvasViewRasterizeThread)
  {
    mCanvasViewRasterizeThread->RemoveTask(this);

    CanvasViewRasterizeThread::TerminateThread(mCanvasViewRasterizeThread);
  }

  if(Adaptor::IsAvailable())
  {
    Adaptor::Get().UnregisterProcessor(*this);
  }
}

Toolkit::CanvasView CanvasView::New(const Vector2& viewBox)
{
  CanvasView* impl = new CanvasView(viewBox);

  Toolkit::CanvasView handle = Toolkit::CanvasView(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

/////////////////////////////////////////////////////////////

void CanvasView::OnInitialize()
{
  // CanvasView can relayout in the OnImageReady, alternative to a signal would be to have a upcall from the Control to CanvasView
  Dali::Toolkit::Control handle(GetOwner());

  DevelControl::SetAccessibilityConstructor(Self(), [](Dali::Actor actor) {
    return std::unique_ptr<Dali::Accessibility::Accessible>(
      new DevelControl::AccessibleImpl(actor, Dali::Accessibility::Role::IMAGE));
  });

  Adaptor::Get().RegisterProcessor(*this);
}

void CanvasView::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  if(!mCanvasRenderer ||
     !mCanvasRenderer.SetSize(size))
  {
    return;
  }
  mSize = size;
}

void CanvasView::OnSizeSet(const Vector3& targetSize)
{
  Control::OnSizeSet(targetSize);

  if(!mCanvasRenderer ||
     !mCanvasRenderer.SetSize(Vector2(targetSize)))
  {
    return;
  }
  mSize.width  = targetSize.width;
  mSize.height = targetSize.height;
}

void CanvasView::SetProperty(BaseObject* object, Property::Index propertyIndex, const Property::Value& value)
{
  Toolkit::CanvasView canvasView = Toolkit::CanvasView::DownCast(Dali::BaseHandle(object));
  if(canvasView)
  {
    CanvasView& canvasViewImpl(GetImpl(canvasView));

    switch(propertyIndex)
    {
      case Toolkit::CanvasView::Property::VIEW_BOX:
      {
        Vector2 valueVector2;
        if(value.Get(valueVector2))
        {
          canvasViewImpl.SetViewBox(valueVector2);
        }
        break;
      }
    }
  }
}

Property::Value CanvasView::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;

  Toolkit::CanvasView canvasView = Toolkit::CanvasView::DownCast(Dali::BaseHandle(object));

  if(canvasView)
  {
    CanvasView& canvasViewImpl(GetImpl(canvasView));

    switch(propertyIndex)
    {
      case Toolkit::CanvasView::Property::VIEW_BOX:
      {
        value = canvasViewImpl.GetViewBox();
        break;
      }
    }
  }
  return value;
}

void CanvasView::Process(bool postProcessor)
{
  if(mCanvasRenderer && mCanvasRenderer.IsCanvasChanged() && mSize.width > 0 && mSize.height > 0)
  {
    AddRasterizationTask();
  }
}

void CanvasView::AddRasterizationTask()
{
  CanvasRendererRasterizingTaskPtr newTask = new CanvasRendererRasterizingTask(this, mCanvasRenderer);

  if(!mCanvasViewRasterizeThread)
  {
    mCanvasViewRasterizeThread = new CanvasViewRasterizeThread();
    mCanvasViewRasterizeThread->RasterizationCompletedSignal().Connect(this, &CanvasView::ApplyRasterizedImage);
    mCanvasViewRasterizeThread->Start();
  }

  if(mCanvasRenderer.Commit())
  {
    mCanvasViewRasterizeThread->AddTask(newTask);
  }
}

void CanvasView::ApplyRasterizedImage(PixelData rasterizedPixelData)
{
  if(rasterizedPixelData)
  {
    auto rasterizedPixelDataWidth  = rasterizedPixelData.GetWidth();
    auto rasterizedPixelDataHeight = rasterizedPixelData.GetHeight();

    if(rasterizedPixelDataWidth > 0 && rasterizedPixelDataHeight > 0)
    {
      if(!mTexture || mTexture.GetWidth() != rasterizedPixelDataWidth || mTexture.GetHeight() != rasterizedPixelDataHeight)
      {
        mTexture = Texture::New(TextureType::TEXTURE_2D, Dali::Pixel::RGBA8888, rasterizedPixelDataWidth, rasterizedPixelDataHeight);
        mTexture.Upload(rasterizedPixelData);

        if(!mTextureSet)
        {
          mTextureSet       = TextureSet::New();
          Geometry geometry = VisualFactoryCache::CreateQuadGeometry();
          Shader   shader   = Shader::New(SHADER_CANVAS_VIEW_VERT, SHADER_CANVAS_VIEW_FRAG);
          Renderer renderer = Renderer::New(geometry, shader);
          renderer.SetTextures(mTextureSet);
          renderer.SetProperty(Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA, true);

          Actor actor = Self();
          if(actor)
          {
            actor.AddRenderer(renderer);
          }
        }
        mTextureSet.SetTexture(0, mTexture);
      }
      else
      {
        //Update texture
        mTexture.Upload(rasterizedPixelData);
      }
    }
  }

  //If there are accumulated changes to CanvasRenderer during Rasterize, Rasterize once again.
  if(mCanvasRenderer && mCanvasRenderer.IsCanvasChanged())
  {
    AddRasterizationTask();
  }
}

bool CanvasView::AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  if(mCanvasRenderer && mCanvasRenderer.AddDrawable(drawable))
  {
    return true;
  }
  return false;
}

bool CanvasView::RemoveDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  if(mCanvasRenderer && mCanvasRenderer.RemoveDrawable(drawable))
  {
    return true;
  }
  return false;
}

bool CanvasView::RemoveAllDrawables()
{
  if(mCanvasRenderer && mCanvasRenderer.RemoveAllDrawables())
  {
    return true;
  }
  return false;
}

bool CanvasView::SetViewBox(const Vector2& viewBox)
{
  if(mCanvasRenderer && mCanvasRenderer.SetViewBox(viewBox))
  {
    return true;
  }
  return false;
}

const Vector2& CanvasView::GetViewBox()
{
  if(mCanvasRenderer)
  {
    return mCanvasRenderer.GetViewBox();
  }
  return Vector2::ZERO;
}
} // namespace Internal
} // namespace Toolkit
} // namespace Dali
