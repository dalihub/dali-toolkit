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
 *
 */

// CLASS HEADER
#include "canvas-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
static constexpr std::string_view CANVAS_VISUAL_INDEX_PROPERTY_NAME("canvasVisualIndexProperty");

BaseHandle Create()
{
  return BaseHandle();
}
// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Toolkit::CanvasView, Toolkit::Control, Create);
DALI_PROPERTY_REGISTRATION(Toolkit, CanvasView, "viewBox", VECTOR2, VIEW_BOX)
DALI_PROPERTY_REGISTRATION(Toolkit, CanvasView, "synchronousLoading", BOOLEAN, SYNCHRONOUS_LOADING)
DALI_PROPERTY_REGISTRATION(Toolkit, CanvasView, "rasterizationRequestManually", BOOLEAN, RASTERIZATION_REQUEST_MANUALLY)
DALI_TYPE_REGISTRATION_END()

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_VECTOR_ANIMATION_PERFORMANCE_MARKER, false);
} // anonymous namespace

using namespace Dali;

CanvasView::CanvasView(const Vector2& viewBox)
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mCanvasRenderer(CanvasRenderer::New(viewBox)),
  mTexture(),
  mSize(viewBox),
  mCanvasVisualIndex(Property::INVALID_INDEX),
  mIsSynchronous(true),
  mManualRasterization(false),
  mProcessorRegistered(false),
  mLastCommitRasterized(false)
{
  DALI_LOG_DEBUG_INFO("[%p] Created\n", this);
  if(DALI_UNLIKELY(!mCanvasRenderer))
  {
    DALI_LOG_ERROR("CanvasView is not supported!\n");
  }
}

CanvasView::~CanvasView()
{
  if(Adaptor::IsAvailable() && mProcessorRegistered)
  {
    DALI_LOG_DEBUG_INFO("[%p] Destroyed\n", this);
    if(mRasterizingTask)
    {
      Dali::AsyncTaskManager::Get().RemoveTask(mRasterizingTask);
      mRasterizingTask.Reset();
    }
    Adaptor::Get().UnregisterProcessorOnce(*this, true);
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

  Self().SetProperty(DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Accessibility::Role::IMAGE);

  // Request rasterization once at very first time.
  RequestRasterization();
}

void CanvasView::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_CANVAS_VIEW_RELAYOUT");
  if(!(DALI_LIKELY(mCanvasRenderer) && mCanvasRenderer.SetSize(size)))
  {
    return;
  }
  mSize = size;
}

void CanvasView::OnSizeSet(const Vector3& targetSize)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_CANVAS_VIEW_RESIZE");
  Control::OnSizeSet(targetSize);

  if(!(DALI_LIKELY(mCanvasRenderer) && mCanvasRenderer.SetSize(Vector2(targetSize))))
  {
    return;
  }
  mSize.width  = targetSize.width;
  mSize.height = targetSize.height;
}

void CanvasView::OnSceneConnection(int depth)
{
  Dali::Window window = DevelWindow::Get(Self());
  if(DALI_LIKELY(window))
  {
    mPlacementWindow = window;
  }
  Control::OnSceneConnection(depth);
}

void CanvasView::OnSceneDisconnection()
{
  mPlacementWindow.Reset();
  Control::OnSceneDisconnection();
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
      case Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING:
      {
        bool isSynchronous;
        if(value.Get(isSynchronous))
        {
          canvasViewImpl.SetSynchronous(isSynchronous);
        }
        break;
      }
      case Toolkit::CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY:
      {
        bool isRasterizationManually;
        if(value.Get(isRasterizationManually))
        {
          canvasViewImpl.SetRasterizationRequestManually(isRasterizationManually);
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
      case Toolkit::CanvasView::Property::SYNCHRONOUS_LOADING:
      {
        value = canvasViewImpl.IsSynchronous();
        break;
      }
      case Toolkit::CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY:
      {
        value = canvasViewImpl.IsRasterizationRequestManually();
        break;
      }
    }
  }
  return value;
}

void CanvasView::Process(bool postProcessor)
{
  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_CANVAS_VIEW_PROCESS", [&](std::ostringstream& oss)
  { oss << "[s:" << mIsSynchronous << ", l:" << mLastCommitRasterized << ", "
        << "m:" << mManualRasterization << ", s:" << mSize.width << "x" << mSize.height << "]"; });

  bool rasterizeRequired = false;

  mProcessorRegistered = false;

  if(DALI_LIKELY(mCanvasRenderer) && mSize.width > 0 && mSize.height > 0)
  {
    const bool forcibleRasterization = (mIsSynchronous && !mLastCommitRasterized);
    rasterizeRequired                = (forcibleRasterization) || (mCanvasRenderer.IsCanvasChanged());

    if(rasterizeRequired)
    {
      AddRasterizationTask(forcibleRasterization);
    }
  }

  const bool isSynchronousRasterizationFailed = (rasterizeRequired && mIsSynchronous && !mLastCommitRasterized);

  // If we are not doing manual rasterization, register processor once again.
  // TODO : Could we reqest it only if IsCanvasChagned() is true?
  if(DALI_LIKELY(mCanvasRenderer) && (!mManualRasterization || isSynchronousRasterizationFailed))
  {
    RequestRasterization();

    if(isSynchronousRasterizationFailed && DALI_LIKELY(Adaptor::IsAvailable()))
    {
      // To make sure we will process next time.
      Adaptor::Get().RequestProcessEventsOnIdle();
    }
  }
  DALI_TRACE_END(gTraceFilter, "DALI_CANVAS_VIEW_PROCESS");
}

