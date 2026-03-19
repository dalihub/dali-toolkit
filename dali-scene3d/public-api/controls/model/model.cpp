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
#include <dali-scene3d/public-api/controls/model/model.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/model/model-impl.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

using Dali::Integration::ToStdString;
using Dali::Integration::ToStdStringView;

namespace Dali::Scene3D
{
Model::Model() = default;

Model::Model(const Model& model) = default;

Model::Model(Model&& rhs) noexcept = default;

Model& Model::operator=(const Model& model) = default;

Model& Model::operator=(Model&& rhs) noexcept = default;

Model::~Model() = default;

Model Model::New(const Dali::String& modelUrl, const Dali::String& resourceDirectoryUrl)
{
  return Internal::Model::New(ToStdString(modelUrl), ToStdString(resourceDirectoryUrl));
}

Model Model::DownCast(BaseHandle handle)
{
  return Control::DownCast<Model, Internal::Model>(handle);
}

Model::Model(Internal::Model& implementation)
: Control(implementation)
{
}

Model::Model(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  VerifyCustomActorPointer<Internal::Model>(internal);
}

const ModelNode Model::GetModelRoot() const
{
  return GetImpl(*this).GetModelRoot();
}

void Model::AddModelNode(ModelNode modelNode)
{
  return GetImpl(*this).AddModelNode(modelNode);
}

void Model::RemoveModelNode(ModelNode modelNode)
{
  return GetImpl(*this).RemoveModelNode(modelNode);
}

void Model::SetChildrenSensitive(bool enable)
{
  GetImpl(*this).SetChildrenSensitive(enable);
}

bool Model::GetChildrenSensitive() const
{
  return GetImpl(*this).GetChildrenSensitive();
}

void Model::SetChildrenFocusable(bool enable)
{
  GetImpl(*this).SetChildrenFocusable(enable);
}

bool Model::GetChildrenFocusable() const
{
  return GetImpl(*this).GetChildrenFocusable();
}

void Model::SetImageBasedLightSource(const Dali::String& diffuseUrl, const Dali::String& specularUrl, float scaleFactor)
{
  GetImpl(*this).SetImageBasedLightSource(ToStdString(diffuseUrl), ToStdString(specularUrl), scaleFactor);
}

void Model::SetImageBasedLightScaleFactor(float scaleFactor)
{
  GetImpl(*this).SetImageBasedLightScaleFactor(scaleFactor);
}

float Model::GetImageBasedLightScaleFactor() const
{
  return GetImpl(*this).GetImageBasedLightScaleFactor();
}

uint32_t Model::GetAnimationCount() const
{
  return GetImpl(*this).GetAnimationCount();
}

Dali::Animation Model::GetAnimation(uint32_t index) const
{
  return GetImpl(*this).GetAnimation(index);
}

Dali::Animation Model::GetAnimation(const Dali::String& name) const
{
  return GetImpl(*this).GetAnimation(ToStdString(name));
}

uint32_t Model::GetCameraCount() const
{
  return GetImpl(*this).GetCameraCount();
}

Dali::CameraActor Model::GenerateCamera(uint32_t index) const
{
  return GetImpl(*this).GenerateCamera(index);
}

bool Model::ApplyCamera(uint32_t index, Dali::CameraActor camera) const
{
  return GetImpl(*this).ApplyCamera(index, camera);
}

ModelNode Model::FindChildModelNodeByName(Dali::StringView nodeName)
{
  return GetImpl(*this).FindChildModelNodeByName(ToStdStringView(nodeName));
}

void Model::RetrieveBlendShapeNames(std::vector<Dali::String>& blendShapeNames) const
{
  std::vector<std::string> stdNames;
  GetImpl(*this).RetrieveBlendShapeNames(stdNames);
  blendShapeNames.reserve(stdNames.size());
  for(auto& name : stdNames)
  {
    blendShapeNames.emplace_back(name.c_str());
  }
}

void Model::RetrieveModelNodesByBlendShapeName(Dali::StringView blendShapeName, std::vector<ModelNode>& modelNodes) const
{
  GetImpl(*this).RetrieveModelNodesByBlendShapeName(ToStdStringView(blendShapeName), modelNodes);
}

Dali::Animation Model::GenerateMotionDataAnimation(MotionData motionData)
{
  return GetImpl(*this).GenerateMotionDataAnimation(motionData);
}

void Model::SetMotionData(MotionData motionData)
{
  GetImpl(*this).SetMotionData(motionData);
}

void Model::CastShadow(bool castShadow)
{
  GetImpl(*this).CastShadow(castShadow);
}

bool Model::IsShadowCasting() const
{
  return GetImpl(*this).IsShadowCasting();
}

void Model::ReceiveShadow(bool receiveShadow)
{
  GetImpl(*this).ReceiveShadow(receiveShadow);
}

bool Model::IsShadowReceiving() const
{
  return GetImpl(*this).IsShadowReceiving();
}

Model::MeshHitSignalType& Model::MeshHitSignal()
{
  return GetImpl(*this).MeshHitSignal();
}

Model::LoadCompletedSignalType& Model::LoadCompletedSignal()
{
  return GetImpl(*this).LoadCompletedSignal();
}

Model::ResourceStatus Model::GetModelResourceStatus() const
{
  return GetImpl(*this).GetModelResourceStatus();
}

} // namespace Dali::Scene3D
