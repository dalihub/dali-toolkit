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
#include <dali-scene3d/internal/controls/panel/panel-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/constraint-integ.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <filesystem>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/common/image-resource-loader.h>
#include <dali-scene3d/internal/controls/scene-view/scene-view-impl.h>
#include <dali-scene3d/internal/event/collider-mesh-processor.h>
#include <dali-scene3d/internal/light/light-impl.h>
#include <dali-scene3d/internal/model-components/model-node-impl.h>
#include <dali-scene3d/internal/model-components/model-node-tree-utility.h>
#include <dali-scene3d/public-api/common/scene3d-constraint-tag-ranges.h>
#include <dali-scene3d/public-api/controls/panel/panel.h>
#include <dali-scene3d/public-api/loader/light-parameters.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>

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
  return Scene3D::Panel::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::Panel, Toolkit::Control, Create);
DALI_PROPERTY_REGISTRATION(Scene3D, Panel, "Transparent", BOOLEAN, TRANSPARENT)
DALI_PROPERTY_REGISTRATION(Scene3D, Panel, "DoubleSided", BOOLEAN, DOUBLE_SIDED)
DALI_PROPERTY_REGISTRATION(Scene3D, Panel, "UseBackFacePlane", BOOLEAN, USE_BACK_FACE_PLANE)
DALI_PROPERTY_REGISTRATION(Scene3D, Panel, "BackFacePlaneColor", VECTOR3, BACK_FACE_PLANE_COLOR)
DALI_TYPE_REGISTRATION_END()

static constexpr Vector3 Y_DIRECTION(1.0f, -1.0f, 1.0f);

static constexpr int32_t PANEL_ORDER_INDEX = 90; // It should be lower value than SceneView's first RenderTask's value.

static constexpr uint32_t PANEL_CONSTRAINT_TAG = Dali::Scene3D::ConstraintTagRanges::SCENE3D_CONSTRAINT_TAG_START + 300;

