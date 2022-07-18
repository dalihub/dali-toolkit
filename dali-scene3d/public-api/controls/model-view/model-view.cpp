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
#include <dali-scene3d/public-api/controls/model-view/model-view.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/model-view/model-view-impl.h>

namespace Dali
{
namespace Scene3D
{
ModelView::ModelView()
{
}

ModelView::ModelView(const ModelView& modelView) = default;

ModelView::ModelView(ModelView&& rhs) = default;

ModelView& ModelView::operator=(const ModelView& modelView) = default;

ModelView& ModelView::operator=(ModelView&& rhs) = default;

ModelView::~ModelView()
{
}

ModelView ModelView::New(const std::string& modelPath, const std::string& resourcePath)
{
  return Internal::ModelView::New(modelPath, resourcePath);
}

ModelView ModelView::DownCast(BaseHandle handle)
{
  return Control::DownCast<ModelView, Internal::ModelView>(handle);
}

ModelView::ModelView(Internal::ModelView& implementation)
: Control(implementation)
{
}

ModelView::ModelView(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::ModelView>(internal);
}

const Actor ModelView::GetModelRoot()
{
  return GetImpl(*this).GetModelRoot();
}

void ModelView::FitSize(bool fit)
{
  GetImpl(*this).FitSize(fit);
}

void ModelView::FitCenter(bool fit)
{
  GetImpl(*this).FitCenter(fit);
}

void ModelView::SetImageBasedLightSource(const std::string& diffuse, const std::string& specular, float scaleFactor)
{
  GetImpl(*this).SetImageBasedLightSource(diffuse, specular, scaleFactor);
}

uint32_t ModelView::GetAnimationCount()
{
  return GetImpl(*this).GetAnimationCount();
}

Dali::Animation ModelView::GetAnimation(uint32_t index)
{
  return GetImpl(*this).GetAnimation(index);
}

Dali::Animation ModelView::GetAnimation(const std::string& name)
{
  return GetImpl(*this).GetAnimation(name);
}

} // namespace Scene3D

} // namespace Dali
