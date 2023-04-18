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
#include <dali-scene3d/internal/model-components/model-node-data-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-components/model-primitive-impl.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

namespace
{
} // namespace

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

ModelNode::Impl::Impl(ModelNode& modelNodeImpl)
: mModelNodeImpl(modelNodeImpl)
{
}

ModelNode::Impl::~Impl()
{
  for(auto&& primitive : mModelPrimitiveContainer)
  {
    GetImplementation(primitive).RemovePrimitiveObserver(this);
  }
  for(auto&& boneData : mBoneDataContainer)
  {
    boneData.primitive.Reset();
    if(boneData.constraint)
    {
      boneData.constraint.Remove();
      boneData.constraint.Reset();
    }
  }
}

void ModelNode::Impl::OnSceneConnection(int depth)
{
}

void ModelNode::Impl::OnSceneDisconnection()
{
}

void ModelNode::Impl::OnRendererCreated(Renderer renderer)
{
  mModelNodeImpl.Self().AddRenderer(renderer);
}

// Public Method

void ModelNode::Impl::AddModelPrimitive(Scene3D::ModelPrimitive modelPrimitive)
{
  for(auto&& primitive : mModelPrimitiveContainer)
  {
    if(primitive == modelPrimitive)
    {
      return;
    }
  }

  mModelPrimitiveContainer.push_back(modelPrimitive);

  Actor self = mModelNodeImpl.Self();

  GetImplementation(modelPrimitive).AddPrimitiveObserver(this);
  if(mDiffuseTexture && mSpecularTexture)
  {
    GetImplementation(modelPrimitive).SetImageBasedLightTexture(mDiffuseTexture, mSpecularTexture, mIblScaleFactor, mSpecularMipmapLevels);
  }

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

void ModelNode::Impl::RemoveModelPrimitive(Scene3D::ModelPrimitive modelPrimitive)
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

void ModelNode::Impl::RemoveModelPrimitive(uint32_t index)
{
  if(index >= mModelPrimitiveContainer.size())
  {
    return;
  }

  Actor self = mModelNodeImpl.Self();
  GetImplementation(mModelPrimitiveContainer[index]).RemovePrimitiveObserver(this);

  Dali::Renderer renderer = GetImplementation(mModelPrimitiveContainer[index]).GetRenderer();
  if(renderer)
  {
    self.RemoveRenderer(renderer);
  }

  mModelPrimitiveContainer.erase(mModelPrimitiveContainer.begin() + index);
}

Scene3D::ModelPrimitive ModelNode::Impl::GetModelPrimitive(uint32_t index) const
{
  if(index < mModelPrimitiveContainer.size())
  {
    return mModelPrimitiveContainer[index];
  }
  return Scene3D::ModelPrimitive();
}

void ModelNode::Impl::SetImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float iblScaleFactor, uint32_t specularMipmapLevels)
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

void ModelNode::Impl::SetImageBasedLightScaleFactor(float iblScaleFactor)
{
  mIblScaleFactor = iblScaleFactor;
  for(auto&& primitive : mModelPrimitiveContainer)
  {
    GetImplementation(primitive).SetImageBasedLightScaleFactor(iblScaleFactor);
  }
}

void ModelNode::Impl::SetBlendShapeData(Scene3D::Loader::BlendShapes::BlendShapeData& data, Scene3D::ModelPrimitive primitive)
{
  GetImplementation(primitive).SetBlendShapeData(data);
}

void ModelNode::Impl::SetBoneMatrix(const Matrix& inverseMatrix, Scene3D::ModelPrimitive primitive, Scene3D::Loader::Index& boneIndex)
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

void ModelNode::Impl::UpdateBoneMatrix(Scene3D::ModelPrimitive primitive)
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

    Dali::Shader shader = renderer.GetShader();
    if(!shader)
    {
      continue;
    }

    if(boneData.constraint)
    {
      boneData.constraint.Remove();
      boneData.constraint.Reset();
    }

    if(shader.GetPropertyIndex(boneData.propertyName) == Property::INVALID_INDEX)
    {
      auto propBoneXform = shader.RegisterProperty(boneData.propertyName, Matrix{false});

      Matrix inverseMatrix = boneData.inverseMatrix;
      // Constrain bone matrix to joint transform.
      boneData.constraint = Constraint::New<Matrix>(shader, propBoneXform, [inverseMatrix](Matrix& output, const PropertyInputContainer& inputs)
                                                    { Matrix::Multiply(output, inverseMatrix, inputs[0]->GetMatrix()); });

      Actor joint = mModelNodeImpl.Self();
      boneData.constraint.AddSource(Source{joint, Actor::Property::WORLD_MATRIX});
      boneData.constraint.ApplyPost();
    }
    break;
  }
}

} // namespace Internal

} // namespace Scene3D

} // namespace Dali
