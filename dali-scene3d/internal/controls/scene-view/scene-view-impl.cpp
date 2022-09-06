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
#include <dali/devel-api/common/stage.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/controls/model-view/model-view-impl.h>
#include <dali-scene3d/public-api/loader/cube-map-loader.h>

#include <dali/integration-api/debug.h>

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

Property::Index RENDERING_BUFFER = Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX + 1;

} // anonymous namespace

SceneView::SceneView()
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT))
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

uint32_t SceneView::GetCameraCount()
{
  return mCameras.size();
}

CameraActor SceneView::GetSelectedCamera()
{
  return mSelectedCamera;
}

CameraActor SceneView::GetCamera(uint32_t index)
{
  if(index < mCameras.size())
  {
    return mCameras[index];
  }
  DALI_LOG_ERROR("Input index is out of bounds\n");
  return CameraActor();
}

CameraActor SceneView::GetCamera(const std::string& name)
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

void SceneView::RegisterModelView(Scene3D::ModelView modelView)
{
  if(modelView)
  {
    modelView.SetImageBasedLightTexture(mDiffuseTexture, mSpecularTexture, mIblScaleFactor);
    mModels.push_back(modelView);
  }
}

void SceneView::UnregisterModelView(Scene3D::ModelView modelView)
{
  if(modelView)
  {
    for(uint32_t i = 0; i < mModels.size(); ++i)
    {
      if(mModels[i] == modelView)
      {
        mModels.erase(mModels.begin() + i);
        break;
      }
    }
  }
}

void SceneView::SetImageBasedLightSource(const std::string& diffuse, const std::string& specular, float scaleFactor)
{
  Texture diffuseTexture = Dali::Scene3D::Loader::LoadCubeMap(diffuse);
  if(diffuseTexture)
  {
    Texture specularTexture = Dali::Scene3D::Loader::LoadCubeMap(specular);
    if(specularTexture)
    {
      mDiffuseTexture  = diffuseTexture;
      mSpecularTexture = specularTexture;
      mIblScaleFactor  = scaleFactor;

      for(auto&& model : mModels)
      {
        if(model)
        {
          model.SetImageBasedLightTexture(mDiffuseTexture, mSpecularTexture, mIblScaleFactor);
        }
      }
    }
  }
}

void SceneView::UseFramebuffer(bool useFramebuffer)
{
  if(mUseFrameBuffer != useFramebuffer)
  {
    mUseFrameBuffer = useFramebuffer;
    UpdateRenderTask();
  }
}

bool SceneView::IsUsingFramebuffer()
{
  return mUseFrameBuffer;
}

///////////////////////////////////////////////////////////
//
// Private methods
//

void SceneView::OnSceneConnection(int depth)
{
  UpdateRenderTask();

  Control::OnSceneConnection(depth);
}

void SceneView::OnSceneDisconnection()
{
  mModels.clear();
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

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  mRenderTask             = taskList.CreateTask();
  mRenderTask.SetSourceActor(mRootLayer);
  mRenderTask.SetExclusive(true);
  mRenderTask.SetInputEnabled(true);
  mRenderTask.SetCullMode(false);
  mRenderTask.SetScreenToFrameBufferMappingActor(Self());

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

    Vector3 size = Self().GetProperty<Vector3>(Dali::Actor::Property::SIZE);
    const float aspectRatio = size.width / size.height;
    mSelectedCamera.SetAspectRatio(aspectRatio);
    const float halfHeight = mSelectedCamera[Dali::CameraActor::Property::TOP_PLANE_DISTANCE];
    const float halfWidth = aspectRatio * halfHeight;
    mSelectedCamera[Dali::CameraActor::Property::LEFT_PLANE_DISTANCE]   = -halfWidth;
    mSelectedCamera[Dali::CameraActor::Property::RIGHT_PLANE_DISTANCE]  = halfWidth;
    mSelectedCamera[Dali::CameraActor::Property::TOP_PLANE_DISTANCE]    = halfHeight; // Top is +ve to keep consistency with orthographic values
    mSelectedCamera[Dali::CameraActor::Property::BOTTOM_PLANE_DISTANCE] = -halfHeight; // Bottom is -ve to keep consistency with orthographic values
    if(mUseFrameBuffer)
    {
      Dali::FrameBuffer currentFrameBuffer = mRenderTask.GetFrameBuffer();
      if(!currentFrameBuffer ||
         currentFrameBuffer.GetColorTexture().GetWidth() != size.width ||
         currentFrameBuffer.GetColorTexture().GetHeight() != size.height)
      {
        mRenderTask.ResetViewportGuideActor();
        mRenderTask.SetViewport(Dali::Viewport(Vector4::ZERO));

        // create offscreen buffer of new size to render our child actors to
        mTexture      = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, unsigned(size.width), unsigned(size.height));
        mRenderTarget = FrameBuffer::New(size.width, size.height, FrameBuffer::Attachment::DEPTH_STENCIL);
        mRenderTarget.AttachColorTexture(mTexture);
        Dali::Toolkit::ImageUrl imageUrl = Dali::Toolkit::Image::GenerateUrl(mRenderTarget, 0u);

        Property::Map imagePropertyMap;
        imagePropertyMap.Insert(Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE);
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::URL, imageUrl.GetUrl());
        // To flip rendered scene without CameraActor::SetInvertYAxis() to avoid backface culling.
        imagePropertyMap.Insert(Toolkit::ImageVisual::Property::PIXEL_AREA, Vector4(0.0f, 1.0f, 1.0f, -1.0f));
        mVisual = Toolkit::VisualFactory::Get().CreateVisual(imagePropertyMap);

        Toolkit::DevelControl::RegisterVisual(*this, RENDERING_BUFFER, mVisual);

        mRenderTask.SetFrameBuffer(mRenderTarget);
        mRenderTask.SetClearEnabled(true);
        mRenderTask.SetClearColor(Color::TRANSPARENT);
      }
    }
    else
    {
      mRenderTask.SetViewportGuideActor(Self());
      if(mRenderTask.GetFrameBuffer())
      {
        FrameBuffer framebuffer;
        mRenderTask.SetFrameBuffer(framebuffer);
        mRenderTask.SetClearEnabled(false);
      }
    }
  }
}

} // namespace Internal
} // namespace Scene3D
} // namespace Dali
