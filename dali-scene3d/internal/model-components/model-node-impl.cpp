/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/model/model-impl.h>
#include <dali-scene3d/internal/light/light-impl.h>
#include <dali-scene3d/internal/model-components/model-primitive-impl.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
/**
 * Creates control through type registry
 */
BaseHandle Create()
{
  return Scene3D::ModelNode::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::ModelNode, Dali::CustomActor, Create);
DALI_TYPE_REGISTRATION_END()
} // unnamed namespace

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
: Control(static_cast<ControlBehaviour>(ControlBehaviour::DISABLE_STYLE_CHANGE_SIGNALS | ActorFlags::DISABLE_SIZE_NEGOTIATION))
{
}

ModelNode::~ModelNode()
{
}

// From Internal::Control.

void ModelNode::OnInitialize()
{
  Actor self = Self();

  // TODO : We need to check this is enough.
  Toolkit::DevelControl::EnableCreateAccessible(Toolkit::Control::DownCast(self), false);

  self.RegisterProperty("uIsShadowCasting", static_cast<int>(mIsShadowCasting));
  self.RegisterProperty("uIsShadowReceiving", static_cast<int>(mIsShadowReceiving));
}

// From CustomActorImpl.

void ModelNode::OnSceneConnection(int depth)
{
}

void ModelNode::OnSceneDisconnection()
{
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
  ModelNode&       impl            = dynamic_cast<Internal::ModelNode&>(customInterface);
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
  return static_cast<uint32_t>(mModelPrimitiveContainer.size());
}

void ModelNode::AddModelPrimitive(Dali::Scene3D::ModelPrimitive modelPrimitive, Loader::ShaderOption::HashType hash)
{
  for(auto&& primitive : mModelPrimitiveContainer)
  {
    if(primitive == modelPrimitive)
    {
      return;
    }
  }

  mModelPrimitiveContainer.push_back(modelPrimitive);

  Actor self = Self();
  GetImplementation(modelPrimitive).AddPrimitiveObserver(this);
  if(mShadowMapTexture)
  {
    GetImplementation(modelPrimitive).SetShadowMapTexture(mShadowMapTexture);
  }

  if(mDiffuseTexture && mSpecularTexture)
  {
    GetImplementation(modelPrimitive).SetImageBasedLightTexture(mDiffuseTexture, mSpecularTexture, mIblScaleFactor, mSpecularMipmapLevels);
  }

  GetImplementation(modelPrimitive).UpdateShader(mShaderManager, hash);

  Dali::Renderer renderer = GetImplementation(modelPrimitive).GetRenderer();
  if(renderer)
  {
    uint32_t rendererCount = self.GetRendererCount();
    bool     exist         = false;
    for(uint32_t i = 0; i < rendererCount; ++i)
    {
      if(renderer == self.GetRendererAt(i))
      {
        exist = true;
        break;
      }
    }
    if(!exist)
    {
      self.AddRenderer(renderer);
    }
  }
}

void ModelNode::RemoveModelPrimitive(Dali::Scene3D::ModelPrimitive modelPrimitive)
{
  uint32_t primitiveCount = GetModelPrimitiveCount();
  for(uint32_t i = 0; i < primitiveCount; ++i)
  {
    if(mModelPrimitiveContainer[i] != modelPrimitive)
    {
      continue;
    }

    RemoveModelPrimitive(i);
    break;
  }
}

void ModelNode::RemoveModelPrimitive(uint32_t index)
{
  if(index >= mModelPrimitiveContainer.size())
  {
    return;
  }

  GetImplementation(mModelPrimitiveContainer[index]).UpdateShader(nullptr, 0u);

  Actor self = Self();
  GetImplementation(mModelPrimitiveContainer[index]).RemovePrimitiveObserver(this);

  Dali::Renderer renderer = GetImplementation(mModelPrimitiveContainer[index]).GetRenderer();
  if(renderer)
  {
    self.RemoveRenderer(renderer);
  }

  mModelPrimitiveContainer.erase(mModelPrimitiveContainer.begin() + index);
}

