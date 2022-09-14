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
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/scene-view/scene-view-impl.h>

namespace Dali
{
namespace Scene3D
{
SceneView::SceneView()
{
}

SceneView::SceneView(const SceneView& sceneView) = default;

SceneView::SceneView(SceneView&& rhs) = default;

SceneView& SceneView::operator=(const SceneView& sceneView) = default;

SceneView& SceneView::operator=(SceneView&& rhs) = default;

SceneView::~SceneView()
{
}

SceneView SceneView::New()
{
  return Internal::SceneView::New();
}

SceneView SceneView::DownCast(BaseHandle handle)
{
  return Control::DownCast<SceneView, Internal::SceneView>(handle);
}

SceneView::SceneView(Internal::SceneView& implementation)
: Control(implementation)
{
}

SceneView::SceneView(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::SceneView>(internal);
}

void SceneView::AddCamera(Dali::CameraActor camera)
{
  GetImpl(*this).AddCamera(camera);
}

void SceneView::RemoveCamera(CameraActor camera)
{
  GetImpl(*this).RemoveCamera(camera);
}

uint32_t SceneView::GetCameraCount() const
{
  return GetImpl(*this).GetCameraCount();
}

CameraActor SceneView::GetSelectedCamera() const
{
  return GetImpl(*this).GetSelectedCamera();
}

CameraActor SceneView::GetCamera(uint32_t index) const
{
  return GetImpl(*this).GetCamera(index);
}

CameraActor SceneView::GetCamera(const std::string& name) const
{
  return GetImpl(*this).GetCamera(name);
}

void SceneView::SelectCamera(uint32_t index)
{
  GetImpl(*this).SelectCamera(index);
}

void SceneView::SelectCamera(const std::string& name)
{
  GetImpl(*this).SelectCamera(name);
}

void SceneView::SetImageBasedLightSource(const std::string& diffuse, const std::string& specular, float scaleFactor)
{
  GetImpl(*this).SetImageBasedLightSource(diffuse, specular, scaleFactor);
}

void SceneView::SetImageBasedLightScaleFactor(float scaleFactor)
{
  GetImpl(*this).SetImageBasedLightScaleFactor(scaleFactor);
}

float SceneView::GetImageBasedLightScaleFactor() const
{
  return GetImpl(*this).GetImageBasedLightScaleFactor();
}

void SceneView::UseFramebuffer(bool useFramebuffer)
{
  GetImpl(*this).UseFramebuffer(useFramebuffer);
}

bool SceneView::IsUsingFramebuffer() const
{
  return GetImpl(*this).IsUsingFramebuffer();
}

} // namespace Scene3D

} // namespace Dali
