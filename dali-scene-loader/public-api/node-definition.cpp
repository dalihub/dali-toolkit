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

// INTERNAL
#include "dali-scene-loader/public-api/node-definition.h"
#include "dali-scene-loader/public-api/renderer-state.h"
#include "dali-scene-loader/public-api/utils.h"

namespace Dali
{
namespace SceneLoader
{
void NodeDefinition::Renderable::RegisterResources(IResourceReceiver& receiver) const
{
  receiver.Register(ResourceType::Shader, mShaderIdx);
}

void NodeDefinition::Renderable::ReflectResources(IResourceReflector& reflector)
{
  reflector.Reflect(ResourceType::Shader, mShaderIdx);
}

void NodeDefinition::Renderable::OnCreate(const NodeDefinition& node, CreateParams& params, Actor& actor) const
{
  DALI_ASSERT_DEBUG(mShaderIdx != INVALID_INDEX);
  auto&  resources = params.mResources;
  Shader shader    = resources.mShaders[mShaderIdx].second;

  static Geometry defaultGeometry = Geometry::New();
  Renderer        renderer        = Renderer::New(defaultGeometry, shader);

  RendererState::Apply(resources.mShaders[mShaderIdx].first.mRendererState, renderer);

  actor.AddRenderer(renderer);
}

const std::string NodeDefinition::ORIGINAL_MATRIX_PROPERTY_NAME = "originalMatrix";

Actor NodeDefinition::CreateActor(CreateParams& params) const
{
  Actor actor = Actor::New();
  SetActorCentered(actor);

  actor.SetProperty(Actor::Property::NAME, mName);
  actor.SetProperty(Actor::Property::POSITION, mPosition);
  actor.SetProperty(Actor::Property::ORIENTATION, mOrientation);
  actor.SetProperty(Actor::Property::SCALE, mScale);
  actor.SetProperty(Actor::Property::SIZE, mSize);
  actor.SetProperty(Actor::Property::VISIBLE, mIsVisible);

  actor.RegisterProperty(ORIGINAL_MATRIX_PROPERTY_NAME, GetLocalSpace(), Property::AccessMode::READ_ONLY);

  if(mRenderable)
  {
    mRenderable->OnCreate(*this, params, actor);
  }

  for(auto& e : mExtras)
  {
    actor.RegisterProperty(e.mKey, e.mValue);
  }

  for(auto& c : mConstraints)
  {
    params.mConstrainables.push_back(ConstraintRequest{&c, actor});
  }

  return actor;
}

Matrix NodeDefinition::GetLocalSpace() const
{
  Matrix localSpace{false};
  localSpace.SetTransformComponents(mScale, mOrientation, mPosition);
  return localSpace;
}

void ModelNode::RegisterResources(IResourceReceiver& receiver) const
{
  Renderable::RegisterResources(receiver);
  receiver.Register(ResourceType::Mesh, mMeshIdx);
  receiver.Register(ResourceType::Material, mMaterialIdx);
}

void ModelNode::ReflectResources(IResourceReflector& reflector)
{
  Renderable::ReflectResources(reflector);
  reflector.Reflect(ResourceType::Mesh, mMeshIdx);
  reflector.Reflect(ResourceType::Material, mMaterialIdx);
}

void ModelNode::OnCreate(const NodeDefinition& node, NodeDefinition::CreateParams& params, Actor& actor) const
{
  DALI_ASSERT_DEBUG(mMeshIdx != INVALID_INDEX);
  Renderable::OnCreate(node, params, actor);

  auto& resources = params.mResources;
  auto& mesh      = resources.mMeshes[mMeshIdx];

  auto     renderer = actor.GetRendererAt(0);
  Geometry geometry = mesh.second.geometry;
  renderer.SetGeometry(geometry);

  auto shader = renderer.GetShader();

  if(mesh.first.IsSkinned())
  {
    params.mSkinnables.push_back(SkinningShaderConfigurationRequest{mesh.first.mSkeletonIdx, shader});
  }

  if(mesh.first.HasBlendShapes())
  {
    params.mBlendshapeRequests.push_back(BlendshapeShaderConfigurationRequest{node.mName, mMeshIdx, shader});
  }

  TextureSet textures = resources.mMaterials[mMaterialIdx].second;

  // Set the blend shape texture.
  if(mesh.second.blendShapeGeometry)
  {
    TextureSet newTextureSet = TextureSet::New();
    newTextureSet.SetTexture(0u, mesh.second.blendShapeGeometry);

    const unsigned int numberOfTextures = textures.GetTextureCount();
    for(unsigned int index = 0u; index < numberOfTextures; ++index)
    {
      const unsigned int newIndex = index + 1u;
      newTextureSet.SetTexture(newIndex, textures.GetTexture(index));
      newTextureSet.SetSampler(newIndex, textures.GetSampler(index));
    }

    textures = newTextureSet;
  }

  renderer.SetTextures(textures);

  actor.SetProperty(Actor::Property::COLOR, mColor);

  auto& matDef = resources.mMaterials[mMaterialIdx].first;
  actor.RegisterProperty("uMetallicFactor", matDef.mMetallic);
  actor.RegisterProperty("uRoughnessFactor", matDef.mRoughness);

  Index envIdx = matDef.mEnvironmentIdx;
  actor.RegisterProperty("uIblIntensity", resources.mEnvironmentMaps[envIdx].first.mIblIntensity);

  const auto alphaCutoff = matDef.GetAlphaCutoff();
  if(alphaCutoff > 0.f)
  {
    actor.RegisterProperty("uAlphaThreshold", alphaCutoff);
  }
}

void ArcNode::OnCreate(const NodeDefinition& node, NodeDefinition::CreateParams& params, Actor& actor) const
{
  ModelNode::OnCreate(node, params, actor);

  actor.RegisterProperty("antiAliasing", mAntiAliasing ? 1 : 0);
  actor.RegisterProperty("arcCaps", mArcCaps);
  actor.RegisterProperty("radius", mRadius);

  const float startAngleRadians = mStartAngleDegrees * Math::PI_OVER_180;
  Vector2     startPolar{std::cos(startAngleRadians), std::sin(startAngleRadians)};
  actor.RegisterProperty("startAngle", startPolar);

  const float endAngleRadians = mEndAngleDegrees * Math::PI_OVER_180;
  Vector2     endPolar{std::cos(endAngleRadians), std::sin(endAngleRadians)};
  actor.RegisterProperty("endAngle", endPolar);
}

void ArcNode::GetEndVectorWithDiffAngle(float startAngle, float diffAngle, Vector2& endVector)
{
  float endAngle = 0.f;

  if(diffAngle <= 0.001f)
  {
    //0.001 is used to ensure is empty arc when startAngle = endAngle + 360 * N
    endAngle = startAngle + 0.001f;
  }
  else if(diffAngle >= 360.f)
  {
    endAngle = diffAngle + 359.99f;
  }
  else
  {
    endAngle = startAngle + 360.0f + diffAngle;
  }
  endVector.x = cosf(endAngle * Math::PI_OVER_180);
  endVector.y = sinf(endAngle * Math::PI_OVER_180);
}

} // namespace SceneLoader
} // namespace Dali
