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
#include <dali-scene3d/internal/controls/model/model-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <filesystem>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/scene-view/scene-view-impl.h>
#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/loader/animation-definition.h>
#include <dali-scene3d/public-api/loader/camera-parameters.h>
#include <dali-scene3d/public-api/loader/cube-map-loader.h>
#include <dali-scene3d/public-api/loader/dli-loader.h>
#include <dali-scene3d/public-api/loader/gltf2-loader.h>
#include <dali-scene3d/public-api/loader/light-parameters.h>
#include <dali-scene3d/public-api/loader/load-result.h>
#include <dali-scene3d/public-api/loader/node-definition.h>
#include <dali-scene3d/public-api/loader/scene-definition.h>
#include <dali-scene3d/public-api/loader/shader-definition-factory.h>

using namespace Dali;

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
namespace
{
BaseHandle Create()
{
  return Scene3D::Model::New(std::string());
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::Model, Toolkit::Control, Create);
DALI_TYPE_REGISTRATION_END()

static constexpr uint32_t OFFSET_FOR_DIFFUSE_CUBE_TEXTURE  = 2u;
static constexpr uint32_t OFFSET_FOR_SPECULAR_CUBE_TEXTURE = 1u;

static constexpr Vector3 Y_DIRECTION(1.0f, -1.0f, 1.0f);

static constexpr bool DEFAULT_MODEL_CHILDREN_SENSITIVE = false;

static constexpr std::string_view KTX_EXTENSION  = ".ktx";
static constexpr std::string_view OBJ_EXTENSION  = ".obj";
static constexpr std::string_view GLTF_EXTENSION = ".gltf";
static constexpr std::string_view DLI_EXTENSION  = ".dli";

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
      if(pointMin[i] == pointMax[i])
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

} // anonymous namespace

Model::Model(const std::string& modelUrl, const std::string& resourceDirectoryUrl)
: Control(ControlBehaviour(DISABLE_SIZE_NEGOTIATION | DISABLE_STYLE_CHANGE_SIGNALS)),
  mModelUrl(modelUrl),
  mResourceDirectoryUrl(resourceDirectoryUrl),
  mModelRoot(),
  mNaturalSize(Vector3::ZERO),
  mModelPivot(AnchorPoint::CENTER),
  mIblScaleFactor(1.0f),
  mModelChildrenSensitive(DEFAULT_MODEL_CHILDREN_SENSITIVE),
  mModelResourceReady(false),
  mIBLResourceReady(true)
{
}

