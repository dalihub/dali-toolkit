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
#include <dali-scene3d/internal/controls/model/model-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <filesystem>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/image-resource-loader.h>
#include <dali-scene3d/internal/common/model-cache-manager.h>
#include <dali-scene3d/internal/controls/scene-view/scene-view-impl.h>
#include <dali-scene3d/internal/event/collider-mesh-processor.h>
#include <dali-scene3d/internal/light/light-impl.h>
#include <dali-scene3d/internal/model-components/model-node-impl.h>
#include <dali-scene3d/internal/model-components/model-node-tree-utility.h>
#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/loader/animation-definition.h>
#include <dali-scene3d/public-api/loader/camera-parameters.h>
#include <dali-scene3d/public-api/loader/light-parameters.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/node-definition.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>
#include <dali-scene3d/public-api/model-motion/motion-index/blend-shape-index.h>
#include <dali-toolkit/public-api/controls/control-impl.h>

using namespace Dali;

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
  return Scene3D::Model::New(std::string());
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::Model, Toolkit::Control, Create);
DALI_TYPE_REGISTRATION_END()

static constexpr Vector3 Y_DIRECTION(1.0f, -1.0f, 1.0f);
static constexpr float   SIZE_STEP_CONDITION = 0.1f;

static constexpr bool DEFAULT_MODEL_CHILDREN_SENSITIVE = false;
static constexpr bool DEFAULT_MODEL_CHILDREN_FOCUSABLE = false;

struct BoundingVolume
{
  void Init()
  {
    pointMin = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    pointMax = Vector3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
  }

  void ConsiderNewPointInVolume(const Vector3& position)
  {
    pointMin.x = std::min(position.x, pointMin.x);
    pointMin.y = std::min(position.y, pointMin.y);
    pointMin.z = std::min(position.z, pointMin.z);

    pointMax.x = std::max(position.x, pointMax.x);
    pointMax.y = std::max(position.y, pointMax.y);
    pointMax.z = std::max(position.z, pointMax.z);
  }

  Vector3 CalculateSize()
  {
    return pointMax - pointMin;
  }

  Vector3 CalculatePivot()
  {
    Vector3 pivot = pointMin / (pointMin - pointMax);
    for(uint32_t i = 0; i < 3; ++i)
    {
      // To avoid divid by zero
      if(Dali::Equals(pointMin[i], pointMax[i]))
      {
        pivot[i] = 0.5f;
      }
    }
    return pivot;
  }

  Vector3 pointMin;
  Vector3 pointMax;
};

void ConfigureBlendShapeShaders(
  Dali::Scene3D::Loader::ResourceBundle& resources, const Dali::Scene3D::Loader::SceneDefinition& scene, Actor root, std::vector<Dali::Scene3D::Loader::BlendshapeShaderConfigurationRequest>&& requests)
{
  std::vector<std::string> errors;
  auto                     onError = [&errors](const std::string& msg) { errors.push_back(msg); };
  if(!scene.ConfigureBlendshapeShaders(resources, root, std::move(requests), onError))
  {
    Dali::Scene3D::Loader::ExceptionFlinger flinger(ASSERT_LOCATION);
    for(auto& msg : errors)
    {
      flinger << msg << '\n';
    }
  }
}

void AddModelTreeToAABB(BoundingVolume& AABB, const Dali::Scene3D::Loader::SceneDefinition& scene, const Dali::Scene3D::Loader::Customization::Choices& choices, Dali::Scene3D::Loader::Index iNode, Dali::Scene3D::Loader::NodeDefinition::CreateParams& nodeParams, Matrix parentMatrix)
{
  static constexpr uint32_t BOX_POINT_COUNT             = 8;
  static uint32_t           BBIndex[BOX_POINT_COUNT][3] = {{0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}, {0, 0, 1}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1}};

  Matrix                                       nodeMatrix;
  const Dali::Scene3D::Loader::NodeDefinition* node        = scene.GetNode(iNode);
  Matrix                                       localMatrix = node->GetLocalSpace();
  Matrix::Multiply(nodeMatrix, localMatrix, parentMatrix);

  Vector3 volume[2];
  if(node->GetExtents(nodeParams.mResources, volume[0], volume[1]))
  {
    for(uint32_t i = 0; i < BOX_POINT_COUNT; ++i)
    {
      Vector4 position       = Vector4(volume[BBIndex[i][0]].x, volume[BBIndex[i][1]].y, volume[BBIndex[i][2]].z, 1.0f);
      Vector4 objectPosition = nodeMatrix * position;
      objectPosition /= objectPosition.w;

      AABB.ConsiderNewPointInVolume(Vector3(objectPosition));
    }
  }

  if(node->mCustomization)
  {
    if(!node->mChildren.empty())
    {
      auto                         choice = choices.Get(node->mCustomization->mTag);
      Dali::Scene3D::Loader::Index i      = std::min(choice != Dali::Scene3D::Loader::Customization::NONE ? choice : 0, static_cast<Dali::Scene3D::Loader::Index>(node->mChildren.size() - 1));

      AddModelTreeToAABB(AABB, scene, choices, node->mChildren[i], nodeParams, nodeMatrix);
    }
  }
  else
  {
    for(auto i : node->mChildren)
    {
      AddModelTreeToAABB(AABB, scene, choices, i, nodeParams, nodeMatrix);
    }
  }
}

