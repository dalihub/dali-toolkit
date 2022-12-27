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
#include <dali-scene3d/internal/controls/scene-view/scene-view-impl.h>

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/public-api/image-loader/image.h>
#include <dali-toolkit/public-api/image-loader/sync-image-loader.h>
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/rendering/frame-buffer-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>
#include <string_view>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/model/model-impl.h>
#include <dali-scene3d/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-scene3d/public-api/loader/cube-map-loader.h>

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

constexpr uint8_t DEFAULT_FRAME_BUFFER_MULTI_SAMPLING_LEVEL = 4u;

static constexpr std::string_view SKYBOX_INTENSITY_STRING = "uIntensity";

Dali::Actor CreateSkybox(const std::string& skyboxUrl, Scene3D::SceneView::SkyboxType skyboxType)
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

  Dali::Texture  skyboxTexture;
  Dali::Shader   shaderSkybox;
  Dali::Renderer skyboxRenderer;

  if(skyboxType == Scene3D::SceneView::SkyboxType::CUBEMAP)
  {
    skyboxTexture = Dali::Scene3D::Loader::LoadCubeMap(skyboxUrl);
    shaderSkybox  = Shader::New(SHADER_SKYBOX_SHADER_VERT.data(), SHADER_SKYBOX_SHADER_FRAG.data());
  }
  else // Scene3D::SceneView::SkyboxType::EQUIRECTANGULAR
  {
    // Load image from file
    PixelData pixels = Dali::Toolkit::SyncImageLoader::Load(skyboxUrl);

    skyboxTexture = Texture::New(TextureType::TEXTURE_2D, pixels.GetPixelFormat(), pixels.GetWidth(), pixels.GetHeight());
    skyboxTexture.Upload(pixels, 0, 0, 0, 0, pixels.GetWidth(), pixels.GetHeight());
    shaderSkybox = Shader::New(SHADER_SKYBOX_SHADER_VERT.data(), SHADER_SKYBOX_EQUIRECTANGULAR_SHADER_FRAG.data());
  }

  Dali::TextureSet skyboxTextures = TextureSet::New();
  skyboxTextures.SetTexture(0, skyboxTexture);

  skyboxRenderer = Renderer::New(skyboxGeometry, shaderSkybox);
  skyboxRenderer.SetTextures(skyboxTextures);
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

SceneView::~SceneView() = default;

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

void SceneView::RegisterSceneItem(Scene3D::Internal::ImageBasedLightObserver* item)
{
  if(item)
  {
    item->NotifyImageBasedLightTexture(mDiffuseTexture, mSpecularTexture, mIblScaleFactor);
    mItems.push_back(item);
  }
}

void SceneView::UnregisterSceneItem(Scene3D::Internal::ImageBasedLightObserver* item)
{
  if(item)
  {
    for(uint32_t i = 0; i < mItems.size(); ++i)
    {
      if(mItems[i] == item)
      {
        mItems.erase(mItems.begin() + i);
        break;
      }
    }
  }
}

void SceneView::SetImageBasedLightSource(const std::string& diffuseUrl, const std::string& specularUrl, float scaleFactor)
{
  mIBLResourceReady = false;

  // If url is empty or invalid, reset IBL.
  mDiffuseTexture  = (!diffuseUrl.empty()) ? Dali::Scene3D::Loader::LoadCubeMap(diffuseUrl) : Texture();
  mSpecularTexture = (!specularUrl.empty()) ? Dali::Scene3D::Loader::LoadCubeMap(specularUrl) : Texture();

  mIblScaleFactor = scaleFactor;

  for(auto&& item : mItems)
  {
    if(item)
    {
      item->NotifyImageBasedLightTexture(mDiffuseTexture, mSpecularTexture, mIblScaleFactor);
    }
  }

  mIBLResourceReady = true;
  if(IsResourceReady())
  {
    Control::SetResourceReady(false);
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

void SceneView::SetSkybox(const std::string& skyboxUrl, Scene3D::SceneView::SkyboxType skyboxType)
{
  mSkyboxResourceReady = false;
  if(mSkybox)
  {
    mSkybox.Unparent();
    mSkybox.Reset();
  }
  mSkybox = CreateSkybox(skyboxUrl, skyboxType);
  SetSkyboxIntensity(mSkyboxIntensity);
  SetSkyboxOrientation(mSkyboxOrientation);
  if(mRootLayer)
  {
    mRootLayer.Add(mSkybox);
  }

  mSkyboxResourceReady = true;
  if(IsResourceReady())
  {
    Control::SetResourceReady(false);
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
  Window window = DevelWindow::Get(Self());
  if(window)
  {
    window.ResizeSignal().Connect(this, &SceneView::OnWindowResized);
    RenderTaskList taskList = window.GetRenderTaskList();
    mRenderTask             = taskList.CreateTask();
    mRenderTask.SetSourceActor(mRootLayer);
    mRenderTask.SetExclusive(true);
    mRenderTask.SetInputEnabled(true);
    mRenderTask.SetCullMode(false);
    mRenderTask.SetScreenToFrameBufferMappingActor(Self());

    UpdateRenderTask();
    mWindow = window;
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
    RenderTaskList taskList = window.GetRenderTaskList();
    if(mRenderTask)
    {
      taskList.RemoveTask(mRenderTask);
      mFrameBuffer.Reset();
    }
  }
  mWindow.Reset();

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

  mDefaultCamera = Dali::CameraActor::New();
  mDefaultCamera.SetProperty(Dali::Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  mDefaultCamera.SetProperty(Dali::Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  mDefaultCamera.SetNearClippingPlane(1.0f);
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
  return mIBLResourceReady & mSkyboxResourceReady;
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
        DevelFrameBuffer::SetMultiSamplingLevel(mFrameBuffer, DEFAULT_FRAME_BUFFER_MULTI_SAMPLING_LEVEL);
        Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::Image::GenerateUrl(mFrameBuffer, 0u);

        Property::Map imagePropertyMap;
        imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, imageUrl.GetUrl());
        // To make sure this visual call LoadTexture API immediate.
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::LOAD_POLICY, Toolkit::ImageVisual::LoadPolicy::IMMEDIATE);
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::RELEASE_POLICY, Toolkit::ImageVisual::ReleasePolicy::DESTROYED);
        // To flip rendered scene without CameraActor::SetInvertYAxis() to avoid backface culling.
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::PIXEL_AREA, Vector4(0.0f, 1.0f, 1.0f, -1.0f));
        mVisual = Toolkit::VisualFactory::Get().CreateVisual(imagePropertyMap);

        // Use premultiplied alpha when we use FBO
        if(mVisual)
        {
          Toolkit::GetImplementation(mVisual).EnablePreMultipliedAlpha(true);
        }

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

} // namespace Internal
} // namespace Scene3D
} // namespace Dali