void CanvasView::AddRasterizationTask(bool forceProcess)
{
  DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_CANVAS_VIEW_ADD_RASTERIZE", [&](std::ostringstream& oss)
  { oss << "[f:" << forceProcess << "]"; });
  if(DALI_LIKELY(mCanvasRenderer) && (mCanvasRenderer.Commit() || forceProcess))
  {
    mLastCommitRasterized = false;
    if(mIsSynchronous)
    {
      CanvasRendererRasterizingTaskPtr rasterizingTask = new CanvasRendererRasterizingTask(mCanvasRenderer, MakeCallback(this, &CanvasView::ApplyRasterizedImage));
      rasterizingTask->Process();
      ApplyRasterizedImage(rasterizingTask);
      rasterizingTask.Reset(); // We don't need it anymore.
    }
    else
    {
      if(mRasterizingTask)
      {
        // Cancel previous request task.
        AsyncTaskManager::Get().RemoveTask(mRasterizingTask);
        mRasterizingTask.Reset();
      }

      mRasterizingTask = new CanvasRendererRasterizingTask(mCanvasRenderer, MakeCallback(this, &CanvasView::ApplyRasterizedImage));
      AsyncTaskManager::Get().AddTask(mRasterizingTask);
    }
  }
  DALI_TRACE_END(gTraceFilter, "DALI_CANVAS_VIEW_ADD_RASTERIZE");
}

void CanvasView::ApplyRasterizedImage(CanvasRendererRasterizingTaskPtr task)
{
  mLastCommitRasterized = task->IsRasterized();
  DALI_LOG_DEBUG_INFO("[%p] Rasterized. Success?[%d]\n", this, mLastCommitRasterized);

  if(mLastCommitRasterized)
  {
    Texture rasterizedTexture = task->GetRasterizedTexture();
    if(rasterizedTexture && rasterizedTexture.GetWidth() != 0 && rasterizedTexture.GetHeight() != 0)
    {
      if(mTexture != rasterizedTexture)
      {
        mTexture = rasterizedTexture;

        Toolkit::ImageUrl     imageUrl = Toolkit::ImageUrl::New(rasterizedTexture, true);
        Toolkit::Visual::Base visual   = Toolkit::VisualFactory::Get().CreateVisual(imageUrl.GetUrl(), ImageDimensions());
        if(visual)
        {
          if(mCanvasVisualIndex == Property::INVALID_INDEX)
          {
            mCanvasVisualIndex = Self().RegisterProperty(CANVAS_VISUAL_INDEX_PROPERTY_NAME, Property::Value(std::string(CANVAS_VISUAL_INDEX_PROPERTY_NAME)), Property::AccessMode::READ_WRITE);
          }
          DevelControl::RegisterVisual(*this, mCanvasVisualIndex, visual, Toolkit::DepthIndex::CONTENT);

          Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get(*this);
          controlDataImpl.EnableCornerPropertiesOverridden(visual, true);
        }
      }
      else
      {
        Dali::Window window = mPlacementWindow.GetHandle();
        if(DALI_LIKELY(window))
        {
          window.KeepRendering(0.0f); // Trigger event processing, and keep rendering.
        }
      }
    }
  }

  if(task == mRasterizingTask)
  {
    mRasterizingTask.Reset(); // We don't need it anymore
  }

  // If there are accumulated changes to CanvasRenderer during Rasterize, or previous rasterization failed, Rasterize once again.
  if(!mIsSynchronous && DALI_LIKELY(mCanvasRenderer) && (!mLastCommitRasterized || (!mManualRasterization && mCanvasRenderer.IsCanvasChanged())))
  {
    AddRasterizationTask(!mLastCommitRasterized);
  }
}

bool CanvasView::AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  if(DALI_LIKELY(mCanvasRenderer) && mCanvasRenderer.AddDrawable(drawable))
  {
    return true;
  }
  return false;
}

bool CanvasView::RemoveDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  if(DALI_LIKELY(mCanvasRenderer) && mCanvasRenderer.RemoveDrawable(drawable))
  {
    return true;
  }
  return false;
}

bool CanvasView::RemoveAllDrawables()
{
  if(DALI_LIKELY(mCanvasRenderer) && mCanvasRenderer.RemoveAllDrawables())
  {
    return true;
  }
  return false;
}

void CanvasView::RequestRasterization()
{
  DALI_LOG_DEBUG_INFO("[%p] Rasterize request\n", this);
  if(DALI_LIKELY(mCanvasRenderer) && !mProcessorRegistered && Adaptor::IsAvailable())
  {
    mProcessorRegistered = true;
    Adaptor::Get().RegisterProcessorOnce(*this, true);
  }
}

bool CanvasView::SetViewBox(const Vector2& viewBox)
{
  if(DALI_LIKELY(mCanvasRenderer) && mCanvasRenderer.SetViewBox(viewBox))
  {
    return true;
  }
  return false;
}

const Vector2& CanvasView::GetViewBox()
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    return mCanvasRenderer.GetViewBox();
  }
  return Vector2::ZERO;
}

void CanvasView::SetSynchronous(const bool isSynchronous)
{
  mIsSynchronous = isSynchronous;
}

const bool CanvasView::IsSynchronous() const
{
  return mIsSynchronous;
}

void CanvasView::SetRasterizationRequestManually(const bool isRasterizationManually)
{
  if(mManualRasterization != isRasterizationManually)
  {
    mManualRasterization = isRasterizationManually;
    if(!mManualRasterization)
    {
      RequestRasterization();

      if(DALI_LIKELY(Adaptor::IsAvailable()))
      {
        // To make sure we will process next time.
        Adaptor::Get().RequestProcessEventsOnIdle();
      }
    }
  }
}

const bool CanvasView::IsRasterizationRequestManually() const
{
  return mManualRasterization;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