Model::~Model()
{
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

const Actor Model::GetModelRoot() const
{
  return mModelRoot;
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

void Model::SetImageBasedLightSource(const std::string& diffuseUrl, const std::string& specularUrl, float scaleFactor)
{
  mIBLResourceReady       = false;
  Texture diffuseTexture  = Dali::Scene3D::Loader::LoadCubeMap(diffuseUrl);
  Texture specularTexture = Dali::Scene3D::Loader::LoadCubeMap(specularUrl);
  SetImageBasedLightTexture(diffuseTexture, specularTexture, scaleFactor);
  mIBLResourceReady = true;

  // If Model resource is already ready, then set resource ready.
  // If Model resource is still not ready, wait for model resource ready.
  if(IsResourceReady())
  {
    SetResourceReady(false);
  }
}

void Model::SetImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float scaleFactor)
{
  if(diffuseTexture && specularTexture)
  {
    if(mDiffuseTexture != diffuseTexture || mSpecularTexture != specularTexture)
    {
      mDiffuseTexture  = diffuseTexture;
      mSpecularTexture = specularTexture;
      UpdateImageBasedLightTexture();
    }
    if(mIblScaleFactor != scaleFactor)
    {
      mIblScaleFactor = scaleFactor;
      UpdateImageBasedLightScaleFactor();
    }
  }
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

///////////////////////////////////////////////////////////
//
// Private methods
//

void Model::OnSceneConnection(int depth)
{
  if(!mModelRoot)
  {
    LoadModel();
  }

  Actor parent = Self().GetParent();
  while(parent)
  {
    Scene3D::SceneView sceneView = Scene3D::SceneView::DownCast(parent);
    if(sceneView)
    {
      GetImpl(sceneView).RegisterModel(Scene3D::Model::DownCast(Self()));
      mParentSceneView = sceneView;
      break;
    }
    parent = parent.GetParent();
  }

  Control::OnSceneConnection(depth);
}

void Model::OnSceneDisconnection()
{
  Scene3D::SceneView sceneView = mParentSceneView.GetHandle();
  if(sceneView)
  {
    GetImpl(sceneView).UnregisterModel(Scene3D::Model::DownCast(Self()));
    mParentSceneView.Reset();
  }
  Control::OnSceneDisconnection();
}

Vector3 Model::GetNaturalSize()
{
  if(!mModelRoot)
  {
    LoadModel();
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
  ScaleModel();
}

bool Model::IsResourceReady() const
{
  return mModelResourceReady && mIBLResourceReady;
}

void Model::LoadModel()
{
  std::filesystem::path modelUrl(mModelUrl);
  if(mResourceDirectoryUrl.empty())
  {
    mResourceDirectoryUrl = std::string(modelUrl.parent_path()) + "/";
  }
  std::string extension = modelUrl.extension();
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

  Dali::Scene3D::Loader::ResourceBundle::PathProvider pathProvider = [&](Dali::Scene3D::Loader::ResourceType::Value type) {
    return mResourceDirectoryUrl;
  };

  Dali::Scene3D::Loader::ResourceBundle                        resources;
  Dali::Scene3D::Loader::SceneDefinition                       scene;
  std::vector<Dali::Scene3D::Loader::AnimationGroupDefinition> animGroups;
  std::vector<Dali::Scene3D::Loader::CameraParameters>         cameraParameters;
  std::vector<Dali::Scene3D::Loader::LightParameters>          lights;

  std::vector<Dali::Scene3D::Loader::AnimationDefinition> animations;
  animations.clear();

  Dali::Scene3D::Loader::LoadResult output{resources, scene, animations, animGroups, cameraParameters, lights};

  if(extension == DLI_EXTENSION)
  {
    Dali::Scene3D::Loader::DliLoader              loader;
    Dali::Scene3D::Loader::DliLoader::InputParams input{
      pathProvider(Dali::Scene3D::Loader::ResourceType::Mesh),
      nullptr,
      {},
      {},
      nullptr,
      {}};
    Dali::Scene3D::Loader::DliLoader::LoadParams loadParams{input, output};
    if(!loader.LoadScene(mModelUrl, loadParams))
    {
      Dali::Scene3D::Loader::ExceptionFlinger(ASSERT_LOCATION) << "Failed to load scene from '" << mModelUrl << "': " << loader.GetParseError();
    }
  }
  else if(extension == GLTF_EXTENSION)
  {
    Dali::Scene3D::Loader::ShaderDefinitionFactory sdf;
    sdf.SetResources(resources);
    Dali::Scene3D::Loader::LoadGltfScene(mModelUrl, sdf, output);

    resources.mEnvironmentMaps.push_back({});
  }
  else
  {
    DALI_LOG_ERROR("Unsupported model type.\n");
  }

  Dali::Scene3D::Loader::Transforms                   xforms{Dali::Scene3D::Loader::MatrixStack{}, Dali::Scene3D::Loader::ViewProjection{}};
  Dali::Scene3D::Loader::NodeDefinition::CreateParams nodeParams{resources, xforms, {}, {}, {}};
  Dali::Scene3D::Loader::Customization::Choices       choices;

  mModelRoot = Actor::New();

  BoundingVolume AABB;
  for(auto iRoot : scene.GetRoots())
  {
    auto resourceRefs = resources.CreateRefCounter();
    scene.CountResourceRefs(iRoot, choices, resourceRefs);
    resources.CountEnvironmentReferences(resourceRefs);

    resources.LoadResources(resourceRefs, pathProvider);

    // glTF Mesh is defined in right hand coordinate system, with positive Y for Up direction.
    // Because DALi uses left hand system, Y direciton will be flipped for environment map sampling.
    for(auto&& env : resources.mEnvironmentMaps)
    {
      env.first.mYDirection = Y_DIRECTION;
    }

    if(auto actor = scene.CreateNodes(iRoot, choices, nodeParams))
    {
      scene.ConfigureSkeletonJoints(iRoot, resources.mSkeletons, actor);
      scene.ConfigureSkinningShaders(resources, actor, std::move(nodeParams.mSkinnables));
      ConfigureBlendShapeShaders(resources, scene, actor, std::move(nodeParams.mBlendshapeRequests));

      scene.ApplyConstraints(actor, std::move(nodeParams.mConstrainables));

      mModelRoot.Add(actor);
    }

    AddModelTreeToAABB(AABB, scene, choices, iRoot, nodeParams, Matrix::IDENTITY);
  }

  if(!animations.empty())
  {
    auto getActor = [&](const std::string& name) {
      return mModelRoot.FindChildByName(name);
    };

    mAnimations.clear();
    for(auto&& animation : animations)
    {
      Dali::Animation anim = animation.ReAnimate(getActor);

      mAnimations.push_back({animation.mName, anim});
    }
  }

  mRenderableActors.clear();
  CollectRenderableActor(mModelRoot);
  UpdateImageBasedLightTexture();
  UpdateImageBasedLightScaleFactor();

  mNaturalSize = AABB.CalculateSize();
  mModelPivot  = AABB.CalculatePivot();
  mModelRoot.SetProperty(Dali::Actor::Property::SIZE, mNaturalSize);
  Vector3 controlSize = Self().GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  if(controlSize.x == 0.0f || controlSize.y == 0.0f)
  {
    Self().SetProperty(Dali::Actor::Property::SIZE, mNaturalSize);
  }

  FitModelPosition();
  ScaleModel();

  mModelRoot.SetProperty(Dali::Actor::Property::SENSITIVE, mModelChildrenSensitive);

  Self().Add(mModelRoot);

  Self().SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Self().SetProperty(Dali::Actor::Property::ANCHOR_POINT, Vector3(mModelPivot.x, 1.0f - mModelPivot.y, mModelPivot.z));

  mModelResourceReady = true;

  Control::SetResourceReady(false);
}

void Model::ScaleModel()
{
  if(mModelRoot)
  {
    float   scale = 1.0f;
    Vector3 size  = Self().GetProperty<Vector3>(Dali::Actor::Property::SIZE);
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
}

void Model::FitModelPosition()
{
  if(mModelRoot)
  {
    // Loaded model pivot is not the model center.
    mModelRoot.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mModelRoot.SetProperty(Dali::Actor::Property::ANCHOR_POINT, Vector3::ONE - mModelPivot);
  }
}

void Model::CollectRenderableActor(Actor actor)
{
  uint32_t rendererCount = actor.GetRendererCount();
  if(rendererCount)
  {
    mRenderableActors.push_back(actor);
  }

  uint32_t childrenCount = actor.GetChildCount();
  for(uint32_t i = 0; i < childrenCount; ++i)
  {
    CollectRenderableActor(actor.GetChildAt(i));
  }
}

void Model::UpdateImageBasedLightTexture()
{
  if(!mDiffuseTexture || !mSpecularTexture)
  {
    return;
  }

  for(auto&& actor : mRenderableActors)
  {
    Actor renderableActor = actor.GetHandle();
    if(renderableActor)
    {
      uint32_t rendererCount = renderableActor.GetRendererCount();
      for(uint32_t i = 0; i < rendererCount; ++i)
      {
        Dali::Renderer renderer = renderableActor.GetRendererAt(i);
        if(renderer)
        {
          Dali::TextureSet textures = renderer.GetTextures();
          if(textures)
          {
            uint32_t textureCount = textures.GetTextureCount();
            // EnvMap requires at least 2 texture, diffuse and specular
            if(textureCount > 2u)
            {
              textures.SetTexture(textureCount - OFFSET_FOR_DIFFUSE_CUBE_TEXTURE, mDiffuseTexture);
              textures.SetTexture(textureCount - OFFSET_FOR_SPECULAR_CUBE_TEXTURE, mSpecularTexture);
            }
          }
        }
      }
    }
  }
}

void Model::UpdateImageBasedLightScaleFactor()
{
  if(!mDiffuseTexture || !mSpecularTexture)
  {
    return;
  }
  for(auto&& actor : mRenderableActors)
  {
    Actor renderableActor = actor.GetHandle();
    if(renderableActor)
    {
      renderableActor.RegisterProperty(Dali::Scene3D::Loader::NodeDefinition::GetIblScaleFactorUniformName().data(), mIblScaleFactor);
    }
  }
}

} // namespace Internal
} // namespace Scene3D
} // namespace Dali