void UpdateBlendShapeNodeMapRecursively(Model::BlendShapeModelNodeMap& resultMap, const Scene3D::ModelNode& node)
{
  if(!node)
  {
    return;
  }
  const auto childCount = node.GetChildCount();
  for(auto i = 0u; i < childCount; ++i)
  {
    UpdateBlendShapeNodeMapRecursively(resultMap, Scene3D::ModelNode::DownCast(node.GetChildAt(i)));
  }

  std::vector<std::string> blendShapeNames;
  node.RetrieveBlendShapeNames(blendShapeNames);
  for(const auto& iter : blendShapeNames)
  {
    // Append or create new list.
    resultMap[iter].push_back(node);
  }
}

void ResetResourceTask(IntrusivePtr<AsyncTask>&& asyncTask)
{
  if(!asyncTask)
  {
    return;
  }
  Dali::AsyncTaskManager::Get().RemoveTask(asyncTask);
  asyncTask.Reset();
}

} // anonymous namespace

Model::Model(const std::string& modelUrl, const std::string& resourceDirectoryUrl)
: Control(ControlBehaviour(DISABLE_SIZE_NEGOTIATION | DISABLE_STYLE_CHANGE_SIGNALS)),
  mModelUrl(modelUrl),
  mResourceDirectoryUrl(resourceDirectoryUrl),
  mModelRoot(),
  mShaderManager(new Scene3D::Loader::ShaderManager()),
  mNaturalSize(Vector3::ZERO),
  mModelPivot(AnchorPoint::CENTER),
  mSceneIblScaleFactor(1.0f),
  mIblScaleFactor(1.0f),
  mSceneSpecularMipmapLevels(1u),
  mSpecularMipmapLevels(1u),
  mModelChildrenSensitive(DEFAULT_MODEL_CHILDREN_SENSITIVE),
  mModelChildrenFocusable(DEFAULT_MODEL_CHILDREN_FOCUSABLE),
  mModelResourceReady(false),
  mIblDiffuseResourceReady(true),
  mIblSpecularResourceReady(true),
  mIblDiffuseDirty(false),
  mIblSpecularDirty(false),
  mIsShadowCasting(true),
  mIsShadowReceiving(true)
{
}

Model::~Model()
{
  ResetResourceTasks();

  if(ModelCacheManager::Get() && !mModelUrl.empty())
  {
    ModelCacheManager::Get().UnreferenceModelCache(mModelUrl);
  }
}