Dali::Scene3D::ModelPrimitive ModelNode::GetModelPrimitive(uint32_t index) const
{
  if(index < mModelPrimitiveContainer.size())
  {
    return mModelPrimitiveContainer[index];
  }
  return Scene3D::ModelPrimitive();
}

Scene3D::ModelNode ModelNode::FindChildModelNodeByName(std::string_view nodeName)
{
  Actor childActor = Self().FindChildByName(nodeName);
  return Scene3D::ModelNode::DownCast(childActor);
}

void ModelNode::RetrieveBlendShapeNames(std::vector<std::string>& blendShapeNames) const
{
  blendShapeNames.reserve(blendShapeNames.size() + mBlendShapeIndexMap.size());
  for(const auto& iter : mBlendShapeIndexMap)
  {
    blendShapeNames.push_back(iter.first);
  }
}

Loader::BlendShapes::Index ModelNode::GetBlendShapeIndexByName(std::string_view blendShapeName) const
{
  auto iter = mBlendShapeIndexMap.find(std::string(blendShapeName));
  if(iter != mBlendShapeIndexMap.end())
  {
    return iter->second;
  }
  return Loader::BlendShapes::INVALID_INDEX;
}

void ModelNode::SetShadowMapTexture(Dali::Texture shadowMapTexture)
{
  mShadowMapTexture = shadowMapTexture;
  for(auto&& primitive : mModelPrimitiveContainer)
  {
    GetImplementation(primitive).SetShadowMapTexture(mShadowMapTexture);
  }
}

void ModelNode::CastShadow(bool castShadow)
{
  if(mIsShadowCasting == castShadow)
  {
    return;
  }

  mIsShadowCasting = castShadow;

  Actor self = Self();
  self.RegisterProperty("uIsShadowCasting", static_cast<int>(mIsShadowCasting));
}

bool ModelNode::IsShadowCasting() const
{
  return mIsShadowCasting;
}

void ModelNode::ReceiveShadow(bool receiveShadow)
{
  if(mIsShadowReceiving == receiveShadow)
  {
    return;
  }

  mIsShadowReceiving = receiveShadow;

  Actor self = Self();
  self.RegisterProperty("uIsShadowReceiving", static_cast<int>(mIsShadowReceiving));
}

bool ModelNode::IsShadowReceiving() const
{
  return mIsShadowReceiving;
}

void ModelNode::SetImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float iblScaleFactor, uint32_t specularMipmapLevels)
{
  mDiffuseTexture       = diffuseTexture;
  mSpecularTexture      = specularTexture;
  mIblScaleFactor       = iblScaleFactor;
  mSpecularMipmapLevels = specularMipmapLevels;
  for(auto&& primitive : mModelPrimitiveContainer)
  {
    GetImplementation(primitive).SetImageBasedLightTexture(diffuseTexture, specularTexture, iblScaleFactor, specularMipmapLevels);
  }
}

void ModelNode::SetImageBasedLightScaleFactor(float iblScaleFactor)
{
  mIblScaleFactor = iblScaleFactor;
  for(auto&& primitive : mModelPrimitiveContainer)
  {
    GetImplementation(primitive).SetImageBasedLightScaleFactor(iblScaleFactor);
  }
}

void ModelNode::UpdateShader(Scene3D::Loader::ShaderManagerPtr shaderManager)
{
  if(mShaderManager != shaderManager)
  {
    mShaderManager = shaderManager;
    for(auto&& primitive : mModelPrimitiveContainer)
    {
      GetImplementation(primitive).UpdateShader(mShaderManager, 0u);
    }
  }
}

