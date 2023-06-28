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
#include <dali-scene3d/internal/controls/scene-view/scene-view-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/public-api/image-loader/image.h>
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/frame-buffer-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <string_view>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/model/model-impl.h>
#include <dali-scene3d/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-scene3d/internal/light/light-impl.h>

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
  return Scene3D::SceneView::New();
}

// Setup properties, signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Scene3D::SceneView, Toolkit::Control, Create);
DALI_TYPE_REGISTRATION_END()

Property::Index   RENDERING_BUFFER    = Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX + 1;
constexpr int32_t DEFAULT_ORIENTATION = 0;
constexpr int32_t INVALID_INDEX = -1;

static constexpr std::string_view SKYBOX_INTENSITY_STRING = "uIntensity";

Dali::Actor CreateSkybox()
{
  struct Vertex
  {
    Vector3 aPosition;
  };

  Vertex skyboxVertices[] = {
    // back
    {Vector3(-1.0f, 1.0f, -1.0f)},
    {Vector3(-1.0f, -1.0f, -1.0f)},
    {Vector3(1.0f, -1.0f, -1.0f)},
    {Vector3(1.0f, -1.0f, -1.0f)},
    {Vector3(1.0f, 1.0f, -1.0f)},
    {Vector3(-1.0f, 1.0f, -1.0f)},

    // left
    {Vector3(-1.0f, -1.0f, 1.0f)},
    {Vector3(-1.0f, -1.0f, -1.0f)},
    {Vector3(-1.0f, 1.0f, -1.0f)},
    {Vector3(-1.0f, 1.0f, -1.0f)},
    {Vector3(-1.0f, 1.0f, 1.0f)},
    {Vector3(-1.0f, -1.0f, 1.0f)},

    // right
    {Vector3(1.0f, -1.0f, -1.0f)},
    {Vector3(1.0f, -1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, -1.0f)},
    {Vector3(1.0f, -1.0f, -1.0f)},

    // front
    {Vector3(-1.0f, -1.0f, 1.0f)},
    {Vector3(-1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, -1.0f, 1.0f)},
    {Vector3(-1.0f, -1.0f, 1.0f)},

    // botton
    {Vector3(-1.0f, 1.0f, -1.0f)},
    {Vector3(1.0f, 1.0f, -1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(1.0f, 1.0f, 1.0f)},
    {Vector3(-1.0f, 1.0f, 1.0f)},
    {Vector3(-1.0f, 1.0f, -1.0f)},

    // top
    {Vector3(-1.0f, -1.0f, -1.0f)},
    {Vector3(-1.0f, -1.0f, 1.0f)},
    {Vector3(1.0f, -1.0f, -1.0f)},
    {Vector3(1.0f, -1.0f, -1.0f)},
    {Vector3(-1.0f, -1.0f, 1.0f)},
    {Vector3(1.0f, -1.0f, 1.0f)}};

  Dali::VertexBuffer vertexBuffer = Dali::VertexBuffer::New(Property::Map().Add("aPosition", Property::VECTOR3));
  vertexBuffer.SetData(skyboxVertices, sizeof(skyboxVertices) / sizeof(Vertex));

  Dali::Geometry skyboxGeometry = Geometry::New();
  skyboxGeometry.AddVertexBuffer(vertexBuffer);
  skyboxGeometry.SetType(Geometry::TRIANGLES);

  Dali::Shader   shaderSkybox = Shader::New(SHADER_SKYBOX_SHADER_VERT.data(), SHADER_SKYBOX_SHADER_FRAG.data());
  Dali::Renderer skyboxRenderer;
  skyboxRenderer = Renderer::New(skyboxGeometry, shaderSkybox);
  skyboxRenderer.SetProperty(Renderer::Property::DEPTH_INDEX, 2.0f);
  // Enables the depth test.
  skyboxRenderer.SetProperty(Renderer::Property::DEPTH_TEST_MODE, DepthTestMode::ON);
  // The fragment shader will run only is those pixels that have the max depth value.
  skyboxRenderer.SetProperty(Renderer::Property::DEPTH_FUNCTION, DepthFunction::LESS_EQUAL);

  Dali::Actor skyboxActor = Actor::New();
  skyboxActor.SetProperty(Dali::Actor::Property::NAME, "SkyBox");
  skyboxActor.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  skyboxActor.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  skyboxActor.AddRenderer(skyboxRenderer);
  return skyboxActor;
}

} // anonymous namespace