Dali::Scene3D::Model Model::New(const std::string& modelUrl, const std::string& resourceDirectoryUrl)
{
  Model* impl = new Model(modelUrl, resourceDirectoryUrl);

  Dali::Scene3D::Model handle = Dali::Scene3D::Model(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

const Scene3D::ModelNode Model::GetModelRoot() const
{
  return mModelRoot;
}

void Model::AddModelNode(Scene3D::ModelNode modelNode)
{
  if(!mModelRoot)
  {
    CreateModelRoot();
  }

  mModelRoot.Add(modelNode);
  if(mModelUrl.empty())
  {
    mModelResourceReady = true;
  }

  ModelNodeTreeUtility::UpdateShaderRecursively(modelNode, mShaderManager);

  if(mShadowMapTexture)
  {
    ModelNodeTreeUtility::UpdateShadowMapTextureRecursively(modelNode, mShadowMapTexture);
  }

  if(mIblDiffuseResourceReady && mIblSpecularResourceReady)
  {
    UpdateImageBasedLightTexture();
    UpdateImageBasedLightScaleFactor();
  }

  GetImplementation(modelNode).SetRootModel(this);

  // If model has a collider mesh set, add it to the container
  if(modelNode.HasColliderMesh())
  {
    RegisterColliderMesh(modelNode);
    Scene3D::ColliderMeshProcessor::Get().ColliderMeshChanged(*this);
  }

  if(Self().GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE))
  {
    NotifyResourceReady();
  }
}

void Model::RegisterColliderMesh(Scene3D::ModelNode& modelNode)
{
  mColliderMeshes[modelNode.GetProperty<int>(Actor::Property::ID)] = modelNode;

  // Add processor
  Scene3D::ColliderMeshProcessor::Get().ColliderMeshChanged(*this);
}

void Model::RemoveColliderMesh(Scene3D::ModelNode& node)
{
  auto id   = node.GetProperty<int>(Actor::Property::ID);
  auto iter = std::find_if(mColliderMeshes.begin(), mColliderMeshes.end(), [id](auto& item) {
    return item.first == id;
  });
  if(iter != mColliderMeshes.end())
  {
    mColliderMeshes.erase(iter);
  }
}

void Model::RemoveModelNode(Scene3D::ModelNode modelNode)
{
  // remove collider mesh from the list if node is being removed
  if(modelNode.HasColliderMesh())
  {
    RemoveColliderMesh(modelNode);
    GetImplementation(modelNode).SetRootModel(nullptr);
  }

  if(mModelRoot)
  {
    ModelNodeTreeUtility::UpdateShaderRecursively(modelNode, nullptr);
    mModelRoot.Remove(modelNode);
  }
}

void Model::SetChildrenSensitive(bool enable)
{
  if(mModelChildrenSensitive != enable)
  {
    mModelChildrenSensitive = enable;
    if(mModelRoot)
    {
      mModelRoot.SetProperty(Dali::Actor::Property::SENSITIVE, mModelChildrenSensitive);
    }
  }
}

bool Model::GetChildrenSensitive() const
{
  return mModelChildrenSensitive;
}

void Model::SetChildrenFocusable(bool enable)
{
  if(mModelChildrenFocusable != enable)
  {
    mModelChildrenFocusable = enable;
    if(mModelRoot)
    {
      mModelRoot.SetProperty(Dali::Actor::Property::KEYBOARD_FOCUSABLE, mModelChildrenFocusable);
      mModelRoot.SetProperty(Dali::DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN, mModelChildrenFocusable);
    }
  }
}

bool Model::GetChildrenFocusable() const
{
  return mModelChildrenFocusable;
}

void Model::SetImageBasedLightSource(const std::string& diffuseUrl, const std::string& specularUrl, float scaleFactor)
{
  bool needIblReset = false;
  bool isOnScene    = Self().GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE);
  if(mDiffuseIblUrl != diffuseUrl)
  {
    mDiffuseIblUrl = diffuseUrl;
    if(mDiffuseIblUrl.empty())
    {
      needIblReset = true;
    }
    else
    {
      mIblDiffuseDirty         = true;
      mIblDiffuseResourceReady = false;
    }
  }

  if(mSpecularIblUrl != specularUrl)
  {
    mSpecularIblUrl = specularUrl;
    if(mSpecularIblUrl.empty())
    {
      needIblReset = true;
    }
    else
    {
      mIblSpecularDirty         = true;
      mIblSpecularResourceReady = false;
    }
  }

  // If one or both of diffuse url and specular url are empty,
  // we don't need to request to load texture.
  if(needIblReset)
  {
    ResetResourceTask(mIblDiffuseLoadTask);
    ResetResourceTask(mIblSpecularLoadTask);

    mIblDiffuseDirty          = false;
    mIblSpecularDirty         = false;
    mIblDiffuseResourceReady  = true;
    mIblSpecularResourceReady = true;

    mDiffuseTexture.Reset();
    mSpecularTexture.Reset();
    UpdateImageBasedLightTexture();

    // Request image resource GC
    Dali::Scene3D::Internal::ImageResourceLoader::RequestGarbageCollect();
  }
  else
  {
    if(isOnScene && mIblDiffuseDirty)
    {
      ResetResourceTask(mIblDiffuseLoadTask);
      mIblDiffuseLoadTask = new EnvironmentMapLoadTask(mDiffuseIblUrl, Scene3D::EnvironmentMapType::CUBEMAP, MakeCallback(this, &Model::OnIblDiffuseLoadComplete));
      Dali::AsyncTaskManager::Get().AddTask(mIblDiffuseLoadTask);
      mIblDiffuseDirty = false;

      // Request image resource GC
      Dali::Scene3D::Internal::ImageResourceLoader::RequestGarbageCollect();
    }

    if(isOnScene && mIblSpecularDirty)
    {
      ResetResourceTask(mIblSpecularLoadTask);
      mIblSpecularLoadTask = new EnvironmentMapLoadTask(mSpecularIblUrl, Scene3D::EnvironmentMapType::CUBEMAP, MakeCallback(this, &Model::OnIblSpecularLoadComplete));
      Dali::AsyncTaskManager::Get().AddTask(mIblSpecularLoadTask);
      mIblSpecularDirty = false;

      // Request image resource GC
      Dali::Scene3D::Internal::ImageResourceLoader::RequestGarbageCollect();
    }
  }

  if(!Dali::Equals(mIblScaleFactor, scaleFactor))
  {
    mIblScaleFactor = scaleFactor;
    UpdateImageBasedLightScaleFactor();
  }

  // If diffuse and specular textures are already loaded, emits resource ready signal here.
  NotifyResourceReady();
}

void Model::SetImageBasedLightScaleFactor(float scaleFactor)
{
  mIblScaleFactor = scaleFactor;
  if(mDiffuseTexture && mSpecularTexture)
  {
    UpdateImageBasedLightScaleFactor();
  }
}

float Model::GetImageBasedLightScaleFactor() const
{
  return mIblScaleFactor;
}

uint32_t Model::GetAnimationCount() const
{
  return mAnimations.size();
}

Dali::Animation Model::GetAnimation(uint32_t index) const
{
  Dali::Animation animation;
  if(mAnimations.size() > index)
  {
    animation = mAnimations[index].second;
  }
  return animation;
}

Dali::Animation Model::GetAnimation(const std::string& name) const
{
  Dali::Animation animation;
  if(!name.empty())
  {
    for(auto&& animationData : mAnimations)
    {
      if(animationData.first == name)
      {
        animation = animationData.second;
        break;
      }
    }
  }
  return animation;
}

uint32_t Model::GetCameraCount() const
{
  return mCameraParameters.size();
}

Dali::CameraActor Model::GenerateCamera(uint32_t index) const
{
  Dali::CameraActor camera;
  if(mCameraParameters.size() > index)
  {
    camera = Dali::CameraActor::New3DCamera();
    if(!mCameraParameters[index].ConfigureCamera(camera, false))
    {
      DALI_LOG_ERROR("Fail to generate %u's camera actor : Some property was not defined. Please check model file.\n", index);
      camera.Reset();
      return camera;
    }

    ApplyCameraTransform(camera);
  }
  return camera;
}