Dali::Geometry CreatePlaneGeometry(bool flip = false)
{
  Property::Map texturedQuadVertexFormat;
  texturedQuadVertexFormat["aPosition"]    = Property::VECTOR3;
  texturedQuadVertexFormat["aNormal"]      = Property::VECTOR3;
  texturedQuadVertexFormat["aTexCoord"]    = Property::VECTOR2;
  texturedQuadVertexFormat["aVertexColor"] = Property::VECTOR4;

  VertexBuffer vertexData   = VertexBuffer::New(texturedQuadVertexFormat);
  const float  halfQuadSize = .5f;
  struct TexturedQuadVertex
  {
    Vector3 position;
    Vector3 normal;
    Vector2 textureCoordinates;
    Vector4 color;
  };
  TexturedQuadVertex texturedQuadVertexData[4] = {
    {Vector3(-halfQuadSize, -halfQuadSize, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(flip ? 1.0f : 0.0f, 0.0f), Vector4::ONE},
    {Vector3(halfQuadSize, -halfQuadSize, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(flip ? 0.0f : 1.0f, 0.0f), Vector4::ONE},
    {Vector3(-halfQuadSize, halfQuadSize, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(flip ? 1.0f : 0.0f, 1.0f), Vector4::ONE},
    {Vector3(halfQuadSize, halfQuadSize, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector2(flip ? 0.0f : 1.0f, 1.0f), Vector4::ONE}};
  vertexData.SetData(texturedQuadVertexData, 4);

  Geometry geometry = Geometry::New();
  geometry.AddVertexBuffer(vertexData);
  unsigned short indexData[6] = {0, 1, 3, 0, 3, 2};
  geometry.SetIndexBuffer(indexData, sizeof(indexData) / sizeof(indexData[0]));
  geometry.SetType(Dali::Geometry::TRIANGLES);

  return geometry;
}

} // anonymous namespace

Panel::Panel()
: Control(ControlBehaviour(DISABLE_VISUALS | DISABLE_SIZE_NEGOTIATION | DISABLE_STYLE_CHANGE_SIGNALS)),
  mPanelResolution(Vector2::ZERO),
  mResolutionPropertyIndex(Property::INVALID_INDEX),
  mIsTransparent(false),
  mIsDoubleSided(false),
  mIsUsingBackFacePlane(true),
  mBackFacePlaneColor(Vector3::ONE),
  mShaderManager(new Scene3D::Loader::ShaderManager()),
  mSceneIblScaleFactor(1.0f),
  mSceneSpecularMipmapLevels(1u),
  mIsShadowCasting(true),
  mIsShadowReceiving(true)
{
}

Panel::~Panel()
{
}

Dali::Scene3D::Panel Panel::New()
{
  Panel* impl = new Panel();

  Dali::Scene3D::Panel handle = Dali::Scene3D::Panel(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void Panel::SetPanelResolution(const Vector2& resolution)
{
  if(resolution.width != mPanelResolution.width || resolution.height != mPanelResolution.height)
  {
    mPanelResolution         = resolution;
    mResolutionPropertyIndex = mPanelNode.RegisterProperty("resolution", mPanelResolution);
    UpdateRenderTask();
  }
}

Vector2 Panel::GetPanelResolution() const
{
  return mPanelResolution;
}

void Panel::SetContent(Dali::Actor rootActor)
{
  ClearPanel();
  mRootLayer.Add(rootActor);
}

Dali::Actor Panel::GetContent() const
{
  return (mRootLayer.GetChildCount() > 1) ? ((mRootLayer.GetChildAt(0u) == mCamera) ? mRootLayer.GetChildAt(1u) : mRootLayer.GetChildAt(0u)) : Dali::Actor();
}

void Panel::ClearPanel()
{
  // CameraActor is needed to be left.
  while(mRootLayer.GetChildCount() > 0u)
  {
    Dali::Actor child = mRootLayer.GetChildAt(0u);
    child.Unparent();
  }

  if(mCamera)
  {
    mRootLayer.Add(mCamera);
  }
}

void Panel::CastShadow(bool castShadow)
{
  mIsShadowCasting = castShadow;
  ModelNodeTreeUtility::UpdateCastShadowRecursively(mPanelNode, mIsShadowCasting);
}

bool Panel::IsShadowCasting() const
{
  return mIsShadowCasting;
}

void Panel::ReceiveShadow(bool receiveShadow)
{
  mIsShadowReceiving = receiveShadow;
  ModelNodeTreeUtility::UpdateReceiveShadowRecursively(mPanelNode, mIsShadowReceiving);
}

bool Panel::IsShadowReceiving() const
{
  return mIsShadowReceiving;
}

void Panel::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Scene3D::Panel panel = Scene3D::Panel::DownCast(Dali::BaseHandle(object));

  if(panel)
  {
    Panel& panelImpl(GetImpl(panel));

    switch(index)
    {
      case Scene3D::Panel::Property::TRANSPARENT:
      {
        bool transparent = value.Get<bool>();
        panelImpl.SetTransparent(transparent);
        break;
      }
      case Scene3D::Panel::Property::DOUBLE_SIDED:
      {
        bool doubleSided = value.Get<bool>();
        panelImpl.SetDoubleSided(doubleSided);
        break;
      }
      case Scene3D::Panel::Property::USE_BACK_FACE_PLANE:
      {
        bool useBackFacePlane = value.Get<bool>();
        panelImpl.SetUseBackFacePlane(useBackFacePlane);
        break;
      }
      case Scene3D::Panel::Property::BACK_FACE_PLANE_COLOR:
      {
        Vector3 backFacePlaneColor = value.Get<Vector3>();
        panelImpl.SetBackFacePlaneColor(backFacePlaneColor);
        break;
      }
    }
  }
}

Property::Value Panel::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;
  Scene3D::Panel  panel = Scene3D::Panel::DownCast(Dali::BaseHandle(object));

  if(panel)
  {
    Panel& panelImpl(GetImpl(panel));

    switch(index)
    {
      case Scene3D::Panel::Property::TRANSPARENT:
      {
        value = panelImpl.IsTransparent();
        break;
      }
      case Scene3D::Panel::Property::DOUBLE_SIDED:
      {
        value = panelImpl.IsDoubleSided();
        break;
      }
      case Scene3D::Panel::Property::USE_BACK_FACE_PLANE:
      {
        value = panelImpl.IsUsingBackFacePlane();
        break;
      }
      case Scene3D::Panel::Property::BACK_FACE_PLANE_COLOR:
      {
        value = panelImpl.GetBackFacePlaneColor();
        break;
      }
    }
  }
  return value;
}

Dali::Actor Panel::GetOffScreenRenderableSourceActor()
{
  return (mRootLayer) ? mRootLayer : Dali::Actor();
}

bool Panel::IsOffScreenRenderTaskExclusive()
{
  return (mRenderTask) ? mRenderTask.IsExclusive() : false;
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void Panel::OnInitialize()
{
  // Make ParentOrigin as Center.
  Self().SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);

  mDefaultDiffuseTexture  = ImageResourceLoader::GetEmptyTextureWhiteRGB();
  mDefaultSpecularTexture = ImageResourceLoader::GetEmptyTextureWhiteRGB();

  Actor self = Self();
  mRootLayer = Layer::New();
  mRootLayer.SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_UI);
  // The models in the SceneView should be have independent coordinate with DALi default coordinate.
  mRootLayer.SetProperty(Dali::Actor::Property::NAME, "PanelRootLayer");
  mRootLayer.SetProperty(Dali::Actor::Property::INHERIT_POSITION, false);
  mRootLayer.SetProperty(Dali::Actor::Property::INHERIT_ORIENTATION, false);
  mRootLayer.SetProperty(Dali::Actor::Property::INHERIT_SCALE, false);
  self.Add(mRootLayer);

  mPanelNode = Scene3D::ModelNode::New();
  mPanelNode.SetProperty(Dali::Actor::Property::NAME, "PanelNode");
  mPanelNode.SetProperty(Dali::Actor::Property::COLOR_MODE, ColorMode::USE_OWN_MULTIPLY_PARENT_COLOR);
  mPanelNode.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mPanelNode.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  self.Add(mPanelNode);

  if(!mGeometry)
  {
    mGeometry = CreatePlaneGeometry();
  }

  mContentPlaneNode = Scene3D::ModelNode::New();
  mContentPlaneNode.SetProperty(Dali::Actor::Property::NAME, "ContentPlaneNode");
  mContentPlaneNode.SetProperty(Dali::Actor::Property::SIZE, Vector2::ONE);
  mContentPlaneNode.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mContentPlaneNode.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  mPanelNode.Add(mContentPlaneNode);

  mContentPlaneMaterial                         = Scene3D::Material::New();
  Scene3D::ModelPrimitive ContentPlanePrimitive = Scene3D::ModelPrimitive::New();
  ContentPlanePrimitive.SetMaterial(mContentPlaneMaterial);
  ContentPlanePrimitive.SetGeometry(mGeometry);
  mContentPlaneNode.AddModelPrimitive(ContentPlanePrimitive);

  // Back Plane
  mBackPlaneNode = Scene3D::ModelNode::New();
  mBackPlaneNode.SetProperty(Dali::Actor::Property::NAME, "BackPlaneNode");
  mBackPlaneNode.SetProperty(Dali::Actor::Property::SIZE, Vector2::ONE);
  mBackPlaneNode.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mBackPlaneNode.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  mBackPlaneNode.SetProperty(Dali::Actor::Property::ORIENTATION, Quaternion(Dali::Radian(Dali::ANGLE_180), Vector3::YAXIS));
  mPanelNode.Add(mBackPlaneNode);

  mBackPlaneMaterial                         = Scene3D::Material::New();
  Scene3D::ModelPrimitive backPlanePrimitive = Scene3D::ModelPrimitive::New();
  backPlanePrimitive.SetMaterial(mBackPlaneMaterial);
  backPlanePrimitive.SetGeometry(mGeometry);
  mBackPlaneNode.AddModelPrimitive(backPlanePrimitive);

  // Double Sided Plane
  if(!mDoubleSidedGeometry)
  {
    mDoubleSidedGeometry = CreatePlaneGeometry(true);
  }

  mDoubleSidedPlaneNode = Scene3D::ModelNode::New();
  mDoubleSidedPlaneNode.SetProperty(Dali::Actor::Property::NAME, "DoubleSidedPlaneNode");
  mDoubleSidedPlaneNode.SetProperty(Dali::Actor::Property::SIZE, Vector2::ONE);
  mDoubleSidedPlaneNode.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mDoubleSidedPlaneNode.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  mDoubleSidedPlaneNode.SetProperty(Dali::Actor::Property::ORIENTATION, Quaternion(Dali::Radian(Dali::ANGLE_180), Vector3::YAXIS));
  mPanelNode.Add(mDoubleSidedPlaneNode);

  Scene3D::ModelPrimitive doubleSidedPlanePrimitive = Scene3D::ModelPrimitive::New();
  doubleSidedPlanePrimitive.SetMaterial(mContentPlaneMaterial);
  doubleSidedPlanePrimitive.SetGeometry(mDoubleSidedGeometry);
  mDoubleSidedPlaneNode.AddModelPrimitive(doubleSidedPlanePrimitive);

  mResolutionPropertyIndex   = mPanelNode.RegisterProperty("resolution", mPanelResolution);
  Constraint scaleConstraint = Constraint::New<Vector3>(mPanelNode, Dali::Actor::Property::SCALE, [](Vector3& output, const PropertyInputContainer& inputs) {
    Vector3 panelSize = inputs[0]->GetVector3();
    Vector2 panelResolution = inputs[1]->GetVector2();
    output = Y_DIRECTION;
    if((EqualsZero(panelSize.width) || EqualsZero(panelSize.height)) ||
       (EqualsZero(panelResolution.width) || EqualsZero(panelResolution.height)))
    {
      return;
    }

    float aspectPanelSize = panelSize.width / panelSize.height;
    float aspectPanelResolution = panelResolution.width / panelResolution.height;
    if(aspectPanelResolution < aspectPanelSize)
    {
      float scaleFactor = panelSize.height / panelResolution.height;
      output.x = panelResolution.width * scaleFactor;
      output.y = panelSize.height;
    }
    else
    {
      float scaleFactor = panelSize.width / panelResolution.width;
      output.x = panelSize.width;
      output.y = panelResolution.height * scaleFactor;
    }
    output = output * Y_DIRECTION; });
  scaleConstraint.AddSource(Source{self, Dali::Actor::Property::SIZE});
  scaleConstraint.AddSource(Source{mPanelNode, mResolutionPropertyIndex});
  Dali::Integration::ConstraintSetInternalTag(scaleConstraint, PANEL_CONSTRAINT_TAG);
  scaleConstraint.Apply();

  UpdateProperties();
  SetOffScreenRenderableType(OffScreenRenderable::Type::FORWARD);
}

void Panel::OnSceneConnection(int depth)
{
  Actor parent = Self().GetParent();
  while(parent)
  {
    // If this Panel has parent SceneView and the its ShaderManager is same with privious ShaderManager,
    // this Panel don't need to update shader.
    Scene3D::SceneView sceneView = Scene3D::SceneView::DownCast(parent);
    if(sceneView)
    {
      mParentSceneView = sceneView;
      GetImpl(sceneView).RegisterSceneItem(this);
      Scene3D::Loader::ShaderManagerPtr shaderManager = GetImpl(sceneView).GetShaderManager();
      if(mShaderManager != shaderManager)
      {
        mShaderManager = shaderManager;
        ModelNodeTreeUtility::UpdateShaderRecursively(mPanelNode, mShaderManager);
      }
      break;
    }
    parent = parent.GetParent();
  }

  // On-screen / Off-screen window
  mSceneHolder = Integration::SceneHolder::Get(Self());
  if(mSceneHolder && !mRenderTask)
  {
    RenderTaskList taskList = mSceneHolder.GetRenderTaskList();
    mRenderTask             = taskList.CreateTask();
    mRenderTask.SetSourceActor(mRootLayer);
    mRenderTask.SetExclusive(true);
    mRenderTask.SetInputEnabled(true);
    mRenderTask.SetCullMode(true);
    mRenderTask.SetOrderIndex(PANEL_ORDER_INDEX);
    mRenderTask.SetScreenToFrameBufferMappingActor(mContentPlaneNode);

    UpdateRenderTask();
  }

  Control::OnSceneConnection(depth);
}

void Panel::OnSceneDisconnection()
{
  // If mParentSceneView is still onScene, that means this model
  // is disconnected from mParentSceneView's sub tree.
  // So, Unregister this Panel from SceneView.
  Scene3D::SceneView sceneView = mParentSceneView.GetHandle();
  if(sceneView && sceneView.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE))
  {
    GetImpl(sceneView).UnregisterSceneItem(this);
    mParentSceneView.Reset();
  }

  if(mSceneHolder)
  {
    if(mRenderTask)
    {
      RenderTaskList taskList = mSceneHolder.GetRenderTaskList();
      taskList.RemoveTask(mRenderTask);
      mRenderTask.Reset();
    }
    mSceneHolder.Reset();
  }
  mTexture.Reset();
  mFrameBuffer.Reset();

  Control::OnSceneDisconnection();
}

void Panel::GetOffScreenRenderTasks(std::vector<Dali::RenderTask>& tasks, bool isForward)
{
  tasks.clear();
  if(isForward)
  {
    if(mRenderTask)
    {
      tasks.push_back(mRenderTask);
    }
  }
}

void Panel::SetTransparent(bool transparent)
{
  if(mIsTransparent != transparent)
  {
    mIsTransparent = transparent;
    UpdateProperties();
  }
}

bool Panel::IsTransparent() const
{
  return mIsTransparent;
}

void Panel::SetDoubleSided(bool doubleSided)
{
  if(mIsDoubleSided != doubleSided)
  {
    mIsDoubleSided = doubleSided;
    UpdateProperties();
  }
}

bool Panel::IsDoubleSided() const
{
  return mIsDoubleSided;
}

void Panel::SetUseBackFacePlane(bool useBackFacePlane)
{
  if(mIsUsingBackFacePlane != useBackFacePlane)
  {
    mIsUsingBackFacePlane = useBackFacePlane;
    UpdateProperties();
  }
}

bool Panel::IsUsingBackFacePlane() const
{
  return mIsUsingBackFacePlane;
}

void Panel::SetBackFacePlaneColor(Vector3 backFacePlaneColor)
{
  if(mBackFacePlaneColor != backFacePlaneColor)
  {
    mBackFacePlaneColor     = backFacePlaneColor;
    Vector4 baseColorFactor = Vector4(mBackFacePlaneColor);
    baseColorFactor.a       = 1.0f;
    mBackPlaneMaterial.SetProperty(Dali::Scene3D::Material::Property::BASE_COLOR_FACTOR, baseColorFactor);
  }
}

Vector3 Panel::GetBackFacePlaneColor() const
{
  return mBackFacePlaneColor;
}

void Panel::UpdateProperties()
{
  if(mRenderTask)
  {
    mRenderTask.SetClearColor(mIsTransparent ? Color::TRANSPARENT : Color::WHITE);
  }

  if(mContentPlaneMaterial)
  {
    Dali::Scene3D::Material::AlphaModeType ContentPlaneAlphaMode = mIsTransparent ? Dali::Scene3D::Material::AlphaModeType::BLEND : Dali::Scene3D::Material::AlphaModeType::OPAQUE;
    mContentPlaneMaterial.SetProperty(Dali::Scene3D::Material::Property::ALPHA_MODE, ContentPlaneAlphaMode);
  }

  bool isBackPlaneVisible = !mIsTransparent && mIsUsingBackFacePlane;
  if(mBackPlaneNode)
  {
    mBackPlaneNode.SetProperty(Dali::Actor::Property::VISIBLE, isBackPlaneVisible);
  }

  if(mDoubleSidedPlaneNode)
  {
    bool isDoubleSidedRenderable = mIsDoubleSided && !isBackPlaneVisible;
    mDoubleSidedPlaneNode.SetProperty(Dali::Actor::Property::VISIBLE, isDoubleSidedRenderable);
  }
}

void Panel::UpdateImageBasedLightTexture()
{
  Dali::Texture currentDiffuseTexture          = mSceneDiffuseTexture;
  Dali::Texture currentSpecularTexture         = mSceneSpecularTexture;
  float         currentIblScaleFactor          = mSceneIblScaleFactor;
  uint32_t      currentIblSpecularMipmapLevels = mSceneSpecularMipmapLevels;

  if(!currentDiffuseTexture || !currentSpecularTexture)
  {
    currentDiffuseTexture          = mDefaultDiffuseTexture;
    currentSpecularTexture         = mDefaultSpecularTexture;
    currentIblScaleFactor          = Dali::Scene3D::Loader::EnvironmentDefinition::GetDefaultIntensity();
    currentIblSpecularMipmapLevels = 1u;
  }

  ModelNodeTreeUtility::UpdateImageBasedLightTextureRecursively(mPanelNode, currentDiffuseTexture, currentSpecularTexture, currentIblScaleFactor, currentIblSpecularMipmapLevels);
}

void Panel::UpdateImageBasedLightScaleFactor()
{
  if(!mSceneDiffuseTexture || !mSceneSpecularTexture)
  {
    return;
  }

  ModelNodeTreeUtility::UpdateImageBasedLightScaleFactorRecursively(mPanelNode, mSceneIblScaleFactor);
}

void Panel::NotifyShadowMapTexture(Dali::Texture shadowMapTexture)
{
  if(mShadowMapTexture != shadowMapTexture)
  {
    mShadowMapTexture = shadowMapTexture;
    ModelNodeTreeUtility::UpdateShadowMapTextureRecursively(mPanelNode, mShadowMapTexture);
  }
}

void Panel::NotifyImageBasedLightTexture(Dali::Texture diffuseTexture, Dali::Texture specularTexture, float scaleFactor, uint32_t specularMipmapLevels)
{
  if(mSceneDiffuseTexture != diffuseTexture || mSceneSpecularTexture != specularTexture)
  {
    mSceneDiffuseTexture       = diffuseTexture;
    mSceneSpecularTexture      = specularTexture;
    mSceneIblScaleFactor       = scaleFactor;
    mSceneSpecularMipmapLevels = specularMipmapLevels;

    UpdateImageBasedLightTexture();
  }
}

void Panel::NotifyImageBasedLightScaleFactor(float scaleFactor)
{
  mSceneIblScaleFactor = scaleFactor;
  if(mSceneDiffuseTexture && mSceneSpecularTexture)
  {
    UpdateImageBasedLightScaleFactor();
  }
}

void Panel::UpdateRenderTask()
{
  if(mPanelResolution.x <= 0.0f || mPanelResolution.y <= 0.0f)
  {
    return;
  }

  if(mRenderTask)
  {
    if(mCamera)
    {
      mCamera.Unparent();
      mCamera.Reset();
    }

    if(mTexture)
    {
      mTexture.Reset();
    }

    if(mFrameBuffer)
    {
      mFrameBuffer.Reset();
    }

    mCamera = Dali::CameraActor::New(mPanelResolution);
    mCamera.SetProperty(Dali::Actor::Property::NAME, "PanelCamera");
    mCamera.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    mCamera.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
    mCamera.SetProperty(Dali::Actor::Property::POSITION_X, mPanelResolution.x / 2.0f);
    mCamera.SetProperty(Dali::Actor::Property::POSITION_Y, mPanelResolution.y / 2.0f);
    mRootLayer.Add(mCamera);

    mRootLayer.SetProperty(Dali::Actor::Property::SIZE, mPanelResolution);

    mTexture     = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, mPanelResolution.width, mPanelResolution.height);
    mFrameBuffer = Dali::FrameBuffer::New(mTexture.GetWidth(), mTexture.GetHeight(), Dali::FrameBuffer::Attachment::DEPTH_STENCIL);
    mFrameBuffer.AttachColorTexture(mTexture);

    mRenderTask.SetCameraActor(mCamera);
    mRenderTask.SetFrameBuffer(mFrameBuffer);
    mRenderTask.SetClearEnabled(true);
    mRenderTask.SetClearColor(mIsTransparent ? Color::TRANSPARENT : Color::WHITE);

    mContentPlaneMaterial.SetTexture(Dali::Scene3D::Material::TextureType::BASE_COLOR, mTexture);
  }
}

} // namespace Internal
} // namespace Scene3D
} // namespace Dali
