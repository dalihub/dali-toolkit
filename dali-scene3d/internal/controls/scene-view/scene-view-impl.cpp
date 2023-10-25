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

Property::Index    RENDERING_BUFFER        = Dali::Toolkit::Control::CONTROL_PROPERTY_END_INDEX + 1;
constexpr int32_t  DEFAULT_ORIENTATION     = 0;
constexpr int32_t  INVALID_INDEX           = -1;
constexpr uint32_t MAXIMUM_SIZE_SHADOW_MAP = 2048;

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

void SetShadowLightConstraint(Dali::CameraActor selectedCamera, Dali::CameraActor shadowLightCamera)
{
  shadowLightCamera.SetProperty(Dali::CameraActor::Property::ASPECT_RATIO, 1.0f);
  shadowLightCamera.SetProperty(Dali::DevelCameraActor::Property::ORTHOGRAPHIC_SIZE, 1.0f);
  shadowLightCamera.SetProperty(Dali::CameraActor::Property::NEAR_PLANE_DISTANCE, 0.5f);
  shadowLightCamera.SetProperty(Dali::CameraActor::Property::FAR_PLANE_DISTANCE, 3.5f);

  //< Make constraint for above properties.
  shadowLightCamera.RemoveConstraints();

  // Compute ViewProjectionMatrix and store it to "tempViewProjectionMatrix" property
  auto       tempViewProjectionMatrixIndex  = shadowLightCamera.RegisterProperty("tempViewProjectionMatrix", Matrix::IDENTITY);
  Constraint projectionMatrixConstraint = Constraint::New<Matrix>(shadowLightCamera, tempViewProjectionMatrixIndex, [](Matrix& output, const PropertyInputContainer& inputs)
                                                                  {
                                                                    Matrix worldMatrix = inputs[0]->GetMatrix();
                                                                    float tangentFov_2 = tanf(inputs[4]->GetFloat());
                                                                    float  nearDistance = inputs[5]->GetFloat();
                                                                    float  farDistance  = inputs[6]->GetFloat();
                                                                    float  aspectRatio  = inputs[7]->GetFloat();
                                                                    float  nearY        = 0.0f;
                                                                    float  nearX        = 0.0f;
                                                                    float  farY         = 0.0f;
                                                                    float  farX         = 0.0f;
                                                                    if(inputs[1]->GetInteger() == Dali::Camera::ProjectionMode::PERSPECTIVE_PROJECTION)
                                                                    {
                                                                      if(inputs[2]->GetInteger() == Dali::DevelCameraActor::ProjectionDirection::VERTICAL)
                                                                      {
                                                                        nearY = tangentFov_2 * nearDistance;
                                                                        nearX = nearY * aspectRatio;
                                                                        farY  = tangentFov_2 * farDistance;
                                                                        farX  = farY * aspectRatio;
                                                                      }
                                                                      else
                                                                      {
                                                                        nearX = tangentFov_2 * nearDistance;
                                                                        nearY = nearX / aspectRatio;
                                                                        farX  = tangentFov_2 * farDistance;
                                                                        farY  = farX / aspectRatio;
                                                                      }
                                                                    }
                                                                    else
                                                                    {
                                                                      if(inputs[2]->GetInteger() == Dali::DevelCameraActor::ProjectionDirection::VERTICAL)
                                                                      {
                                                                        nearY = inputs[3]->GetFloat();
                                                                        nearX = nearY * aspectRatio;
                                                                      }
                                                                      else
                                                                      {
                                                                        nearX = inputs[3]->GetFloat();
                                                                        nearY = nearX / aspectRatio;
                                                                      }
                                                                      farX = nearX;
                                                                      farY = nearY;
                                                                    }

                                                                    std::vector<Vector4> points;
                                                                    points.push_back(Vector4(nearX, nearY, nearDistance, 1.0f));
                                                                    points.push_back(Vector4(-nearX, nearY, nearDistance, 1.0f));
                                                                    points.push_back(Vector4(-nearX, -nearY, nearDistance, 1.0f));
                                                                    points.push_back(Vector4(nearX, -nearY, nearDistance, 1.0f));
                                                                    points.push_back(Vector4(farX, farY, farDistance, 1.0f));
                                                                    points.push_back(Vector4(-farX, farY, farDistance, 1.0f));
                                                                    points.push_back(Vector4(-farX, -farY, farDistance, 1.0f));
                                                                    points.push_back(Vector4(farX, -farY, farDistance, 1.0f));

                                                                    Matrix shadowCameraWorldMatrix = inputs[8]->GetMatrix();
                                                                    Vector4 worldCenter;
                                                                    for(auto&& point : points)
                                                                    {
                                                                      point = worldMatrix * point;
                                                                      worldCenter += point;
                                                                    }
                                                                    worldCenter /= 8.0f;
                                                                    shadowCameraWorldMatrix.SetTranslation(Vector3(worldCenter));
                                                                    Matrix shadowCameraViewMatrix = shadowCameraWorldMatrix;
                                                                    shadowCameraViewMatrix.Invert();

                                                                    Vector3 areaMin = Vector3::ONE * MAXFLOAT, areaMax = Vector3::ONE * -MAXFLOAT;
                                                                    for(auto&& point : points)
                                                                    {
                                                                      Vector4 pointV = shadowCameraViewMatrix * point;
                                                                      areaMin.x      = std::min(areaMin.x, pointV.x);
                                                                      areaMin.y      = std::min(areaMin.y, pointV.y);
                                                                      areaMin.z      = std::min(areaMin.z, pointV.z);
                                                                      areaMax.x      = std::max(areaMax.x, pointV.x);
                                                                      areaMax.y      = std::max(areaMax.y, pointV.y);
                                                                      areaMax.z      = std::max(areaMax.z, pointV.z);
                                                                    }

                                                                    Vector2 center        = Vector2(areaMax + areaMin) * 0.5;
                                                                    float   delta         = std::max(std::abs(areaMax.x - areaMin.x), std::abs(areaMax.y - areaMin.y));
                                                                    float   delta_2       = delta * 0.5f;
                                                                    Vector2 squareAreaMin = center - Vector2::ONE * delta_2;
                                                                    Vector2 squareAreaMax = center + Vector2::ONE * delta_2;
                                                                    float   deltaZ        = areaMax.z - areaMin.z;

                                                                    float right  = -squareAreaMin.x;
                                                                    float left   = -squareAreaMax.x;
                                                                    float top    = squareAreaMin.y;
                                                                    float bottom = squareAreaMax.y;
                                                                    float near   = areaMin.z;
                                                                    float far    = areaMax.z;

                                                                    float* projMatrix = output.AsFloat();

                                                                    projMatrix[0] = -2.0f / delta;
                                                                    projMatrix[1] = 0.0f;
                                                                    projMatrix[2] = 0.0f;
                                                                    projMatrix[3] = 0.0f;

                                                                    projMatrix[4] = 0.0f;
                                                                    projMatrix[5] = -2.0f / delta;
                                                                    projMatrix[6] = 0.0f;
                                                                    projMatrix[7] = 0.0f;

                                                                    projMatrix[8]  = 0.0f;
                                                                    projMatrix[9]  = 0.0f;
                                                                    projMatrix[10] = 2.0f / deltaZ;
                                                                    projMatrix[11] = 0.0f;

                                                                    projMatrix[12] = -(right + left) / delta;
                                                                    projMatrix[13] = -(top + bottom) / delta;
                                                                    projMatrix[14] = -(near + far) / deltaZ;
                                                                    projMatrix[15] = 1.0f;

                                                                    output = output * shadowCameraViewMatrix;
                                                                  });
  projectionMatrixConstraint.AddSource(Source{selectedCamera, Dali::Actor::Property::WORLD_MATRIX});
  projectionMatrixConstraint.AddSource(Source{selectedCamera, Dali::CameraActor::Property::PROJECTION_MODE});
  projectionMatrixConstraint.AddSource(Source{selectedCamera, Dali::DevelCameraActor::Property::PROJECTION_DIRECTION});
  projectionMatrixConstraint.AddSource(Source{selectedCamera, Dali::DevelCameraActor::Property::ORTHOGRAPHIC_SIZE});
  projectionMatrixConstraint.AddSource(Source{selectedCamera, Dali::CameraActor::Property::FIELD_OF_VIEW});
  projectionMatrixConstraint.AddSource(Source{selectedCamera, Dali::CameraActor::Property::NEAR_PLANE_DISTANCE});
  projectionMatrixConstraint.AddSource(Source{selectedCamera, Dali::CameraActor::Property::FAR_PLANE_DISTANCE});
  projectionMatrixConstraint.AddSource(Source{selectedCamera, Dali::CameraActor::Property::ASPECT_RATIO});
  projectionMatrixConstraint.AddSource(Source{shadowLightCamera, Dali::Actor::Property::WORLD_MATRIX});
  projectionMatrixConstraint.ApplyPost();
}

} // anonymous namespace

