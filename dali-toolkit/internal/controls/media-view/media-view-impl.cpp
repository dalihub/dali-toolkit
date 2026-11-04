/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/controls/media-view/media-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/property-value-devel.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-depth-index-ranges.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/controls/control/control-internal.h>
#include <dali-toolkit/internal/visuals/streamed-image/streamed-image-visual.h>
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace Internal
{
namespace
{
/**
 * @brief Converts backend presentation metadata to a streamed image frame.
 * @param[in] presentation The current frame and its displayed geometry
 * @return The frame with crop and rotation applied by StreamedImageVisual
 */
StreamedImageVisual::Frame MakeStreamedFrame(const Dali::Integration::MediaViewPresentation& presentation)
{
  StreamedImageVisual::Frame frame;
  frame.texture        = presentation.texture;
  frame.contentSize    = presentation.contentSize;
  frame.crop           = presentation.sourceCrop;
  frame.horizontalFlip = presentation.horizontalFlip;
  frame.verticalFlip   = presentation.verticalFlip;

  using Transform = Dali::Integration::MediaFrameTransform;
  using Rotation  = StreamedImageVisual::Rotation;
  switch(presentation.sourceTransform)
  {
    case Transform::IDENTITY:
    {
      break;
    }
    case Transform::ROTATE_90:
    {
      frame.rotation = Rotation::DEGREE_90;
      break;
    }
    case Transform::ROTATE_180:
    {
      frame.rotation = Rotation::DEGREE_180;
      break;
    }
    case Transform::ROTATE_270:
    {
      frame.rotation = Rotation::DEGREE_270;
      break;
    }
    case Transform::HORIZONTAL_FLIP:
    {
      frame.horizontalFlip = !frame.horizontalFlip;
      break;
    }
    case Transform::VERTICAL_FLIP:
    {
      frame.verticalFlip = !frame.verticalFlip;
      break;
    }
  }

  // The backend reports the cropped, rotated size, whereas StreamedImageVisual
  // expects the uncropped, unrotated content size. Undo both operations rather
  // than using texture dimensions, which may include producer padding.
  if(frame.rotation == Rotation::DEGREE_90 || frame.rotation == Rotation::DEGREE_270)
  {
    std::swap(frame.contentSize.width, frame.contentSize.height);
  }
  frame.contentSize.width /= frame.crop.width;
  frame.contentSize.height /= frame.crop.height;
  return frame;
}

BaseHandle Create()
{
  return Toolkit::MediaView::New();
}

DALI_TYPE_REGISTRATION_BEGIN(Toolkit::MediaView, Toolkit::Control, Create);
DALI_TYPE_REGISTRATION_END()

} // namespace

MediaView::MediaView()
: ControlImpl(CONTROL_BEHAVIOUR_DEFAULT)
{
}

MediaView::~MediaView()
{
  if(mBackend)
  {
    mBackend->PresentationChangedSignal().Disconnect(this, &MediaView::OnPresentationChanged);
    mBackend->ClearSource();
  }
  // The base class owns registered visuals during destruction.
  mVisual.Reset();
}

Toolkit::MediaView MediaView::New()
{
  auto*              impl = new MediaView;
  Toolkit::MediaView view(*impl);
  impl->Initialize();
  return view;
}

void MediaView::OnInitialize()
{
  mContentVisualIndex = Self().RegisterProperty("mediaViewContent", "mediaViewContent", Property::AccessMode::READ_WRITE);
  mBackend            = Dali::MakeUnique<Dali::Integration::MediaViewBackend>();
  mBackend->PresentationChangedSignal().Connect(this, &MediaView::OnPresentationChanged);
}

void MediaView::OnPresentationChanged()
{
  const auto presentation = mBackend->GetPresentation();
  if(!presentation.texture)
  {
    ClearVisual();
    RelayoutRequest();
    return;
  }
  const auto frame = MakeStreamedFrame(presentation);
  if(!mVisual)
  {
    auto  factoryHandle = Toolkit::VisualFactory::Get();
    auto& factory       = Toolkit::GetImplementation(factoryHandle);
    auto  visual        = StreamedImageVisual::New(factory.GetFactoryCache(), frame);
    mVisual             = Toolkit::Visual::Base(visual.Get());
    DevelControl::RegisterVisual(*this, mContentVisualIndex, mVisual, DepthIndex::CONTENT);
    Control::Get(*this).EnableCornerPropertiesOverridden(mVisual, true);
  }
  else
  {
    auto& visual = static_cast<StreamedImageVisual&>(Toolkit::GetImplementation(mVisual));
    visual.SetFrame(frame);
  }
  RelayoutRequest();
}

Vector3 MediaView::GetNaturalSize()
{
  if(!mVisual)
  {
    return ControlImpl::GetNaturalSize();
  }
  Vector2 size;
  mVisual.GetNaturalSize(size);
  Extents padding;
  GetExtents(Self().GetProperty(Toolkit::Control::Property::PADDING), padding);
  size += Vector2(padding.start + padding.end, padding.top + padding.bottom);
  return Vector3(size);
}

float MediaView::GetHeightForWidth(float width)
{
  Extents padding;
  GetExtents(Self().GetProperty(Toolkit::Control::Property::PADDING), padding);
  if(mVisual)
  {
    const float contentWidth = std::max(0.0f, width - padding.start - padding.end);
    return mVisual.GetHeightForWidth(contentWidth) + padding.top + padding.bottom;
  }
  return ControlImpl::GetHeightForWidth(width);
}

float MediaView::GetWidthForHeight(float height)
{
  Extents padding;
  GetExtents(Self().GetProperty(Toolkit::Control::Property::PADDING), padding);
  if(mVisual)
  {
    const float contentHeight = std::max(0.0f, height - padding.top - padding.bottom);
    return mVisual.GetWidthForHeight(contentHeight) + padding.start + padding.end;
  }
  return ControlImpl::GetWidthForHeight(height);
}

void MediaView::ClearVisual()
{
  if(mVisual)
  {
    DevelControl::UnregisterVisual(*this, mContentVisualIndex);
    mVisual.Reset();
  }
}

void MediaView::OnSceneConnection(int depth)
{
  ControlImpl::OnSceneConnection(depth);
  mBackend->SetSceneConnected(true);
}

void MediaView::OnSceneDisconnection()
{
  mBackend->SetSceneConnected(false);
  ControlImpl::OnSceneDisconnection();
}
} // namespace Internal
} // namespace Toolkit
} // namespace DALI_NAMESPACE
