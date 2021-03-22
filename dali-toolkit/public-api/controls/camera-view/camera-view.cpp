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
#include <dali-toolkit/public-api/controls/camera-view/camera-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/camera-view/camera-view-impl.h>

namespace Dali
{
namespace Toolkit
{
CameraView::CameraView()
{
}

CameraView::CameraView(const CameraView& cameraView) = default;

CameraView::CameraView(CameraView&& rhs) = default;

CameraView& CameraView::operator=(const CameraView& cameraView) = default;

CameraView& CameraView::operator=(CameraView&& rhs) = default;

CameraView::~CameraView()
{
}

CameraView CameraView::New(Any handle, DisplayType type)
{
  return Internal::CameraView::New(handle, type);
}

CameraView CameraView::DownCast(BaseHandle handle)
{
  return Control::DownCast<CameraView, Internal::CameraView>(handle);
}

void CameraView::Update()
{
  Dali::Toolkit::GetImpl(*this).Update();
}

CameraView::CameraView(Internal::CameraView& implementation)
: Control(implementation)
{
}

CameraView::CameraView(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::CameraView>(internal);
}

} // namespace Toolkit

} // namespace Dali