SceneView::SceneView()
: Control(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mWindowOrientation(DEFAULT_ORIENTATION),
  mSkybox(),
  mSkyboxOrientation(Quaternion()),
  mSkyboxIntensity(1.0f),
  mShaderManager(new Scene3D::Loader::ShaderManager())
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
    item->NotifyShadowMapTexture(mShadowTexture);
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
  bool enabled = mShaderManager->AddLight(light);
  mLights.push_back(std::make_pair(light, enabled));

  if(light.IsShadowEnabled())
  {
    SetShadow(light);
  }
}

void SceneView::RemoveLight(Scene3D::Light light)
{
  mShaderManager->RemoveLight(light);
  for(uint32_t i = 0; i < mLights.size(); ++i)
  {
    if(mLights[i].first == light)
    {
      mLights.erase(mLights.begin() + i);
      break;
    }
  }

  if(mLights.size() > mShaderManager->GetLightCount())
  {
    for(auto&& waitingLight : mLights)
    {
      if(waitingLight.second)
      {
        continue;
      }

      waitingLight.second = mShaderManager->AddLight(waitingLight.first);
      break;
    }
  }

  if(light == mShadowLight)
  {
    RemoveShadow(light);
  }

  if(!mShadowLight)
  {
    for(auto&& lightEntity : mLights)
    {
      if(!lightEntity.second || !lightEntity.first.IsShadowEnabled())
      {
        continue;
      }
      SetShadow(lightEntity.first);
      break;
    }
  }
}

void SceneView::SetShadow(Scene3D::Light light)
{
  if(!!mShadowLight)
  {
    return;
  }

  auto foundLight = std::find_if(mLights.begin(), mLights.end(), [light](std::pair<Scene3D::Light, bool> lightEntity) -> bool
                                 { return (lightEntity.second && lightEntity.first == light); });

  if(foundLight == mLights.end())
  {
    return;
  }

  mShadowLight = light;

  // Directional Light setting.
  CameraActor lightCamera    = GetImplementation(light).GetCamera();
  CameraActor selectedCamera = GetSelectedCamera();
  SetShadowLightConstraint(selectedCamera, lightCamera);

  // make framebuffer for depth map and set it to render task.
  Vector3  size               = Self().GetProperty<Vector3>(Dali::Actor::Property::SIZE);
  uint32_t shadowMapBufferSize = std::min(static_cast<uint32_t>(std::max(size.width, size.height)), MAXIMUM_SIZE_SHADOW_MAP);
  UpdateShadowMapBuffer(shadowMapBufferSize);

  // use lightCamera as a camera of shadow render task.
  mShadowMapRenderTask.SetCameraActor(lightCamera);

  mShaderManager->SetShadow(light);
}

void SceneView::RemoveShadow(Scene3D::Light light)
{
  if(mShadowLight != light)
  {
    return;
  }

  // remove all constraint from light camera
  CameraActor lightCamera = GetImplementation(mShadowLight).GetCamera();
  lightCamera.RemoveConstraints();

  // reset framebuffer and remove it from render task.
  mShadowFrameBuffer.Reset();
  mShaderManager->RemoveShadow();
  mShadowMapRenderTask.SetCameraActor(CameraActor());

  mShadowLight.Reset();

  mShadowTexture.Reset();
  for(auto&& item : mItems)
  {
    if(item)
    {
      item->NotifyShadowMapTexture(mShadowTexture);
    }
  }

  for(auto&& lightEntity : mLights)
  {
    if(!lightEntity.second || !lightEntity.first.IsShadowEnabled())
    {
      continue;
    }
    SetShadow(lightEntity.first);
    break;
  }
}