SceneView::SceneView()
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mWindowOrientation(DEFAULT_ORIENTATION),
  mSkybox(),
  mSkyboxOrientation(Quaternion()),
  mSkyboxIntensity(1.0f)
{
}

SceneView::~SceneView()
{
  if(Dali::Adaptor::IsAvailable())
  {
    if(mIblDiffuseLoadTask)
    {
      Dali::AsyncTaskManager::Get().RemoveTask(mIblDiffuseLoadTask);
      mIblDiffuseLoadTask.Reset();
    }
    if(mIblSpecularLoadTask)
    {
      Dali::AsyncTaskManager::Get().RemoveTask(mIblSpecularLoadTask);
      mIblSpecularLoadTask.Reset();
    }
    if(mSkyboxLoadTask)
    {
      Dali::AsyncTaskManager::Get().RemoveTask(mSkyboxLoadTask);
      mSkyboxLoadTask.Reset();
    }
  }
}

Dali::Scene3D::SceneView SceneView::New()
{
  SceneView* impl = new SceneView();

  Dali::Scene3D::SceneView handle = Dali::Scene3D::SceneView(*impl);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  impl->Initialize();

  return handle;
}

void SceneView::AddCamera(CameraActor camera)
{
  if(camera)
  {
    if(mCameras.empty())
    {
      UpdateCamera(camera);
    }
    mCameras.push_back(camera);
  }
}

void SceneView::RemoveCamera(CameraActor camera)
{
  if(camera == mDefaultCamera)
  {
    DALI_LOG_ERROR("Default Camera cannot removed.\n");
    return;
  }

  if(camera)
  {
    for(uint32_t i = 0; i < mCameras.size(); ++i)
    {
      if(mCameras[i] == camera)
      {
        mCameras.erase(mCameras.begin() + i);
        break;
      }
    }

    if(mSelectedCamera == camera)
    {
      CameraActor newCurrentCamera = *mCameras.begin();
      UpdateCamera(newCurrentCamera);
    }
  }
}

uint32_t SceneView::GetCameraCount() const
{
  return mCameras.size();
}

CameraActor SceneView::GetSelectedCamera() const
{
  return mSelectedCamera;
}

CameraActor SceneView::GetCamera(uint32_t index) const
{
  if(index < mCameras.size())
  {
    return mCameras[index];
  }
  DALI_LOG_ERROR("Input index is out of bounds\n");
  return CameraActor();
}

CameraActor SceneView::GetCamera(const std::string& name) const
{
  CameraActor returnCamera;
  for(auto&& camera : mCameras)
  {
    if(camera.GetProperty<std::string>(Actor::Property::NAME) == name)
    {
      returnCamera = camera;
      break;
    }
  }
  return returnCamera;
}

void SceneView::SelectCamera(uint32_t index)
{
  UpdateCamera(GetCamera(index));
}

void SceneView::SelectCamera(const std::string& name)
{
  UpdateCamera(GetCamera(name));
}

void SceneView::RegisterSceneItem(Scene3D::Internal::LightObserver* item)
{
  if(item)
  {
    item->NotifyImageBasedLightTexture(mDiffuseTexture, mSpecularTexture, mIblScaleFactor, mSpecularMipmapLevels);

    if(mActivatedLightCount > 0)
    {
      uint32_t maxLightCount = Scene3D::Internal::Light::GetMaximumEnabledLightCount();
      for(uint32_t i = 0; i < maxLightCount; ++i)
      {
        if(mActivatedLights[i])
        {
          item->NotifyLightAdded(i, mActivatedLights[i]);
        }
      }
    }
    mItems.push_back(item);
  }
}

void SceneView::UnregisterSceneItem(Scene3D::Internal::LightObserver* item)
{
  if(item)
  {
    for(uint32_t i = 0; i < mItems.size(); ++i)
    {
      if(mItems[i] == item)
      {
        if(mActivatedLightCount > 0)
        {
          uint32_t maxLightCount = Scene3D::Internal::Light::GetMaximumEnabledLightCount();
          for(uint32_t i = 0; i < maxLightCount; ++i)
          {
            if(mActivatedLights[i])
            {
              item->NotifyLightRemoved(i);
            }
          }
        }
        mItems.erase(mItems.begin() + i);
        break;
      }
    }
  }
}

void SceneView::SetImageBasedLightSource(const std::string& diffuseUrl, const std::string& specularUrl, float scaleFactor)
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
    if(mIblDiffuseLoadTask)
    {
      Dali::AsyncTaskManager::Get().RemoveTask(mIblDiffuseLoadTask);
      mIblDiffuseLoadTask.Reset();
    }

    if(mIblSpecularLoadTask)
    {
      Dali::AsyncTaskManager::Get().RemoveTask(mIblSpecularLoadTask);
      mIblSpecularLoadTask.Reset();
    }

    mIblDiffuseDirty          = false;
    mIblSpecularDirty         = false;
    mIblDiffuseResourceReady  = true;
    mIblSpecularResourceReady = true;

    mDiffuseTexture.Reset();
    mSpecularTexture.Reset();

    mSpecularMipmapLevels = 1u;
    NotifyImageBasedLightTextureChange();
  }
  else
  {
    if(isOnScene && mIblDiffuseDirty)
    {
      if(mIblDiffuseLoadTask)
      {
        Dali::AsyncTaskManager::Get().RemoveTask(mIblDiffuseLoadTask);
        mIblDiffuseLoadTask.Reset();
      }
      mIblDiffuseLoadTask = new EnvironmentMapLoadTask(mDiffuseIblUrl, Scene3D::EnvironmentMapType::CUBEMAP, MakeCallback(this, &SceneView::OnIblDiffuseLoadComplete));
      Dali::AsyncTaskManager::Get().AddTask(mIblDiffuseLoadTask);
      mIblDiffuseDirty = false;
    }

    if(isOnScene && mIblSpecularDirty)
    {
      if(mIblSpecularLoadTask)
      {
        Dali::AsyncTaskManager::Get().RemoveTask(mIblSpecularLoadTask);
        mIblSpecularLoadTask.Reset();
      }
      mIblSpecularLoadTask = new EnvironmentMapLoadTask(mSpecularIblUrl, Scene3D::EnvironmentMapType::CUBEMAP, MakeCallback(this, &SceneView::OnIblSpecularLoadComplete));
      Dali::AsyncTaskManager::Get().AddTask(mIblSpecularLoadTask);
      mIblSpecularDirty = false;
    }
  }

  if(!Dali::Equals(mIblScaleFactor, scaleFactor))
  {
    SetImageBasedLightScaleFactor(scaleFactor);
  }

  // If diffuse and specular textures are already loaded, emits resource ready signal here.
  if(IsResourceReady())
  {
    Control::SetResourceReady();
  }
}

void SceneView::SetImageBasedLightScaleFactor(float scaleFactor)
{
  mIblScaleFactor = scaleFactor;
  for(auto&& item : mItems)
  {
    if(item)
    {
      item->NotifyImageBasedLightScaleFactor(scaleFactor);
    }
  }
}

float SceneView::GetImageBasedLightScaleFactor() const
{
  return mIblScaleFactor;
}

void SceneView::AddLight(Scene3D::Light light)
{
  bool enabled = AddLightInternal(light);
  mLights.push_back(std::make_pair(light, enabled));
}

void SceneView::RemoveLight(Scene3D::Light light)
{
  if(mActivatedLights.empty())
  {
    return;
  }

  bool needToDisable = false;
  for(uint32_t i = 0; i < mLights.size(); ++i)
  {
    if(mLights[i].first == light)
    {
      // If mLights[i].second is true, it means the light is currently activated in Scene.
      // Then it should be removed from mActivatedLights list too.
      needToDisable = mLights[i].second;
      mLights.erase(mLights.begin() + i);
      break;
    }
  }

  uint32_t maxNumberOfLight = Scene3D::Internal::Light::GetMaximumEnabledLightCount();
  if(needToDisable)
  {
    uint32_t removedIndex = RemoveLightInternal(light);
    if(mActivatedLightCount < maxNumberOfLight && mLights.size() >= maxNumberOfLight)
    {
      for(auto && lightItem : mLights)
      {
        if(lightItem.second == false)
        {
          lightItem.second = AddLightInternal(lightItem.first);
          break;
        }
      }
    }

    // To remove empty entity of mActivatedLights, moves last object to empty position.
    // Because one Light is removed, mActivatedLights[mActivatedLightCount] is current last object of the list.
    if(!mActivatedLights[removedIndex] && mActivatedLightCount > 0 && removedIndex < mActivatedLightCount)
    {
      Scene3D::Light reorderingLight = mActivatedLights[mActivatedLightCount];
      RemoveLightInternal(reorderingLight);
      AddLightInternal(reorderingLight);
    }
  }
}

uint32_t SceneView::GetActivatedLightCount() const
{
  return mActivatedLightCount;
}

void SceneView::UseFramebuffer(bool useFramebuffer)
{
  if(mUseFrameBuffer != useFramebuffer)
  {
    mUseFrameBuffer = useFramebuffer;
    UpdateRenderTask();
  }
}

bool SceneView::IsUsingFramebuffer() const
{
  return mUseFrameBuffer;
}

void SceneView::SetFramebufferMultiSamplingLevel(uint8_t multiSamplingLevel)
{
  if(mFrameBufferMultiSamplingLevel != multiSamplingLevel)
  {
    mFrameBufferMultiSamplingLevel = multiSamplingLevel;

    // Create new framebuffer with changed multiSamplingLevel.
    if(mRenderTask && mFrameBuffer && mTexture)
    {
      Vector3 size = Self().GetProperty<Vector3>(Dali::Actor::Property::SIZE);

      mFrameBuffer = FrameBuffer::New(size.width, size.height, FrameBuffer::Attachment::DEPTH_STENCIL);
      mFrameBuffer.AttachColorTexture(mTexture);
      DevelFrameBuffer::SetMultiSamplingLevel(mFrameBuffer, mFrameBufferMultiSamplingLevel);
      mRenderTask.SetFrameBuffer(mFrameBuffer);

      // Note : we don't need to create new visual since visual's url is depend on mTexture.
    }
  }
}

uint8_t SceneView::GetFramebufferMultiSamplingLevel() const
{
  return mFrameBufferMultiSamplingLevel;
}

void SceneView::SetSkybox(const std::string& skyboxUrl)
{
  if(mSkyboxUrl != skyboxUrl)
  {
    UpdateSkybox(skyboxUrl, mSkyboxEnvironmentMapType);
  }
}

void SceneView::SetSkyboxEnvironmentMapType(Scene3D::EnvironmentMapType skyboxEnvironmentMapType)
{
  if(mSkyboxEnvironmentMapType != skyboxEnvironmentMapType)
  {
    UpdateSkybox(mSkyboxUrl, skyboxEnvironmentMapType);
  }
}

void SceneView::SetSkyboxIntensity(float intensity)
{
  mSkyboxIntensity = intensity;
  if(intensity < 0)
  {
    DALI_LOG_ERROR("Intensity should be greater than or equal to 0.\n");
    mSkyboxIntensity = 0.0f;
  }

  if(mSkybox)
  {
    mSkybox.RegisterProperty(SKYBOX_INTENSITY_STRING.data(), mSkyboxIntensity);
  }
}

float SceneView::GetSkyboxIntensity() const
{
  return mSkyboxIntensity;
}

void SceneView::SetSkyboxOrientation(const Quaternion& orientation)
{
  mSkyboxOrientation = orientation;
  if(mSkybox)
  {
    mSkybox.SetProperty(Dali::Actor::Property::ORIENTATION, orientation);
  }
}