bool Model::ApplyCamera(uint32_t index, Dali::CameraActor camera) const
{
  if(camera && mCameraParameters.size() > index)
  {
    if(!mCameraParameters[index].ConfigureCamera(camera, false))
    {
      DALI_LOG_ERROR("Fail to apply %u's camera actor : Some property was not defined. Please check model file.\n", index);
      return false;
    }

    ApplyCameraTransform(camera);
    return true;
  }
  return false;
}

Scene3D::ModelNode Model::FindChildModelNodeByName(std::string_view nodeName)
{
  Actor childActor = Self().FindChildByName(nodeName);
  return Scene3D::ModelNode::DownCast(childActor);
}

void Model::RetrieveBlendShapeNames(std::vector<std::string>& blendShapeNames) const
{
  blendShapeNames.reserve(blendShapeNames.size() + mBlendShapeModelNodeMap.size());
  for(const auto& iter : mBlendShapeModelNodeMap)
  {
    blendShapeNames.push_back(iter.first);
  }
}

void Model::RetrieveModelNodesByBlendShapeName(std::string_view blendShapeName, std::vector<Scene3D::ModelNode>& modelNodes) const
{
  auto iter = mBlendShapeModelNodeMap.find(std::string(blendShapeName));
  if(iter != mBlendShapeModelNodeMap.end())
  {
    const auto& modelNodeList = iter->second;
    modelNodes.reserve(modelNodes.size() + modelNodeList.size());
    for(const auto& nodeIter : modelNodeList)
    {
      modelNodes.push_back(nodeIter);
    }
  }
}

