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

// INTERNAL
#include "dali-scene3d/public-api/loader/node-definition.h"
#include "dali-scene3d/public-api/loader/renderer-state.h"
#include "dali-scene3d/public-api/loader/utils.h"

namespace Dali
{
namespace
{
constexpr std::string_view IBL_INTENSITY_STRING("uIblIntensity");
constexpr std::string_view IBL_Y_DIRECTION("uYDirection");
} // namespace

namespace Scene3D
{
namespace Loader
{
bool NodeDefinition::Renderable::GetExtents(const ResourceBundle& resources, Vector3& min, Vector3& max) const
{
  return false;
}

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

std::string_view NodeDefinition::GetIblScaleFactorUniformName()
{
  return IBL_INTENSITY_STRING;
}

std::string_view NodeDefinition::GetIblYDirectionUniformName()
{
  return IBL_Y_DIRECTION;
}

bool NodeDefinition::GetExtents(const ResourceBundle& resources, Vector3& min, Vector3& max) const
{
  if(mRenderable)
  {
    if(!mRenderable->GetExtents(resources, min, max))
    {
      // If the renderable node don't have mesh accessor, use size to compute extents.
      min = -mSize / 2.0f;
      max = mSize / 2.0f;
    }
    return true;
  }
  return false;
}

bool ModelNode::GetExtents(const ResourceBundle& resources, Vector3& min, Vector3& max) const
{
  auto&    mesh    = resources.mMeshes[mMeshIdx];
  uint32_t minSize = mesh.first.mPositions.mBlob.mMin.size();
  uint32_t maxSize = mesh.first.mPositions.mBlob.mMax.size();
  if(minSize == maxSize && minSize >= 2u && maxSize >= 2u)
  {
    min = Vector3(mesh.first.mPositions.mBlob.mMin[0], mesh.first.mPositions.mBlob.mMin[1], 0.0f);
    max = Vector3(mesh.first.mPositions.mBlob.mMax[0], mesh.first.mPositions.mBlob.mMax[1], 0.0f);
    if(minSize == 3u)
    {
      min.z = mesh.first.mPositions.mBlob.mMin[2];
      max.z = mesh.first.mPositions.mBlob.mMax[2];
    }
    return true;
  }
  return false;
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

  actor.RegisterProperty("uHasVertexColor", static_cast<float>(mesh.first.mColors.IsDefined()));

  auto& matDef = resources.mMaterials[mMaterialIdx].first;
  actor.RegisterProperty("uColorFactor", matDef.mBaseColorFactor);
  actor.RegisterProperty("uMetallicFactor", matDef.mMetallic);
  actor.RegisterProperty("uRoughnessFactor", matDef.mRoughness);
  actor.RegisterProperty("uNormalScale", matDef.mNormalScale);
  if(matDef.mFlags & MaterialDefinition::OCCLUSION)
  {
    actor.RegisterProperty("uOcclusionStrength", matDef.mOcclusionStrength);
  }
  if(matDef.mFlags & MaterialDefinition::EMISSIVE)
  {
    actor.RegisterProperty("uEmissiveFactor", matDef.mEmissiveFactor);
  }

  Index envIdx = matDef.mEnvironmentIdx;
  actor.RegisterProperty(IBL_INTENSITY_STRING.data(), resources.mEnvironmentMaps[envIdx].first.mIblIntensity);
  actor.RegisterProperty(IBL_Y_DIRECTION.data(), resources.mEnvironmentMaps[envIdx].first.mYDirection);

  float opaque      = 0.0f;
  float mask        = 0.0f;
  float alphaCutoff = matDef.GetAlphaCutoff();
  if(!MaskMatch(matDef.mFlags, MaterialDefinition::TRANSPARENCY))
  {
    opaque = 1.0f;
  }
  else
  {
    if(alphaCutoff > 0.f)
    {
      mask = 1.0f;
    }
  }
  actor.RegisterProperty("uOpaque", opaque);
  actor.RegisterProperty("uMask", mask);
  actor.RegisterProperty("uAlphaThreshold", alphaCutoff);
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

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
