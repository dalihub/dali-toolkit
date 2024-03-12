/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/public-api/model-components/model-node.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/custom-actor.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-components/model-node-impl.h>

namespace Dali
{
namespace Scene3D
{
ModelNode ModelNode::New()
{
  return Scene3D::Internal::ModelNode::New();
}

ModelNode::ModelNode()
{
}

ModelNode::ModelNode(const ModelNode& modelNode) = default;

ModelNode::ModelNode(ModelNode&& rhs) noexcept = default;

ModelNode::~ModelNode()
{
}

ModelNode& ModelNode::operator=(const ModelNode& handle) = default;

ModelNode& ModelNode::operator=(ModelNode&& rhs) noexcept = default;

ModelNode ModelNode::DownCast(BaseHandle handle)
{
  ModelNode result;

  CustomActor custom = Dali::CustomActor::DownCast(handle);
  if(custom)
  {
    CustomActorImpl& customImpl = custom.GetImplementation();

    Internal::ModelNode* impl = dynamic_cast<Internal::ModelNode*>(&customImpl);

    if(impl)
    {
      result = ModelNode(customImpl.GetOwner());
    }
  }

  return result;
}

ModelNode::ModelNode(Internal::ModelNode& implementation)
: Control(implementation)
{
}

ModelNode::ModelNode(Dali::Internal::CustomActor* internal)
: Control(internal)
{
  // Can have a NULL pointer so we only need to check if the internal implementation is our class
  // when there is a value.
  if(internal)
  {
    DALI_ASSERT_DEBUG(dynamic_cast<Internal::ModelNode*>(&CustomActor(internal).GetImplementation()));
  }
}

// Public Method

uint32_t ModelNode::GetModelPrimitiveCount() const
{
  return Internal::GetImplementation(*this).GetModelPrimitiveCount();
}

void ModelNode::AddModelPrimitive(ModelPrimitive modelPrimitive)
{
  Internal::GetImplementation(*this).AddModelPrimitive(modelPrimitive, 0u);
}

void ModelNode::RemoveModelPrimitive(Dali::Scene3D::ModelPrimitive modelPrimitive)
{
  Internal::GetImplementation(*this).RemoveModelPrimitive(modelPrimitive);
}

void ModelNode::RemoveModelPrimitive(uint32_t index)
{
  Internal::GetImplementation(*this).RemoveModelPrimitive(index);
}

ModelPrimitive ModelNode::GetModelPrimitive(uint32_t index) const
{
  return Internal::GetImplementation(*this).GetModelPrimitive(index);
}

ModelNode ModelNode::FindChildModelNodeByName(std::string_view nodeName)
{
  return Internal::GetImplementation(*this).FindChildModelNodeByName(nodeName);
}

void ModelNode::RetrieveBlendShapeNames(std::vector<std::string>& blendShapeNames) const
{
  return Internal::GetImplementation(*this).RetrieveBlendShapeNames(blendShapeNames);
}

Loader::BlendShapes::Index ModelNode::GetBlendShapeIndexByName(std::string_view blendShapeName) const
{
  return Internal::GetImplementation(*this).GetBlendShapeIndexByName(blendShapeName);
}

void ModelNode::SetColliderMesh(std::unique_ptr<Algorithm::ColliderMesh>&& colliderMesh)
{
  Internal::GetImplementation(*this).SetColliderMesh(std::move(colliderMesh));
}

const Algorithm::ColliderMesh& ModelNode::GetColliderMesh()
{
  return Internal::GetImplementation(*this).GetColliderMesh();
}

bool ModelNode::HasColliderMesh() const
{
  return Internal::GetImplementation(*this).HasColliderMesh();
}

void ModelNode::CastShadow(bool castShadow)
{
  Internal::GetImplementation(*this).CastShadow(castShadow);
}

bool ModelNode::IsShadowCasting() const
{
  return Internal::GetImplementation(*this).IsShadowCasting();
}

void ModelNode::ReceiveShadow(bool receiveShadow)
{
  Internal::GetImplementation(*this).ReceiveShadow(receiveShadow);
}

bool ModelNode::IsShadowReceiving() const
{
  return Internal::GetImplementation(*this).IsShadowReceiving();
}

} // namespace Scene3D

} // namespace Dali
