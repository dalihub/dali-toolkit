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
#include <dali-scene3d/internal/model-components/model-node-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-components/model-node-data-impl.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
Dali::Scene3D::ModelNode ModelNode::New()
{
  // Create the implementation, temporarily owned on stack
  IntrusivePtr<ModelNode> nodeImpl = new ModelNode();

  // Pass ownership to handle
  Scene3D::ModelNode handle(*nodeImpl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  nodeImpl->Initialize();

  return handle;
}

ModelNode::ModelNode()
: CustomActorImpl(ActorFlags::DISABLE_SIZE_NEGOTIATION),
  mImpl(new Impl(*this))
{
}

ModelNode::~ModelNode()
{
}

void ModelNode::Initialize()
{
  OnInitialize();
}

void ModelNode::OnInitialize()
{
}

void ModelNode::OnSceneConnection(int depth)
{
  mImpl->OnSceneConnection(depth);
}

void ModelNode::OnSceneDisconnection()
{
  mImpl->OnSceneDisconnection();
}

void ModelNode::OnChildAdd(Actor& child)
{
}

void ModelNode::OnChildRemove(Actor& child)
{
}

void ModelNode::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
}

void ModelNode::OnSizeSet(const Vector3& targetSize)
{
}

void ModelNode::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // @todo size negotiate background to new size, animate as well?
}

void ModelNode::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
}

void ModelNode::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
}

Vector3 ModelNode::GetNaturalSize()
{
  return Vector3::ZERO;
}

float ModelNode::CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
{
  return 0.0f;
}

float ModelNode::GetHeightForWidth(float width)
{
  return 0.0f;
}

float ModelNode::GetWidthForHeight(float height)
{
  return 0.0f;
}

bool ModelNode::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return false;
}

void ModelNode::OnCalculateRelayoutSize(Dimension::Type dimension)
{
}

void ModelNode::OnLayoutNegotiated(float size, Dimension::Type dimension)
{
}

ModelNode& GetImplementation(Dali::Scene3D::ModelNode& handle)
{
  CustomActorImpl& customInterface = handle.GetImplementation();
  ModelNode& impl = dynamic_cast<Internal::ModelNode&>(customInterface);
  return impl;
}

const ModelNode& GetImplementation(const Dali::Scene3D::ModelNode& handle)
{
  const CustomActorImpl& customInterface = handle.GetImplementation();
  // downcast to control
  const ModelNode& impl = dynamic_cast<const Internal::ModelNode&>(customInterface);
  return impl;
}

// Public Method

uint32_t ModelNode::GetModelPrimitiveCount() const
{
  return mImpl->GetModelPrimitiveCount();
}

void ModelNode::AddModelPrimitive(Dali::Scene3D::ModelPrimitive modelPrimitive)
{
  mImpl->AddModelPrimitive(modelPrimitive);
}

void ModelNode::RemoveModelPrimitive(Dali::Scene3D::ModelPrimitive modelPrimitive)
{
  mImpl->RemoveModelPrimitive(modelPrimitive);
}

void ModelNode::RemoveModelPrimitive(uint32_t index)
{
  mImpl->RemoveModelPrimitive(index);
}

Dali::Scene3D::ModelPrimitive ModelNode::GetModelPrimitive(uint32_t index) const
{
  return mImpl->GetModelPrimitive(index);
}

Scene3D::ModelNode ModelNode::FindChildModelNodeByName(std::string_view nodeName)
{
  Actor childActor = Self().FindChildByName(nodeName);
  return Scene3D::ModelNode::DownCast(childActor);
}

void ModelNode::SetImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float iblScaleFactor, uint32_t specularMipmapLevels)
{
  mImpl->SetImageBasedLightTexture(diffuseTexture, specularTexture, iblScaleFactor, specularMipmapLevels);
}

void ModelNode::SetImageBasedLightScaleFactor(float iblScaleFactor)
{
  mImpl->SetImageBasedLightScaleFactor(iblScaleFactor);
}

void ModelNode::SetBlendShapeData(Scene3D::Loader::BlendShapes::BlendShapeData& data, Scene3D::ModelPrimitive primitive)
{
  mImpl->SetBlendShapeData(data, primitive);
}

void ModelNode::SetBoneMatrix(const Matrix& inverseMatrix, Scene3D::ModelPrimitive primitive, Scene3D::Loader::Index& boneIndex)
{
  mImpl->SetBoneMatrix(inverseMatrix, primitive, boneIndex);
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali
