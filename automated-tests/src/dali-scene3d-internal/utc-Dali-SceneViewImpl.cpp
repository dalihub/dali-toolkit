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

#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-event-thread-callback.h>
#include <toolkit-timer.h>

#include <dali-toolkit/dali-toolkit.h>
#include <stdlib.h>
#include <iostream>

#include <dali-scene3d/public-api/controls/model/model.h>
#include <dali-scene3d/public-api/controls/scene-view/scene-view.h>
#include <dali-scene3d/internal/controls/scene-view/scene-view-impl.h>
#include <dali/devel-api/actors/camera-actor-devel.h>


using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

// Functor to test whether a Finish signal is emitted
struct TransitionFinishCheck
{
  TransitionFinishCheck(bool& signalReceived)
  : mSignalReceived(signalReceived)
  {
  }

  void operator()(Scene3D::SceneView& sceneView)
  {
    mSignalReceived = true;
  }

  void Reset()
  {
    mSignalReceived = false;
  }

  void CheckSignalReceived()
  {
    if(!mSignalReceived)
    {
      tet_printf("Expected Finish signal was not received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  void CheckSignalNotReceived()
  {
    if(mSignalReceived)
    {
      tet_printf("Unexpected Finish signal was received\n");
      tet_result(TET_FAIL);
    }
    else
    {
      tet_result(TET_PASS);
    }
  }

  bool& mSignalReceived; // owned by individual tests
};

}

int UtcDaliSceneViewImplCameraTransitionFail(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  application.GetScene().Add(view);

  CameraActor camera1 = CameraActor::New3DCamera();
  camera1.SetProperty(Dali::Actor::Property::NAME, "camera1");
  view.AddCamera(camera1);
  DALI_TEST_CHECK(!camera1.GetParent());
  view.SelectCamera("camera1");
  DALI_TEST_CHECK(camera1.GetParent());
  DALI_TEST_EQUALS(camera1, view.GetSelectedCamera(), TEST_LOCATION);

  camera1.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * -50.0f);
  view.Add(camera1);

  bool                  signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  view.CameraTransitionFinishedSignal().Connect(&application, finishCheck);

  view.StartCameraTransition("camera1", 1.0f);

  application.SendNotification();
  application.Render(500);
  application.SendNotification();

  // We didn't expect the animation to finish yet
  finishCheck.CheckSignalNotReceived();

  application.SendNotification();
  application.Render(600);
  application.SendNotification();

  finishCheck.CheckSignalNotReceived();

  CameraActor finalCamera = view.GetSelectedCamera();
  DALI_TEST_EQUALS(finalCamera, camera1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewImplCameraChangeDuringTransition(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  application.GetScene().Add(view);

  CameraActor camera1 = CameraActor::New3DCamera();
  camera1.SetProperty(Dali::Actor::Property::NAME, "camera1");
  view.AddCamera(camera1);
  DALI_TEST_CHECK(!camera1.GetParent());
  view.SelectCamera("camera1");
  DALI_TEST_CHECK(camera1.GetParent());
  DALI_TEST_EQUALS(camera1, view.GetSelectedCamera(), TEST_LOCATION);

  camera1.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * -50.0f);
  view.Add(camera1);

  CameraActor camera2 = CameraActor::New3DCamera();
  camera2.SetProperty(Dali::Actor::Property::NAME, "camera2");
  view.AddCamera(camera2);
  camera2.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * 100.0f);

  application.SendNotification();
  application.Render();

  bool                  signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  view.CameraTransitionFinishedSignal().Connect(&application, finishCheck);

  view.StartCameraTransition("camera2", 1.0f);

  CameraActor camera3 = CameraActor::New3DCamera();
  camera3.SetProperty(Dali::Actor::Property::NAME, "camera3");
  view.AddCamera(camera3);
  view.SelectCamera("camera3");

  DALI_TEST_NOT_EQUALS(camera3, view.GetSelectedCamera(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(camera2, view.GetSelectedCamera(), TEST_LOCATION);

  application.SendNotification();
  application.Render(500);
  application.SendNotification();

  // We didn't expect the animation to finish yet
  finishCheck.CheckSignalNotReceived();

  Dali::Scene3D::Internal::SceneView& sceneViewImpl = Dali::Scene3D::GetImpl(view);
  auto renderTask = sceneViewImpl.GetRenderTask();
  DALI_TEST_CHECK(renderTask);

  CameraActor currentCamera = renderTask.GetCameraActor();
  DALI_TEST_CHECK(currentCamera);
  DALI_TEST_NOT_EQUALS(currentCamera, camera1, 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(currentCamera, camera2, 0.0f, TEST_LOCATION);

  view.SelectCamera(view.GetCameraCount() - 1);
  DALI_TEST_NOT_EQUALS(camera3, view.GetSelectedCamera(), 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(currentCamera, view.GetSelectedCamera(), 0.0f, TEST_LOCATION);

  application.SendNotification();
  application.Render(600);
  application.SendNotification();

  finishCheck.CheckSignalReceived();

  CameraActor finalCamera = view.GetSelectedCamera();
  DALI_TEST_EQUALS(finalCamera, camera2, TEST_LOCATION);

  view.SelectCamera("camera3");
  DALI_TEST_EQUALS(camera3, view.GetSelectedCamera(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewImplStartCameraTransitionDuringTransition(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  application.GetScene().Add(view);

  CameraActor camera1 = CameraActor::New3DCamera();
  camera1.SetProperty(Dali::Actor::Property::NAME, "camera1");
  view.AddCamera(camera1);
  DALI_TEST_CHECK(!camera1.GetParent());
  view.SelectCamera("camera1");
  DALI_TEST_CHECK(camera1.GetParent());
  DALI_TEST_EQUALS(camera1, view.GetSelectedCamera(), TEST_LOCATION);

  camera1.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * -50.0f);
  view.Add(camera1);

  CameraActor camera2 = CameraActor::New3DCamera();
  camera2.SetProperty(Dali::Actor::Property::NAME, "camera2");
  view.AddCamera(camera2);
  camera2.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * 100.0f);

  application.SendNotification();
  application.Render();

  bool                  signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  view.CameraTransitionFinishedSignal().Connect(&application, finishCheck);

  view.StartCameraTransition("camera2", 1.0f);

  application.SendNotification();
  application.Render(500);
  application.SendNotification();

  // We didn't expect the animation to finish yet
  finishCheck.CheckSignalNotReceived();

  CameraActor camera3 = CameraActor::New3DCamera();
  camera3.SetProperty(Dali::Actor::Property::NAME, "camera3");
  view.AddCamera(camera3);

  view.StartCameraTransition("camera3", 1.0f);

  application.SendNotification();
  application.Render(600);
  application.SendNotification();

  finishCheck.CheckSignalReceived();

  CameraActor finalCamera = view.GetSelectedCamera();
  DALI_TEST_EQUALS(finalCamera, camera2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewImplCameraTransition1(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  application.GetScene().Add(view);

  CameraActor camera1 = CameraActor::New3DCamera();
  camera1.SetProperty(Dali::Actor::Property::NAME, "camera1");
  view.AddCamera(camera1);
  DALI_TEST_CHECK(!camera1.GetParent());
  view.SelectCamera("camera1");
  DALI_TEST_CHECK(camera1.GetParent());
  DALI_TEST_EQUALS(camera1, view.GetSelectedCamera(), TEST_LOCATION);

  Scene3D::Model model1 = Scene3D::Model::New();
  model1.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * -50.0f);
  view.Add(model1);

  camera1.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * -50.0f);
  camera1.SetNearClippingPlane(10.0f);
  camera1.SetFarClippingPlane(100.0f);
  model1.Add(camera1);

  CameraActor camera2 = CameraActor::New3DCamera();
  camera2.SetProperty(Dali::Actor::Property::NAME, "camera2");
  view.AddCamera(camera2);
  camera2.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * 100.0f);
  camera2.SetNearClippingPlane(5.0f);
  camera2.SetFarClippingPlane(50.0f);

  Scene3D::Model model2 = Scene3D::Model::New();
  model2.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * 100.0f);
  view.Add(model2);
  model2.Add(camera2);

  application.SendNotification();
  application.Render();

  camera1.SetProperty(Dali::DevelCameraActor::Property::PROJECTION_DIRECTION, Dali::DevelCameraActor::ProjectionDirection::VERTICAL);
  camera2.SetProperty(Dali::DevelCameraActor::Property::PROJECTION_DIRECTION, Dali::DevelCameraActor::ProjectionDirection::HORIZONTAL);
  camera1.SetFieldOfView(1.0f); // Vertical : 1.0f, Horizontal : 0.533293254f
  camera2.SetFieldOfView(1.0f); // Vertical : 1.65924551f, Horizontal : 1.0f
  camera1.SetAspectRatio(0.5f);
  camera2.SetAspectRatio(0.5f);
  tet_printf("camera1 fov : %f\n", camera1.GetFieldOfView());
  tet_printf("camera2 fov : %f\n", camera2.GetFieldOfView());
  tet_printf("camera1 aspect : %f\n", camera1.GetAspectRatio());
  tet_printf("camera2 aspect : %f\n", camera2.GetAspectRatio());
  tet_printf("camera1 direction : %d\n", camera1.GetProperty<Dali::DevelCameraActor::ProjectionDirection>(Dali::DevelCameraActor::Property::PROJECTION_DIRECTION));
  tet_printf("camera2 direction : %d\n", camera2.GetProperty<Dali::DevelCameraActor::ProjectionDirection>(Dali::DevelCameraActor::Property::PROJECTION_DIRECTION));

  bool                  signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  view.CameraTransitionFinishedSignal().Connect(&application, finishCheck);

  view.StartCameraTransition("camera2", 1.0f);

  application.SendNotification();
  application.Render(500);
  application.SendNotification();

  // We didn't expect the animation to finish yet
  finishCheck.CheckSignalNotReceived();


  Dali::Scene3D::Internal::SceneView& sceneViewImpl = Dali::Scene3D::GetImpl(view);
  auto renderTask = sceneViewImpl.GetRenderTask();
  DALI_TEST_CHECK(renderTask);

  CameraActor currentCamera = renderTask.GetCameraActor();
  DALI_TEST_CHECK(currentCamera);
  DALI_TEST_NOT_EQUALS(currentCamera, camera1, 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(currentCamera, camera2, 0.0f, TEST_LOCATION);

  Vector3 currentPosition = currentCamera.GetCurrentProperty<Vector3>(Dali::Actor::Property::POSITION);
  DALI_TEST_EQUALS(currentPosition, Vector3::ONE * 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(currentCamera.GetNearClippingPlane(), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(currentCamera.GetFarClippingPlane(), 100.0f, TEST_LOCATION);
  float currentFov =  (0.533293254f + 1.0f) / 2.0f;
  DALI_TEST_EQUALS(currentCamera.GetProperty<Dali::DevelCameraActor::ProjectionDirection>(Dali::DevelCameraActor::Property::PROJECTION_DIRECTION), Dali::DevelCameraActor::ProjectionDirection::HORIZONTAL, TEST_LOCATION);
  DALI_TEST_EQUALS(currentCamera.GetCurrentProperty<float>(Dali::CameraActor::Property::FIELD_OF_VIEW), currentFov, 0.05f, TEST_LOCATION);

  application.SendNotification();
  application.Render(600);
  application.SendNotification();

  finishCheck.CheckSignalReceived();

  CameraActor finalCamera = view.GetSelectedCamera();
  DALI_TEST_EQUALS(finalCamera, camera2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneViewImplCameraTransition2(void)
{
  ToolkitTestApplication application;

  Scene3D::SceneView view = Scene3D::SceneView::New();
  application.GetScene().Add(view);

  CameraActor camera1 = CameraActor::New3DCamera();
  camera1.SetProperty(Dali::Actor::Property::NAME, "camera1");
  view.AddCamera(camera1);
  DALI_TEST_CHECK(!camera1.GetParent());
  view.SelectCamera("camera1");
  DALI_TEST_CHECK(camera1.GetParent());
  DALI_TEST_EQUALS(camera1, view.GetSelectedCamera(), TEST_LOCATION);

  Scene3D::Model model1 = Scene3D::Model::New();
  model1.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * -50.0f);
  view.Add(model1);

  camera1.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * -50.0f);
  camera1.SetNearClippingPlane(10.0f);
  camera1.SetFarClippingPlane(100.0f);
  model1.Add(camera1);

  CameraActor camera2 = CameraActor::New3DCamera();
  camera2.SetProperty(Dali::Actor::Property::NAME, "camera2");
  view.AddCamera(camera2);
  camera2.SetProperty(Dali::Actor::Property::POSITION, Vector3::ONE * 200.0f);
  camera2.SetNearClippingPlane(5.0f);
  camera2.SetFarClippingPlane(50.0f);
  // Camera2 is not added on SceneView, it will added on root layer automatically.

  application.SendNotification();
  application.Render();

  bool                  signalReceived(false);
  TransitionFinishCheck finishCheck(signalReceived);
  view.CameraTransitionFinishedSignal().Connect(&application, finishCheck);

  camera1.SetProjectionMode(Dali::Camera::ProjectionMode::ORTHOGRAPHIC_PROJECTION);
  camera2.SetProjectionMode(Dali::Camera::ProjectionMode::ORTHOGRAPHIC_PROJECTION);
  camera1.SetProperty(Dali::DevelCameraActor::Property::PROJECTION_DIRECTION, Dali::DevelCameraActor::ProjectionDirection::VERTICAL);
  camera2.SetProperty(Dali::DevelCameraActor::Property::PROJECTION_DIRECTION, Dali::DevelCameraActor::ProjectionDirection::HORIZONTAL);

  camera1.SetProperty(Dali::DevelCameraActor::Property::ORTHOGRAPHIC_SIZE, 10.0f);  // Vertical : 10.0f, Horizontal : 5.0f
  camera2.SetProperty(Dali::DevelCameraActor::Property::ORTHOGRAPHIC_SIZE, 10.0f);  // Vertical : 20.0f, Horizontal : 10.0f
  camera1.SetAspectRatio(0.5f);
  camera2.SetAspectRatio(0.5f);

  tet_printf("camera1 fov : %f\n", camera1.GetProperty<float>(Dali::DevelCameraActor::Property::ORTHOGRAPHIC_SIZE));
  tet_printf("camera2 fov : %f\n", camera2.GetProperty<float>(Dali::DevelCameraActor::Property::ORTHOGRAPHIC_SIZE));
  tet_printf("camera1 aspect : %f\n", camera1.GetAspectRatio());
  tet_printf("camera2 aspect : %f\n", camera2.GetAspectRatio());
  tet_printf("camera1 direction : %f\n", camera1.GetProperty<Dali::DevelCameraActor::ProjectionDirection>(Dali::DevelCameraActor::Property::PROJECTION_DIRECTION));
  tet_printf("camera2 direction : %f\n", camera2.GetProperty<Dali::DevelCameraActor::ProjectionDirection>(Dali::DevelCameraActor::Property::PROJECTION_DIRECTION));

  int camera2Index = view.GetCameraCount() - 1;
  view.StartCameraTransition(camera2Index, 1.0f);

  application.SendNotification();
  application.Render(500);
  application.SendNotification();

  // We didn't expect the animation to finish yet
  finishCheck.CheckSignalNotReceived();

  Dali::Scene3D::Internal::SceneView& sceneViewImpl = Dali::Scene3D::GetImpl(view);
  auto renderTask = sceneViewImpl.GetRenderTask();
  DALI_TEST_CHECK(renderTask);

  CameraActor currentCamera = renderTask.GetCameraActor();
  DALI_TEST_CHECK(currentCamera);
  DALI_TEST_NOT_EQUALS(currentCamera, camera1, 0.0f, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(currentCamera, camera2, 0.0f, TEST_LOCATION);

  Vector3 currentPosition = currentCamera.GetCurrentProperty<Vector3>(Dali::Actor::Property::POSITION);
  DALI_TEST_EQUALS(currentPosition, Vector3::ONE * 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(currentCamera.GetNearClippingPlane(), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(currentCamera.GetFarClippingPlane(), 100.0f, TEST_LOCATION);
  float currentOrthographicSize = (5.0f + 10.0f) / 2.0f;
  DALI_TEST_EQUALS(currentCamera.GetProperty<Dali::DevelCameraActor::ProjectionDirection>(Dali::DevelCameraActor::Property::PROJECTION_DIRECTION), Dali::DevelCameraActor::ProjectionDirection::HORIZONTAL, TEST_LOCATION);
  DALI_TEST_EQUALS(currentCamera.GetCurrentProperty<float>(Dali::DevelCameraActor::Property::ORTHOGRAPHIC_SIZE), currentOrthographicSize, 0.05f, TEST_LOCATION);

  application.SendNotification();
  application.Render(600);
  application.SendNotification();

  finishCheck.CheckSignalReceived();

  CameraActor finalCamera = view.GetSelectedCamera();
  DALI_TEST_EQUALS(finalCamera, camera2, TEST_LOCATION);

  END_TEST;
}