Dali::Animation Model::GenerateMotionDataAnimation(Scene3D::MotionData motionData)
{
  Dali::Animation animation;

  // TODO : Need to collect duplicated codes with SetMotionData()

  if(motionData)
  {
    const uint32_t motionCount = motionData.GetMotionCount();
    for(uint32_t i = 0u; i < motionCount; ++i)
    {
      auto motionIndex = motionData.GetIndex(i);
      auto motionValue = motionData.GetValue(i);
      if(motionIndex && motionValue)
      {
        if(motionIndex.GetModelNodeId() != Property::INVALID_KEY)
        {
          Scene3D::ModelNode modelNode;
          if(motionIndex.GetModelNodeId().type == Property::Key::Type::STRING)
          {
            modelNode = FindChildModelNodeByName(motionIndex.GetModelNodeId().stringKey);
          }
          else if(motionIndex.GetModelNodeId().type == Property::Key::Type::INDEX)
          {
            // TODO : Not implement yet.
          }

          if(modelNode)
          {
            KeyFrames keyFrames = motionValue.GetKeyFrames();

            if(keyFrames)
            {
              // Try to use index first. If failed, try to use name
              Property::Index animatedPropertyIndex = motionIndex.GetPropertyIndex(modelNode);
              if(animatedPropertyIndex != Property::INVALID_INDEX)
              {
                if(DALI_UNLIKELY(!animation))
                {
                  animation = Animation::New(motionData.GetDuration());
                }
                animation.AnimateBetween(Dali::Property(modelNode, animatedPropertyIndex), keyFrames);
              }
              else
              {
                std::string    animatedPropertyName = motionIndex.GetPropertyName(modelNode);
                Dali::Property property(modelNode, animatedPropertyName);
                if(property.propertyIndex != Property::INVALID_INDEX)
                {
                  if(DALI_UNLIKELY(!animation))
                  {
                    animation = Animation::New(motionData.GetDuration());
                  }
                  animation.AnimateBetween(property, keyFrames);
                }
              }
            }
          }
        }
        else
        {
          Scene3D::BlendShapeIndex blendShapeIndex = Scene3D::BlendShapeIndex::DownCast(motionIndex);
          if(blendShapeIndex && blendShapeIndex.GetBlendShapeId().type == Property::Key::Type::STRING)
          {
            // Special case : For BlendShapeIndex that doesn't have ModelNodeId and has string BlendShapeId,
            // we need to animate all kind of blendshapes

            KeyFrames keyFrames = motionValue.GetKeyFrames();

            if(keyFrames)
            {
              std::vector<Scene3D::ModelNode> modelNodes;
              RetrieveModelNodesByBlendShapeName(blendShapeIndex.GetBlendShapeId().stringKey, modelNodes);

              for(auto& modelNode : modelNodes)
              {
                // Try to use index first. If failed, try to use name
                Property::Index animatedPropertyIndex = motionIndex.GetPropertyIndex(modelNode);
                if(animatedPropertyIndex != Property::INVALID_INDEX)
                {
                  if(DALI_UNLIKELY(!animation))
                  {
                    animation = Animation::New(motionData.GetDuration());
                  }
                  animation.AnimateBetween(Dali::Property(modelNode, animatedPropertyIndex), keyFrames);
                }
                else
                {
                  std::string    animatedPropertyName = motionIndex.GetPropertyName(modelNode);
                  Dali::Property property(modelNode, animatedPropertyName);

                  if(property.propertyIndex != Property::INVALID_INDEX)
                  {
                    if(DALI_UNLIKELY(!animation))
                    {
                      animation = Animation::New(motionData.GetDuration());
                    }
                    animation.AnimateBetween(property, keyFrames);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  return animation;
}

void Model::SetMotionData(Scene3D::MotionData motionData)
{
  // TODO : Need to collect duplicated codes with GenerateMotionDataAnimation()

  if(motionData)
  {
    const uint32_t motionCount = motionData.GetMotionCount();
    for(uint32_t i = 0u; i < motionCount; ++i)
    {
      auto motionIndex = motionData.GetIndex(i);
      auto motionValue = motionData.GetValue(i);
      if(motionIndex && motionValue)
      {
        if(motionIndex.GetModelNodeId() != Property::INVALID_KEY)
        {
          Scene3D::ModelNode modelNode;
          if(motionIndex.GetModelNodeId().type == Property::Key::Type::STRING)
          {
            modelNode = FindChildModelNodeByName(motionIndex.GetModelNodeId().stringKey);
          }
          else if(motionIndex.GetModelNodeId().type == Property::Key::Type::INDEX)
          {
            // TODO : Not implement yet.
          }

          if(modelNode)
          {
            Property::Value value = motionValue.GetPropertyValue();

            if(value.GetType() != Property::Type::NONE)
            {
              // Try to use index first. If failed, try to use name
              Property::Index propertyIndex = motionIndex.GetPropertyIndex(modelNode);
              if(propertyIndex != Property::INVALID_INDEX)
              {
                modelNode.SetProperty(propertyIndex, value);
              }
              else
              {
                std::string    propertyName = motionIndex.GetPropertyName(modelNode);
                Dali::Property property(modelNode, propertyName);
                if(property.propertyIndex != Property::INVALID_INDEX)
                {
                  modelNode.SetProperty(property.propertyIndex, value);
                }
              }
            }
          }
        }
        else
        {
          Scene3D::BlendShapeIndex blendShapeIndex = Scene3D::BlendShapeIndex::DownCast(motionIndex);
          if(blendShapeIndex && blendShapeIndex.GetBlendShapeId().type == Property::Key::Type::STRING)
          {
            // Special case : For BlendShapeIndex that doesn't have ModelNodeId and has string BlendShapeId,
            // we need to animate all kind of blendshapes

            Property::Value value = motionValue.GetPropertyValue();

            if(value.GetType() != Property::Type::NONE)
            {
              std::vector<Scene3D::ModelNode> modelNodes;
              RetrieveModelNodesByBlendShapeName(blendShapeIndex.GetBlendShapeId().stringKey, modelNodes);

              for(auto& modelNode : modelNodes)
              {
                // Try to use index first. If failed, try to use name
                Property::Index propertyIndex = motionIndex.GetPropertyIndex(modelNode);
                if(propertyIndex != Property::INVALID_INDEX)
                {
                  modelNode.SetProperty(propertyIndex, value);
                }
                else
                {
                  std::string    propertyName = motionIndex.GetPropertyName(modelNode);
                  Dali::Property property(modelNode, propertyName);
                  if(property.propertyIndex != Property::INVALID_INDEX)
                  {
                    modelNode.SetProperty(property.propertyIndex, value);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

void Model::CastShadow(bool castShadow)
{
  mIsShadowCasting = castShadow;
  ModelNodeTreeUtility::UpdateCastShadowRecursively(mModelRoot, mIsShadowCasting);
}

bool Model::IsShadowCasting() const
{
  return mIsShadowCasting;
}

void Model::ReceiveShadow(bool receiveShadow)
{
  mIsShadowReceiving = receiveShadow;
  ModelNodeTreeUtility::UpdateReceiveShadowRecursively(mModelRoot, mIsShadowReceiving);
}

bool Model::IsShadowReceiving() const
{
  return mIsShadowReceiving;
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void Model::OnInitialize()
{
  Collidable::SetCollidableActor(Scene3D::Model::DownCast(Self()));

  // Make ParentOrigin as Center.
  Self().SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  mDefaultDiffuseTexture  = ImageResourceLoader::GetEmptyTextureWhiteRGB();
  mDefaultSpecularTexture = ImageResourceLoader::GetEmptyTextureWhiteRGB();
}

void Model::OnSceneConnection(int depth)
{
  Actor parent = Self().GetParent();
  while(parent)
  {
    // If this Model has parent SceneView and the its ShaderManager is same with privious ShaderManager,
    // this Model don't need to update shader.
    Scene3D::SceneView sceneView = Scene3D::SceneView::DownCast(parent);
    if(sceneView)
    {
      mParentSceneView = sceneView;
      GetImpl(sceneView).RegisterSceneItem(this);
      Scene3D::Loader::ShaderManagerPtr shaderManager = GetImpl(sceneView).GetShaderManager();
      if(mShaderManager != shaderManager)
      {
        mShaderManager = shaderManager;
        ModelNodeTreeUtility::UpdateShaderRecursively(mModelRoot, mShaderManager);
      }
      break;
    }
    parent = parent.GetParent();
  }

  // Model can be added on Dali::Scene directly without SceneView.
  // So, Model's mShaderManager and shaders of child ModelNodes are needed to be reset when this Model has not parent SceneView.
  Scene3D::SceneView parentSceneView = mParentSceneView.GetHandle();
  if(!parentSceneView)
  {
    mShaderManager = new Dali::Scene3D::Loader::ShaderManager();
    ModelNodeTreeUtility::UpdateShaderRecursively(mModelRoot, mShaderManager);
  }

  if(!mModelLoadTask && !mModelResourceReady && !mModelUrl.empty())
  {
    // Request model load only if we setup url.
    if(ModelCacheManager::Get())
    {
      ModelCacheManager::Get().ReferenceModelCache(mModelUrl);
    }
    mModelLoadTask = new ModelLoadTask(mModelUrl, mResourceDirectoryUrl, MakeCallback(this, &Model::OnModelLoadComplete));
    Dali::AsyncTaskManager::Get().AddTask(mModelLoadTask);
  }

  // If diffuse and specular url is not valid, IBL does not need to be loaded.
  if(!mDiffuseIblUrl.empty() && !mSpecularIblUrl.empty())
  {
    SetImageBasedLightSource(mDiffuseIblUrl, mSpecularIblUrl, mIblScaleFactor);
  }

  NotifyResourceReady();

  mSizeNotification = Self().AddPropertyNotification(Actor::Property::SIZE, StepCondition(SIZE_STEP_CONDITION));
  mSizeNotification.NotifySignal().Connect(this, &Model::OnSizeNotification);
  Control::OnSceneConnection(depth);
}

void Model::OnSceneDisconnection()
{
  // If mParentSceneView is still onScene, that means this model
  // is disconnected from mParentSceneView's sub tree.
  // So, Unregister this Model from SceneView.
  Scene3D::SceneView sceneView = mParentSceneView.GetHandle();
  if(sceneView && sceneView.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE))
  {
    GetImpl(sceneView).UnregisterSceneItem(this);
    mParentSceneView.Reset();
  }

  mSizeNotification.NotifySignal().Disconnect(this, &Model::OnSizeNotification);
  Self().RemovePropertyNotification(mSizeNotification);
  mSizeNotification.Reset();

  Control::OnSceneDisconnection();
}

void Model::OnSizeSet(const Vector3& size)
{
  ScaleModel(false);
}

Vector3 Model::GetNaturalSize()
{
  if(!mModelRoot)
  {
    DALI_LOG_ERROR("Model is still not loaded.\n");
    return Vector3::ZERO;
  }

  return mNaturalSize;
}

float Model::GetHeightForWidth(float width)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  return Control::GetHeightForWidth(width) + padding.top + padding.bottom;
}

float Model::GetWidthForHeight(float height)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  return Control::GetWidthForHeight(height) + padding.start + padding.end;
}

void Model::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  Control::OnRelayout(size, container);
  ScaleModel(false);
}

bool Model::IsResourceReady() const
{
  return mModelResourceReady && mIblDiffuseResourceReady && mIblSpecularResourceReady;
}

void Model::CreateModelRoot()
{
  mModelRoot = Scene3D::ModelNode::New();
  mModelRoot.SetProperty(Actor::Property::COLOR_MODE, ColorMode::USE_OWN_MULTIPLY_PARENT_COLOR);
  mModelRoot.SetProperty(Dali::Actor::Property::SCALE, Y_DIRECTION);
  mModelRoot.SetProperty(Dali::Actor::Property::SENSITIVE, mModelChildrenSensitive);
  mModelRoot.SetProperty(Dali::Actor::Property::KEYBOARD_FOCUSABLE, mModelChildrenFocusable);
  mModelRoot.SetProperty(Dali::DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN, mModelChildrenFocusable);
  Self().Add(mModelRoot);
}

void Model::ScaleModel(bool useCurrentSize)
{
  if(!mModelRoot)
  {
    return;
  }

  float   scale = 1.0f;
  Vector3 size  = (useCurrentSize) ? Self().GetCurrentProperty<Vector3>(Dali::Actor::Property::SIZE) : Self().GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  if(size.x > 0.0f && size.y > 0.0f)
  {
    scale = MAXFLOAT;
    scale = std::min(size.x / mNaturalSize.x, scale);
    scale = std::min(size.y / mNaturalSize.y, scale);
  }
  // Models in glTF and dli are defined as right hand coordinate system.
  // DALi uses left hand coordinate system. Scaling negative is for change winding order.
  mModelRoot.SetProperty(Dali::Actor::Property::SCALE, Y_DIRECTION * scale);
}

void Model::FitModelPosition()
{
  if(!mModelRoot)
  {
    return;
  }
  // Loaded model pivot is not the model center.
  mModelRoot.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mModelRoot.SetProperty(Dali::Actor::Property::ANCHOR_POINT, Vector3::ONE - mModelPivot);
}

void Model::UpdateImageBasedLightTexture()
{
  Dali::Texture currentDiffuseTexture          = (mDiffuseTexture && mSpecularTexture) ? mDiffuseTexture : mSceneDiffuseTexture;
  Dali::Texture currentSpecularTexture         = (mDiffuseTexture && mSpecularTexture) ? mSpecularTexture : mSceneSpecularTexture;
  float         currentIblScaleFactor          = (mDiffuseTexture && mSpecularTexture) ? mIblScaleFactor : mSceneIblScaleFactor;
  uint32_t      currentIblSpecularMipmapLevels = (mDiffuseTexture && mSpecularTexture) ? mSpecularMipmapLevels : mSceneSpecularMipmapLevels;

  if(!currentDiffuseTexture || !currentSpecularTexture)
  {
    currentDiffuseTexture          = mDefaultDiffuseTexture;
    currentSpecularTexture         = mDefaultSpecularTexture;
    currentIblScaleFactor          = Dali::Scene3D::Loader::EnvironmentDefinition::GetDefaultIntensity();
    currentIblSpecularMipmapLevels = 1u;
  }

  ModelNodeTreeUtility::UpdateImageBasedLightTextureRecursively(mModelRoot, currentDiffuseTexture, currentSpecularTexture, currentIblScaleFactor, currentIblSpecularMipmapLevels);
}

void Model::UpdateImageBasedLightScaleFactor()
{
  if((!mDiffuseTexture || !mSpecularTexture) &&
     (!mSceneDiffuseTexture || !mSceneSpecularTexture))
  {
    return;
  }

  float currentIblScaleFactor = (mDiffuseTexture && mSpecularTexture) ? mIblScaleFactor : mSceneIblScaleFactor;
  ModelNodeTreeUtility::UpdateImageBasedLightScaleFactorRecursively(mModelRoot, currentIblScaleFactor);
}

void Model::ApplyCameraTransform(Dali::CameraActor camera) const
{
  Vector3    selfPosition    = Self().GetProperty<Vector3>(Actor::Property::POSITION);
  Quaternion selfOrientation = Self().GetProperty<Quaternion>(Actor::Property::ORIENTATION);
  Vector3    selfScale       = Self().GetProperty<Vector3>(Actor::Property::SCALE);

  Vector3    cameraPosition    = camera.GetProperty<Vector3>(Actor::Property::POSITION);
  Quaternion cameraOrientation = camera.GetProperty<Quaternion>(Actor::Property::ORIENTATION);
  Vector3    cameraScale       = camera.GetProperty<Vector3>(Actor::Property::SCALE);

  // Models in glTF and dli are defined as right hand coordinate system.
  // DALi uses left hand coordinate system. Scaling negative is for change winding order.
  if(!Dali::Equals(Y_DIRECTION.Dot(Vector3::YAXIS), 1.0f))
  {
    // Reflect by XZ plane
    cameraPosition.y = -cameraPosition.y;
    Quaternion yDirectionQuaternion;
    yDirectionQuaternion.mVector = Vector3::YAXIS;
    // Reflect orientation
    cameraOrientation = yDirectionQuaternion * cameraOrientation * yDirectionQuaternion;
  }

  Vector3    resultPosition;
  Quaternion resultOrientation;
  Vector3    resultScale;

  Matrix selfMatrix(false);
  Matrix cameraMatrix(false);
  Matrix resultMatrix(false);
  selfMatrix.SetTransformComponents(selfScale, selfOrientation, selfPosition);
  cameraMatrix.SetTransformComponents(cameraScale, cameraOrientation, cameraPosition);
  Matrix::Multiply(resultMatrix, cameraMatrix, selfMatrix);
  resultMatrix.GetTransformComponents(resultPosition, resultOrientation, resultScale);

  camera.SetProperty(Actor::Property::POSITION, resultPosition);
  camera.SetProperty(Actor::Property::ORIENTATION, resultOrientation);
  camera.SetProperty(Actor::Property::SCALE, resultScale);
}

void Model::NotifyShadowMapTexture(Dali::Texture shadowMapTexture)
{
  if(mShadowMapTexture != shadowMapTexture)
  {
    mShadowMapTexture = shadowMapTexture;
    ModelNodeTreeUtility::UpdateShadowMapTextureRecursively(mModelRoot, mShadowMapTexture);
  }
}

void Model::NotifyImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float scaleFactor, uint32_t specularMipmapLevels)
{
  if(mSceneDiffuseTexture != diffuseTexture || mSceneSpecularTexture != specularTexture)
  {
    mSceneDiffuseTexture       = diffuseTexture;
    mSceneSpecularTexture      = specularTexture;
    mSceneIblScaleFactor       = scaleFactor;
    mSceneSpecularMipmapLevels = specularMipmapLevels;
    // If Model IBL is not set, use SceneView's IBL.
    if(!mDiffuseTexture || !mSpecularTexture)
    {
      UpdateImageBasedLightTexture();
    }
  }
}

void Model::NotifyImageBasedLightScaleFactor(float scaleFactor)
{
  mSceneIblScaleFactor = scaleFactor;
  if(mSceneDiffuseTexture && mSceneSpecularTexture)
  {
    UpdateImageBasedLightScaleFactor();
  }
}

void Model::OnModelLoadComplete()
{
  IntrusivePtr<Model> self = this; // Keep reference until this API finished

  if(!mModelLoadTask->HasSucceeded())
  {
    ResetResourceTasks();

    if(ModelCacheManager::Get() && !mModelUrl.empty())
    {
      ModelCacheManager::Get().UnreferenceModelCache(mModelUrl);
    }

    EmitLoadCompletedSignal(false);

    return;
  }

  if(!mModelRoot)
  {
    CreateModelRoot();
  }
  CreateModel();

  auto& resources = mModelLoadTask->GetResources();
  auto& scene     = mModelLoadTask->GetScene();
  CreateAnimations(scene);
  ResetCameraParameters();
  if(!resources.mEnvironmentMaps.empty())
  {
    if(resources.mEnvironmentMaps.front().second.mDiffuse)
    {
      mDefaultDiffuseTexture = resources.mEnvironmentMaps.front().second.mDiffuse;
    }
    if(resources.mEnvironmentMaps.front().second.mSpecular)
    {
      mDefaultSpecularTexture = resources.mEnvironmentMaps.front().second.mSpecular;
    }
  }

  if(mShadowMapTexture)
  {
    ModelNodeTreeUtility::UpdateShadowMapTextureRecursively(mModelRoot, mShadowMapTexture);
  }
  UpdateImageBasedLightTexture();
  UpdateImageBasedLightScaleFactor();
  Self().SetProperty(Dali::Actor::Property::ANCHOR_POINT, Vector3(mModelPivot.x, 1.0f - mModelPivot.y, mModelPivot.z));

  mModelResourceReady = true;
  ResetResourceTask(mModelLoadTask);
  NotifyResourceReady();

  EmitLoadCompletedSignal(true);
}

void Model::OnIblDiffuseLoadComplete()
{
  mDiffuseTexture = mIblDiffuseLoadTask->GetLoadedTexture();
  ResetResourceTask(mIblDiffuseLoadTask);
  mIblDiffuseResourceReady = true;
  if(mIblDiffuseResourceReady && mIblSpecularResourceReady)
  {
    OnIblLoadComplete();
  }
}

void Model::OnIblSpecularLoadComplete()
{
  mSpecularTexture      = mIblSpecularLoadTask->GetLoadedTexture();
  mSpecularMipmapLevels = mIblSpecularLoadTask->GetMipmapLevels();
  ResetResourceTask(mIblSpecularLoadTask);
  mIblSpecularResourceReady = true;
  if(mIblDiffuseResourceReady && mIblSpecularResourceReady)
  {
    OnIblLoadComplete();
  }
}

void Model::OnIblLoadComplete()
{
  UpdateImageBasedLightTexture();
  NotifyResourceReady();
}

void Model::OnSizeNotification(Dali::PropertyNotification& source)
{
  ScaleModel(true);
}

void Model::ResetResourceTasks()
{
  if(!Dali::Adaptor::IsAvailable())
  {
    return;
  }
  ResetResourceTask(mModelLoadTask);
  ResetResourceTask(mIblDiffuseLoadTask);
  ResetResourceTask(mIblSpecularLoadTask);
}

void Model::NotifyResourceReady()
{
  if(!IsResourceReady())
  {
    return;
  }
  Control::SetResourceReady();
}

void Model::CreateModel()
{
  BoundingVolume                    AABB;
  auto&                             resources       = mModelLoadTask->GetResources();
  auto&                             scene           = mModelLoadTask->GetScene();
  auto&                             resourceChoices = mModelLoadTask->GetResourceChoices();
  Dali::Scene3D::Loader::Transforms xforms{Dali::Scene3D::Loader::MatrixStack{}, Dali::Scene3D::Loader::ViewProjection{}};

  Dali::Scene3D::Loader::NodeDefinition::CreateParams nodeParams{resources, xforms, mShaderManager, {}, {}, {}};

  // Generate Dali handles from resource bundle. Note that we generate all scene's resouce immediatly.
  resources.GenerateResources();
  for(auto iRoot : scene.GetRoots())
  {
    if(auto modelNode = scene.CreateNodes(iRoot, resourceChoices, nodeParams))
    {
      scene.ConfigureSkinningShaders(resources, modelNode, std::move(nodeParams.mSkinnables));
      ConfigureBlendShapeShaders(resources, scene, modelNode, std::move(nodeParams.mBlendshapeRequests));

      scene.ApplyConstraints(modelNode, std::move(nodeParams.mConstrainables));

      mModelRoot.Add(modelNode);
    }

    AddModelTreeToAABB(AABB, scene, resourceChoices, iRoot, nodeParams, Matrix::IDENTITY);
  }

  UpdateBlendShapeNodeMap();

  mNaturalSize = AABB.CalculateSize();
  mModelPivot  = AABB.CalculatePivot();
  mModelRoot.SetProperty(Dali::Actor::Property::SIZE, mNaturalSize);
  Vector3 controlSize = Self().GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  if(Dali::EqualsZero(controlSize.x) || Dali::EqualsZero(controlSize.y))
  {
    Self().SetProperty(Dali::Actor::Property::SIZE, mNaturalSize);
  }
  FitModelPosition();
  ScaleModel(false);
}

void Model::CreateAnimations(Dali::Scene3D::Loader::SceneDefinition& scene)
{
  mAnimations.clear();
  if(!mModelLoadTask->GetAnimations().empty())
  {
    auto getActor = [&](const Scene3D::Loader::AnimatedProperty& property) {
      if(property.mNodeIndex == Scene3D::Loader::INVALID_INDEX)
      {
        return mModelRoot.FindChildByName(property.mNodeName);
      }
      auto* node = scene.GetNode(property.mNodeIndex);
      if(node == nullptr)
      {
        return Dali::Actor();
      }
      return mModelRoot.FindChildById(node->mNodeId);
    };

    for(auto&& animation : mModelLoadTask->GetAnimations())
    {
      Dali::Animation anim = animation.ReAnimate(getActor);
      mAnimations.push_back({animation.GetName(), anim});
    }
  }
}

void Model::ResetCameraParameters()
{
  mCameraParameters.clear();
  if(!mModelLoadTask->GetCameras().empty())
  {
    // Copy camera parameters.
    std::copy(mModelLoadTask->GetCameras().begin(), mModelLoadTask->GetCameras().end(), std::back_inserter(mCameraParameters));
  }
}

void Model::UpdateBlendShapeNodeMap()
{
  // Remove privous node map
  mBlendShapeModelNodeMap.clear();

  UpdateBlendShapeNodeMapRecursively(mBlendShapeModelNodeMap, mModelRoot);
}

} // namespace Internal
} // namespace Scene3D
} // namespace Dali
