/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/image-view/image-view-impl.h>

namespace Dali
{
namespace Toolkit
{
ImageView::ImageView()
{
}

ImageView::ImageView(const ImageView& imageView) = default;

ImageView::ImageView(ImageView&& rhs) noexcept = default;

ImageView& ImageView::operator=(const ImageView& imageView) = default;

ImageView& ImageView::operator=(ImageView&& rhs) noexcept = default;

ImageView::~ImageView()
{
}

ImageView ImageView::New()
{
  return Toolkit::Internal::ImageView::New();
}

ImageView ImageView::New(const std::string& url)
{
  ImageView imageView = Internal::ImageView::New();
  imageView.SetImage(url, ImageDimensions());
  return imageView;
}

ImageView ImageView::New(const std::string& url, ImageDimensions size)
{
  ImageView imageView = Internal::ImageView::New();
  imageView.SetImage(url, size);
  return imageView;
}

ImageView ImageView::New(ControlBehaviour additionalBehaviour)
{
  return Toolkit::Internal::ImageView::New(static_cast<Toolkit::Internal::Control::ControlBehaviour>(additionalBehaviour));
}

ImageView ImageView::New(ControlBehaviour additionalBehaviour, const std::string& url)
{
  ImageView imageView = Internal::ImageView::New(static_cast<Toolkit::Internal::Control::ControlBehaviour>(additionalBehaviour));
  imageView.SetImage(url, ImageDimensions());
  return imageView;
}

ImageView ImageView::New(ControlBehaviour additionalBehaviour, const std::string& url, ImageDimensions size)
{
  ImageView imageView = Internal::ImageView::New(static_cast<Toolkit::Internal::Control::ControlBehaviour>(additionalBehaviour));
  imageView.SetImage(url, size);
  return imageView;
}

ImageView ImageView::DownCast(BaseHandle handle)
{
  return Control::DownCast<ImageView, Internal::ImageView>(handle);
}

void ImageView::SetImage(const std::string& url)
{
  Dali::Toolkit::GetImpl(*this).SetImage(url, ImageDimensions());
}

void ImageView::SetImage(const std::string& url, ImageDimensions size)
{
  Dali::Toolkit::GetImpl(*this).SetImage(url, size);
}

ImageView::ImageView(Internal::ImageView& implementation)
: Control(implementation)
{
}

ImageView::ImageView(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::ImageView>(internal);
}

} // namespace Toolkit

} // namespace Dali