void ModelNode::SetBlendShapeData(Scene3D::Loader::BlendShapes::BlendShapeData&& data, Scene3D::ModelPrimitive primitive)
{
  // Update mBlendShapeIndexMap
  mBlendShapeIndexMap.clear();
  const auto blendShapeCount = data.names.size();
  for(Loader::BlendShapes::Index index = 0u; index < blendShapeCount; ++index)
  {
    auto& name = data.names[index];
    if(!name.empty())
    {
      mBlendShapeIndexMap[name] = index;
    }
  }

  GetImplementation(primitive).SetBlendShapeData(std::move(data));
}

void ModelNode::SetBoneMatrix(const Matrix& inverseMatrix, Scene3D::ModelPrimitive primitive, Scene3D::Loader::Index& boneIndex)
{
  Dali::Scene3D::Loader::Skinning::BoneData boneData;
  boneData.primitive = primitive;
  boneData.boneIndex = boneIndex;
  char propertyNameBuffer[32];
  snprintf(propertyNameBuffer, sizeof(propertyNameBuffer), "%s[%d]", Dali::Scene3D::Loader::Skinning::BONE_UNIFORM_NAME, boneIndex);
  boneData.propertyName  = propertyNameBuffer;
  boneData.inverseMatrix = inverseMatrix;
  mBoneDataContainer.push_back(std::move(boneData));

  UpdateBoneMatrix(primitive);
}

void ModelNode::OnRendererCreated(Renderer renderer)
{
  Self().AddRenderer(renderer);
}

void ModelNode::UpdateBoneMatrix(Scene3D::ModelPrimitive primitive)
{
  for(auto&& boneData : mBoneDataContainer)
  {
    if(boneData.primitive != primitive)
    {
      continue;
    }

    Dali::Renderer renderer = GetImplementation(primitive).GetRenderer();
    if(!renderer)
    {
      continue;
    }

    if(boneData.constraint)
    {
      boneData.constraint.Remove();
      boneData.constraint.Reset();
    }

    auto propBoneXform = renderer.GetPropertyIndex(boneData.propertyName);
    if(propBoneXform == Property::INVALID_INDEX)
    {
      propBoneXform = renderer.RegisterProperty(boneData.propertyName, Matrix{false});
    }

    Matrix inverseMatrix = boneData.inverseMatrix;
    // Constrain bone matrix to joint transform.
    boneData.constraint = Constraint::New<Matrix>(renderer, propBoneXform, [inverseMatrix](Matrix& output, const PropertyInputContainer& inputs) { Matrix::Multiply(output, inverseMatrix, inputs[0]->GetMatrix()); });

    Actor joint = Self();
    boneData.constraint.AddSource(Source{joint, Actor::Property::WORLD_MATRIX});
    boneData.constraint.ApplyPost();
    break;
  }
}

void ModelNode::SetColliderMesh(ColliderMeshUniquePtr&& colliderMesh)
{
  if(!colliderMesh && !mColliderMesh)
  {
    return;
  }

  if(!mParentModel) // find parent model if not set
  {
    auto parent = Self().GetParent();
    while(parent)
    {
      auto modelHandle = Scene3D::Model::DownCast(parent);
      if(modelHandle)
      {
        mParentModel = &GetImpl(modelHandle);
        break;
      }
      parent = parent.GetParent();
    }
  }

  // Resetting collider mesh if argument is nullptr
  auto handle = Scene3D::ModelNode::DownCast(Self());
  if(mParentModel)
  {
    if(mColliderMesh)
    {
      mParentModel->RemoveColliderMesh(handle);
    }

    // If collider mesh is to be set then register it with the parent model.
    // If nullptr, ignore.
    if(colliderMesh)
    {
      mParentModel->RegisterColliderMesh(handle);
    }
  }

  mColliderMesh = std::move(colliderMesh);
}

bool ModelNode::HasColliderMesh() const
{
  return mColliderMesh != nullptr;
}

const Scene3D::Algorithm::ColliderMesh& ModelNode::GetColliderMesh() const
{
  return *mColliderMesh;
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali
