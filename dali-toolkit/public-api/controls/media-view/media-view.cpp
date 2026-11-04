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
#include <dali-toolkit/public-api/controls/media-view/media-view.h>
// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/media-view/media-view-impl.h>
namespace DALI_NAMESPACE
{
namespace Toolkit
{
namespace MediaViewAccess
{
Internal::MediaView& GetImplementation(MediaView& view)
{
  DALI_ASSERT_ALWAYS(view && "MediaView handle is empty");
  return static_cast<Internal::MediaView&>(view.GetImplementation());
}

const Internal::MediaView& GetImplementation(const MediaView& view)
{
  DALI_ASSERT_ALWAYS(view && "MediaView handle is empty");
  return static_cast<const Internal::MediaView&>(view.GetImplementation());
}
} //namespace MediaViewAccess

MediaView::MediaView() = default;

MediaView::~MediaView() = default;

MediaView::MediaView(const MediaView&) = default;

MediaView::MediaView(MediaView&&) noexcept = default;

MediaView& MediaView::operator=(const MediaView&) = default;

MediaView& MediaView::operator=(MediaView&&) noexcept = default;

MediaView::MediaView(Internal::MediaView& impl)
: Control(impl)
{
}

MediaView::MediaView(Dali::Internal::CustomActor* actor)
: Control(actor)
{
}

MediaView MediaView::New()
{
  return Internal::MediaView::New();
}

MediaView MediaView::New(Dali::Any source)
{
  auto view = New();
  return view.SetSource(source) ? view : MediaView();
}

MediaView MediaView::DownCast(BaseHandle handle)
{
  return Control::DownCast<MediaView, Internal::MediaView>(handle);
}

bool MediaView::SetSource(Dali::Any nativeSource)
{
  return MediaViewAccess::GetImplementation(*this).Backend().SetSource(nativeSource);
}

Dali::Any MediaView::GetSource() const
{
  return MediaViewAccess::GetImplementation(*this).Backend().GetSource();
}

void MediaView::ClearSource()
{
  MediaViewAccess::GetImplementation(*this).Backend().ClearSource();
}

bool MediaView::SetSourceCrop(Dali::Rect<float> crop)
{
  return MediaViewAccess::GetImplementation(*this).Backend().SetSourceCrop(crop);
}

Dali::Rect<float> MediaView::GetSourceCrop() const
{
  return MediaViewAccess::GetImplementation(*this).Backend().GetSourceCrop();
}

void MediaView::SetHorizontalFlipEnabled(bool enabled)
{
  MediaViewAccess::GetImplementation(*this).Backend().SetHorizontalFlipEnabled(enabled);
}

bool MediaView::IsHorizontalFlipEnabled() const
{
  return MediaViewAccess::GetImplementation(*this).Backend().IsHorizontalFlipEnabled();
}

void MediaView::SetVerticalFlipEnabled(bool enabled)
{
  MediaViewAccess::GetImplementation(*this).Backend().SetVerticalFlipEnabled(enabled);
}

bool MediaView::IsVerticalFlipEnabled() const
{
  return MediaViewAccess::GetImplementation(*this).Backend().IsVerticalFlipEnabled();
}
} // namespace Toolkit
} // namespace DALI_NAMESPACE