uint32_t SceneView::GetActivatedLightCount() const
{
  return mShaderManager->GetLightCount();
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

Dali::Scene3D::Loader::ShaderManagerPtr SceneView::GetShaderManager() const
{
  return mShaderManager;
}

void SceneView::UpdateShadowUniform(Scene3D::Light light)
{
  mShaderManager->UpdateShadowUniform(light);
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
    mShadowMapRenderTask    = taskList.CreateTask();
    mShadowMapRenderTask.SetSourceActor(mRootLayer);
    mShadowMapRenderTask.SetExclusive(true);
    mShadowMapRenderTask.SetInputEnabled(false);
    mShadowMapRenderTask.SetCullMode(false);
    mShadowMapRenderTask.SetClearEnabled(true);
    mShadowMapRenderTask.SetClearColor(Color::WHITE);
    mShadowMapRenderTask.SetRenderPassTag(10);
    mShadowMapRenderTask.SetCameraActor(CameraActor());

    mRenderTask = taskList.CreateTask();
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
    if(mShadowMapRenderTask)
    {
      RenderTaskList taskList = mSceneHolder.GetRenderTaskList();
      taskList.RemoveTask(mShadowMapRenderTask);
      mShadowMapRenderTask.Reset();
    }
    mSceneHolder.Reset();
  }
  mFrameBuffer.Reset();
  mShadowFrameBuffer.Reset();

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
  if(mShadowLight)
  {
    SetShadowLightConstraint(mSelectedCamera, GetImplementation(mShadowLight).GetCamera());
  }
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

    uint32_t shadowMapBufferSize = std::min(static_cast<uint32_t>(std::max(size.width, size.height)), MAXIMUM_SIZE_SHADOW_MAP);
    UpdateShadowMapBuffer(shadowMapBufferSize);

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

void SceneView::UpdateShadowMapBuffer(uint32_t shadowMapSize)
{
  Dali::FrameBuffer currentShadowFrameBuffer = mShadowMapRenderTask.GetFrameBuffer();
  if(mShadowLight &&
     (!currentShadowFrameBuffer ||
      !mShadowTexture ||
      !Dali::Equals(DevelFrameBuffer::GetDepthTexture(currentShadowFrameBuffer).GetWidth(), shadowMapSize)))
  {
    mShadowFrameBuffer.Reset();
    mShadowTexture     = Dali::Texture::New(TextureType::TEXTURE_2D, Pixel::DEPTH_UNSIGNED_INT, shadowMapSize, shadowMapSize);
    mShadowFrameBuffer = FrameBuffer::New(shadowMapSize, shadowMapSize, FrameBuffer::Attachment::NONE);
    DevelFrameBuffer::AttachDepthTexture(mShadowFrameBuffer, mShadowTexture);
    mShadowMapRenderTask.SetFrameBuffer(mShadowFrameBuffer);

    for(auto&& item : mItems)
    {
      if(item)
      {
        item->NotifyShadowMapTexture(mShadowTexture);
      }
    }
  }
}

} // namespace Internal
} // namespace Scene3D
} // namespace Dali
