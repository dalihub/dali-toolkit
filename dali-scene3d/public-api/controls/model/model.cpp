/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/model/model-impl.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

namespace Dali
{
namespace Scene3D
{
Model::Model()
{
}

Model::Model(const Model& model) = default;

Model::Model(Model&& rhs) = default;

Model& Model::operator=(const Model& model) = default;

Model& Model::operator=(Model&& rhs) = default;

Model::~Model()
{
}

Model Model::New(const std::string& modelUrl, const std::string& resourceDirectoryUrl)
{
  return Internal::Model::New(modelUrl, resourceDirectoryUrl);
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

void Model::SetImageBasedLightSource(const std::string& diffuseUrl, const std::string& specularUrl, float scaleFactor)
{
  GetImpl(*this).SetImageBasedLightSource(diffuseUrl, specularUrl, scaleFactor);
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

Dali::Animation Model::GetAnimation(const std::string& name) const
{
  return GetImpl(*this).GetAnimation(name);
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

ModelNode Model::FindChildModelNodeByName(std::string_view nodeName)
{
  return GetImpl(*this).FindChildModelNodeByName(nodeName);
}

void Model::RetrieveBlendShapeNames(std::vector<std::string>& blendShapeNames) const
{
  GetImpl(*this).RetrieveBlendShapeNames(blendShapeNames);
}

void Model::RetrieveModelNodesByBlendShapeName(std::string_view blendShapeName, std::vector<ModelNode>& modelNodes) const
{
  GetImpl(*this).RetrieveModelNodesByBlendShapeName(blendShapeName, modelNodes);
}

Dali::Animation Model::GenerateMotionDataAnimation(MotionData motionData)
{
  return GetImpl(*this).GenerateMotionDataAnimation(motionData);
}

void Model::SetMotionData(MotionData motionData)
{
  GetImpl(*this).SetMotionData(motionData);
}

} // namespace Scene3D

} // namespace Dali