Quaternion SceneView::GetSkyboxOrientation() const
{
  return mSkyboxOrientation;
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void SceneView::OnSceneConnection(int depth)
{
  // If diffuse and specular url is not valid, IBL does not need to be loaded.
  if(!mDiffuseIblUrl.empty() && !mSpecularIblUrl.empty())
  {
    SetImageBasedLightSource(mDiffuseIblUrl, mSpecularIblUrl, mIblScaleFactor);
  }

  if(!mSkyboxUrl.empty())
  {
    UpdateSkybox(mSkyboxUrl, mSkyboxEnvironmentMapType);
  }

  Window window = DevelWindow::Get(Self());
  if(window)
  {
    // Only for on-screen window
    window.ResizeSignal().Connect(this, &SceneView::OnWindowResized);

    mWindow            = window;
    mWindowOrientation = DevelWindow::GetPhysicalOrientation(window);
  }

  // On-screen / Off-screen window
  mSceneHolder = Integration::SceneHolder::Get(Self());
  if(mSceneHolder)
  {
    RenderTaskList taskList = mSceneHolder.GetRenderTaskList();
    mRenderTask             = taskList.CreateTask();
    mRenderTask.SetSourceActor(mRootLayer);
    mRenderTask.SetExclusive(true);
    mRenderTask.SetInputEnabled(true);
    mRenderTask.SetCullMode(false);
    mRenderTask.SetScreenToFrameBufferMappingActor(Self());

    UpdateRenderTask();
  }

  Control::OnSceneConnection(depth);
}

void SceneView::OnSceneDisconnection()
{
  mItems.clear();

  Window window = mWindow.GetHandle();
  if(window)
  {
    window.ResizeSignal().Disconnect(this, &SceneView::OnWindowResized);
  }
  mWindow.Reset();

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
  mFrameBuffer.Reset();

  Control::OnSceneDisconnection();
}

void SceneView::OnInitialize()
{
  Actor self = Self();
  mRootLayer = Layer::New();
  mRootLayer.SetProperty(Layer::Property::BEHAVIOR, Layer::LAYER_3D);
  mRootLayer.SetProperty(Layer::Property::DEPTH_TEST, true);
  // The models in the SceneView should be have independent coordinate with DALi default coordinate.
  mRootLayer.SetProperty(Dali::Actor::Property::INHERIT_POSITION, false);
  mRootLayer.SetProperty(Dali::Actor::Property::INHERIT_ORIENTATION, false);
  mRootLayer.SetProperty(Dali::Actor::Property::INHERIT_SCALE, false);
  self.Add(mRootLayer);

  mDefaultCamera = Dali::CameraActor::New3DCamera();
  mDefaultCamera.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mDefaultCamera.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  AddCamera(mDefaultCamera);
  UpdateCamera(mDefaultCamera);
}

void SceneView::OnChildAdd(Actor& child)
{
  if(child != mRootLayer)
  {
    mRootLayer.Add(child);
  }
  Control::OnChildAdd(child);
}

void SceneView::OnChildRemove(Actor& child)
{
  mRootLayer.Remove(child);
  Control::OnChildRemove(child);
}

float SceneView::GetHeightForWidth(float width)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  return Control::GetHeightForWidth(width) + padding.top + padding.bottom;
}

float SceneView::GetWidthForHeight(float height)
{
  Extents padding;
  padding = Self().GetProperty<Extents>(Toolkit::Control::Property::PADDING);
  return Control::GetWidthForHeight(height) + padding.start + padding.end;
}

void SceneView::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  Control::OnRelayout(size, container);
  // Change canvas size of camera actor.
  UpdateRenderTask();
}

bool SceneView::IsResourceReady() const
{
  return mIblDiffuseResourceReady && mIblSpecularResourceReady && mSkyboxResourceReady;
}

void SceneView::UpdateCamera(CameraActor camera)
{
  if(camera)
  {
    if(mSelectedCamera && mSelectedCamera.GetParent())
    {
      mSelectedCamera.Unparent();
    }
    mRootLayer.Add(camera);
  }

  mSelectedCamera = camera;
  UpdateRenderTask();
}

