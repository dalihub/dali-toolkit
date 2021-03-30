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
#include <dali-toolkit/devel-api/controls/canvas-view/canvas-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/canvas-view/canvas-view-impl.h>

namespace Dali
{
namespace Toolkit
{
CanvasView::CanvasView()
{
}

CanvasView::CanvasView(const CanvasView& canvasView)
: Control(canvasView)
{
}

CanvasView::CanvasView(CanvasView&& rhs) = default;

CanvasView& CanvasView::operator=(const CanvasView& rhs)
{
  if(&rhs != this)
  {
    Control::operator=(rhs);
  }
  return *this;
}

CanvasView& CanvasView::operator=(CanvasView&& rhs) = default;

CanvasView::~CanvasView()
{
}

CanvasView CanvasView::New(const Vector2& viewBox)
{
  CanvasView canvasView = Internal::CanvasView::New(viewBox);
  return canvasView;
}

CanvasView CanvasView::DownCast(BaseHandle handle)
{
  return Control::DownCast<CanvasView, Internal::CanvasView>(handle);
}

void CanvasView::AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  Dali::Toolkit::GetImpl(*this).AddDrawable(drawable);
}

CanvasView::CanvasView(Internal::CanvasView& implementation)
: Control(implementation)
{
}

CanvasView::CanvasView(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::CanvasView>(internal);
}

} // namespace Toolkit

} // namespace Dali
