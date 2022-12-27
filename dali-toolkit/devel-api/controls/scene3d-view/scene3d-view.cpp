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
#include <dali-toolkit/devel-api/controls/scene3d-view/scene3d-view.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scene3d-view/scene3d-view-impl.h>

namespace Dali
{
namespace Toolkit
{
Scene3dView::Scene3dView()
{
}

Scene3dView::~Scene3dView()
{
}

Scene3dView::Scene3dView(const Scene3dView& handle) = default;

Scene3dView& Scene3dView::operator=(const Scene3dView& handle) = default;

Scene3dView::Scene3dView(Scene3dView&& rhs) = default;

Scene3dView& Scene3dView::operator=(Scene3dView&& rhs) = default;

Scene3dView Scene3dView::New(const std::string& filePath)
{
  return Internal::Scene3dView::New(filePath);
}

Scene3dView Scene3dView::New(const std::string& filePath, const std::string& diffuseTexturePath, const std::string& specularTexturePath, Vector4 scaleFactor)
{
  return Internal::Scene3dView::New(filePath, diffuseTexturePath, specularTexturePath, scaleFactor);
}

Scene3dView::Scene3dView(Internal::Scene3dView& implementation)
: Control(implementation)
{
}

Scene3dView::Scene3dView(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::Scene3dView>(internal);
}

Scene3dView Scene3dView::DownCast(BaseHandle handle)
{
  return Control::DownCast<Scene3dView, Internal::Scene3dView>(handle);
}

uint32_t Scene3dView::GetAnimationCount()
{
  return GetImpl(*this).GetAnimationCount();
}

bool Scene3dView::PlayAnimation(uint32_t index)
{
  return GetImpl(*this).PlayAnimation(index);
}

bool Scene3dView::PlayAnimations()
{
  return GetImpl(*this).PlayAnimations();
}

bool Scene3dView::SetLight(LightType type, Vector3 lightVector, Vector3 lightColor)
{
  return GetImpl(*this).SetLight(type, lightVector, lightColor);
}

CameraActor Scene3dView::GetDefaultCamera()
{
  return GetImpl(*this).GetDefaultCamera();
}

uint32_t Scene3dView::GetCameraCount()
{
  return GetImpl(*this).GetCameraCount();
}

CameraActor Scene3dView::GetCamera(uint32_t cameraIndex)
{
  return GetImpl(*this).GetCamera(cameraIndex);
}

} //namespace Toolkit

} //namespace Dali