void SceneView::UpdateRenderTask()
{
  if(mRenderTask)
  {
    if(mSelectedCamera != mRenderTask.GetCameraActor())
    {
      mRenderTask.SetCameraActor(mSelectedCamera);
    }

    Vector3     size        = Self().GetProperty<Vector3>(Dali::Actor::Property::SIZE);
    const float aspectRatio = size.width / size.height;
    mSelectedCamera.SetAspectRatio(aspectRatio);

    if(mUseFrameBuffer)
    {
      Dali::FrameBuffer currentFrameBuffer = mRenderTask.GetFrameBuffer();
      if(!currentFrameBuffer ||
         !Dali::Equals(currentFrameBuffer.GetColorTexture().GetWidth(), size.width) ||
         !Dali::Equals(currentFrameBuffer.GetColorTexture().GetHeight(), size.height))
      {
        mRootLayer.SetProperty(Dali::Actor::Property::COLOR_MODE, ColorMode::USE_OWN_COLOR);
        mRenderTask.ResetViewportGuideActor();
        mRenderTask.SetViewport(Dali::Viewport(Vector4::ZERO));

        // create offscreen buffer of new size to render our child actors to
        mTexture     = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, unsigned(size.width), unsigned(size.height));
        mFrameBuffer = FrameBuffer::New(size.width, size.height, FrameBuffer::Attachment::DEPTH_STENCIL);
        mFrameBuffer.AttachColorTexture(mTexture);
        DevelFrameBuffer::SetMultiSamplingLevel(mFrameBuffer, mFrameBufferMultiSamplingLevel);
        Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::Image::GenerateUrl(mFrameBuffer, 0u);

        Property::Map imagePropertyMap;
        imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, imageUrl.GetUrl());
        // To flip rendered scene without CameraActor::SetInvertYAxis() to avoid backface culling.
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::PIXEL_AREA, Vector4(0.0f, 1.0f, 1.0f, -1.0f));
        mVisual = Toolkit::VisualFactory::Get().CreateVisual(imagePropertyMap);

        Toolkit::DevelControl::RegisterVisual(*this, RENDERING_BUFFER, mVisual);

        mRenderTask.SetFrameBuffer(mFrameBuffer);
        mRenderTask.SetClearEnabled(true);
        mRenderTask.SetClearColor(Color::TRANSPARENT);
      }
    }
    else
    {
      mRenderTask.SetViewportGuideActor(Self());
      if(mRenderTask.GetFrameBuffer())
      {
        mRootLayer.SetProperty(Dali::Actor::Property::COLOR_MODE, ColorMode::USE_OWN_MULTIPLY_PARENT_ALPHA);
        FrameBuffer framebuffer;
        mRenderTask.SetFrameBuffer(framebuffer);
        mRenderTask.SetClearEnabled(false);

        Toolkit::DevelControl::UnregisterVisual(*this, RENDERING_BUFFER);

        mVisual.Reset();
        mFrameBuffer.Reset();
        mTexture.Reset();
      }
    }

    RotateCamera();
  }
}

void SceneView::OnWindowResized(Window window, Window::WindowSize size)
{
  mWindowOrientation = DevelWindow::GetPhysicalOrientation(window);
  RotateCamera();
}

void SceneView::RotateCamera()
{
  if(mUseFrameBuffer)
  {
    DevelCameraActor::RotateProjection(mSelectedCamera, DEFAULT_ORIENTATION);
  }
  else
  {
    DevelCameraActor::RotateProjection(mSelectedCamera, mWindowOrientation);
  }
}

void SceneView::UpdateSkybox(const std::string& skyboxUrl, Scene3D::EnvironmentMapType skyboxEnvironmentMapType)
{
  bool isOnScene = Self().GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE);
  if(mSkyboxUrl != skyboxUrl || mSkyboxEnvironmentMapType != skyboxEnvironmentMapType)
  {
    mSkyboxDirty              = true;
    mSkyboxResourceReady      = false;
    mSkyboxUrl                = skyboxUrl;
    mSkyboxEnvironmentMapType = skyboxEnvironmentMapType;
  }

  if(mSkyboxUrl.empty())
  {
    if(mSkyboxLoadTask)
    {
      Dali::AsyncTaskManager::Get().RemoveTask(mSkyboxLoadTask);
      mSkyboxLoadTask.Reset();
    }

    if(mSkybox)
    {
        mSkybox.Unparent();
        mSkybox.Reset();
        mSkyboxTexture.Reset();
    }

    mSkyboxDirty         = false;
    mSkyboxResourceReady = true;
  }
  else
  {
    if(isOnScene && mSkyboxDirty)
    {
      if(mSkyboxLoadTask)
      {
        Dali::AsyncTaskManager::Get().RemoveTask(mSkyboxLoadTask);
        mSkyboxLoadTask.Reset();
      }

      mSkyboxLoadTask = new EnvironmentMapLoadTask(mSkyboxUrl, mSkyboxEnvironmentMapType, MakeCallback(this, &SceneView::OnSkyboxLoadComplete));
      Dali::AsyncTaskManager::Get().AddTask(mSkyboxLoadTask);
      mSkyboxDirty = false;
    }
  }

  if(IsResourceReady())
  {
    Control::SetResourceReady();
  }
}

void SceneView::OnSkyboxLoadComplete()
{
  if(!mSkybox)
  {
    mSkybox = CreateSkybox();
    SetSkyboxIntensity(mSkyboxIntensity);
    SetSkyboxOrientation(mSkyboxOrientation);
    if(mRootLayer)
    {
      mRootLayer.Add(mSkybox);
    }
  }

  mSkyboxResourceReady = true;
  if(IsResourceReady())
  {
    Control::SetResourceReady();
  }

  mSkyboxTexture = mSkyboxLoadTask->GetLoadedTexture();
  Shader skyboxShader;
  if(mSkyboxLoadTask->GetEnvironmentMapType() == Scene3D::EnvironmentMapType::CUBEMAP)
  {
    skyboxShader = Shader::New(SHADER_SKYBOX_SHADER_VERT.data(), SHADER_SKYBOX_SHADER_FRAG.data());
  }
  else
  {
    skyboxShader = Shader::New(SHADER_SKYBOX_SHADER_VERT.data(), SHADER_SKYBOX_EQUIRECTANGULAR_SHADER_FRAG.data());
  }

  Renderer skyboxRenderer = (mSkybox.GetRendererCount() > 0u) ? mSkybox.GetRendererAt(0u) : Renderer();
  if(skyboxRenderer)
  {
    Dali::TextureSet skyboxTextures = TextureSet::New();
    skyboxTextures.SetTexture(0, mSkyboxTexture);
    skyboxRenderer.SetTextures(skyboxTextures);
    skyboxRenderer.SetShader(skyboxShader);
  }

  mSkyboxLoadTask.Reset();
}

void SceneView::OnIblDiffuseLoadComplete()
{
  mDiffuseTexture          = mIblDiffuseLoadTask->GetLoadedTexture();
  mIblDiffuseResourceReady = true;
  if(mIblDiffuseResourceReady && mIblSpecularResourceReady)
  {
    OnIblLoadComplete();
  }
  mIblDiffuseLoadTask.Reset();
}

void SceneView::OnIblSpecularLoadComplete()
{
  mSpecularTexture          = mIblSpecularLoadTask->GetLoadedTexture();
  mSpecularMipmapLevels     = mIblSpecularLoadTask->GetMipmapLevels();
  mIblSpecularResourceReady = true;
  if(mIblDiffuseResourceReady && mIblSpecularResourceReady)
  {
    OnIblLoadComplete();
  }
  mIblSpecularLoadTask.Reset();
}

void SceneView::OnIblLoadComplete()
{
  NotifyImageBasedLightTextureChange();
  if(IsResourceReady())
  {
    Control::SetResourceReady();
  }
}

void SceneView::NotifyImageBasedLightTextureChange()
{
  for(auto&& item : mItems)
  {
    if(item)
    {
      item->NotifyImageBasedLightTexture(mDiffuseTexture, mSpecularTexture, mIblScaleFactor, mSpecularMipmapLevels);
    }
  }
}

bool SceneView::AddLightInternal(Scene3D::Light light)
{
  uint32_t maxNumberOfLight = Scene3D::Internal::Light::GetMaximumEnabledLightCount();
  if(mActivatedLightCount == 0)
  {
    mActivatedLights.resize(maxNumberOfLight);
  }

  bool enabled = false;
  if(mActivatedLightCount < maxNumberOfLight)
  {
    uint32_t newLightIndex = 0u;
    for(; newLightIndex < maxNumberOfLight; ++newLightIndex)
    {
      if(!mActivatedLights[newLightIndex])
      {
        mActivatedLights[newLightIndex] = light;
        break;
      }
    }

    for(auto&& item : mItems)
    {
      if(item)
      {
        item->NotifyLightAdded(newLightIndex, light);
      }
    }

    mActivatedLightCount++;
    enabled = true;
  }
  return enabled;
}

int32_t SceneView::RemoveLightInternal(Scene3D::Light light)
{
  int32_t removedIndex = INVALID_INDEX;
  uint32_t maxNumberOfLight = Scene3D::Internal::Light::GetMaximumEnabledLightCount();
  for(uint32_t i = 0; i < maxNumberOfLight; ++i)
  {
    if(mActivatedLights[i] == light)
    {
      for(auto&& item : mItems)
      {
        if(item)
        {
          item->NotifyLightRemoved(i);
        }
      }
      mActivatedLights[i].Reset();
      mActivatedLightCount--;
      removedIndex = i;
      break;
    }
  }

  return removedIndex;
}

} // namespace Internal
} // namespace Scene3D
} // namespace Dali
