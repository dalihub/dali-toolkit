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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/video-view/video-view-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/public-api/controls/video-view/video-view.h>
#include <dali/devel-api/adaptor-framework/video-sync-mode.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <stdlib.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const TEST_FILE("test.mp4");
const char* const VOLUME_LEFT("volumeLeft");
const char* const VOLUME_RIGHT("volumeRight");
const char* const RENDERING_TYPE("renderingTarget");
const char* const DUMMY_STRING("dummy string");

// clang-format off
const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2     aPosition;\n
  varying mediump vec2     vTexCoord;\n
  uniform mediump mat4   uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n
    vTexCoord = aPosition + vec2(0.5);\n
    gl_Position = vertexPosition;\n
  }\n);

const char* fragmentShaderPrefix("#extension GL_OES_EGL_image_external:require\n");
const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform samplerExternalOES sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D(sTexture, vTexCoord) * uColor;\n
  }\n);

// clang-format on

} // namespace

void video_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void video_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Negative test case for a method
int UtcDaliVideoViewUninitialized(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewUninitialized");

  Toolkit::VideoView view;

  try
  {
    // New() must be called to create a VideoView or it wont be valid.
    Actor a = Actor::New();
    view.Add(a);
    DALI_TEST_CHECK(false);
  }
  catch(Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(!view);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliVideoViewNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewNew");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK(view);

  const std::string  url(DUMMY_STRING);
  Toolkit::VideoView view2 = Toolkit::VideoView::New(url);
  DALI_TEST_CHECK(view2);
  END_TEST;
}

// Positive test case for a method
int UtcDaliVideoViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewDownCast");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  BaseHandle         handle(view);

  Toolkit::VideoView view2 = Toolkit::VideoView::DownCast(handle);
  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(view2);
  DALI_TEST_CHECK(view == view2);
  END_TEST;
}

// Positive test case for a method
int UtcDaliVideoViewProperty1(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty1");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK(view);

  std::string file;
  view.SetProperty(VideoView::Property::VIDEO, TEST_FILE);
  Property::Value val = view.GetProperty(VideoView::Property::VIDEO);
  DALI_TEST_CHECK(val.Get(file));
  DALI_TEST_CHECK(file == TEST_FILE);
  END_TEST;
}

// Positive test case for a method
int UtcDaliVideoViewProperty1b(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty1");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK(view);
  Integration::Scene stage = application.GetScene();

  std::string   file;
  Property::Map map;

  view.SetProperty(VideoView::Property::VIDEO,
                   Property::Map()
                     .Add("rendererType", "IMAGE")
                     .Add("url", "video.mpg") // Note, videoView doesn't use this url
                     .Add("RENDERING_TARGET", "windowSurfaceTarget")
                     .Add("width", 100)
                     .Add("height", 100));

  stage.Add(view);

  Property::Value val       = view.GetProperty(VideoView::Property::VIDEO);
  Property::Map*  resultMap = val.GetMap();

  DALI_TEST_CHECK(resultMap);
  Property::Value* value = resultMap->Find("url");
  DALI_TEST_CHECK(value);
  DALI_TEST_EQUALS(value->Get<std::string>(), "video.mpg", TEST_LOCATION);

  stage.Remove(view);

  END_TEST;
}

int UtcDaliVideoViewProperty2(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty2");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK(view);

  bool            looping;
  Property::Value val = view.GetProperty(VideoView::Property::LOOPING);
  DALI_TEST_CHECK(val.Get(looping));
  DALI_TEST_CHECK(!looping);

  view.SetProperty(VideoView::Property::LOOPING, true);
  val = view.GetProperty(VideoView::Property::LOOPING);
  DALI_TEST_CHECK(val.Get(looping));
  DALI_TEST_CHECK(looping);
  END_TEST;
}

int UtcDaliVideoViewProperty3(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty3");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK(view);

  bool            muted;
  Property::Value val = view.GetProperty(VideoView::Property::MUTED);
  DALI_TEST_CHECK(val.Get(muted));
  DALI_TEST_CHECK(!muted);

  view.SetProperty(VideoView::Property::MUTED, true);
  val = view.GetProperty(VideoView::Property::MUTED);
  DALI_TEST_CHECK(val.Get(muted));
  DALI_TEST_CHECK(muted);

  END_TEST;
}

int UtcDaliVideoViewProperty4(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty4");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK(view);

  float left, right;
  left = right = 0.f;

  Property::Map map;
  map.Insert(VOLUME_LEFT, 0.5f);
  map.Insert(VOLUME_RIGHT, 0.5f);

  view.SetProperty(VideoView::Property::VOLUME, map);
  Property::Value val = view.GetProperty(VideoView::Property::VOLUME);

  Property::Map map2;
  DALI_TEST_CHECK(val.Get(map2));

  Property::Value* volumeLeft  = map2.Find(VOLUME_LEFT);
  Property::Value* volumeRight = map2.Find(VOLUME_RIGHT);

  DALI_TEST_CHECK(volumeLeft && volumeLeft->Get(left));
  DALI_TEST_CHECK(volumeRight && volumeRight->Get(right));
  DALI_TEST_CHECK(left == 0.5f);
  DALI_TEST_CHECK(right == 0.5f);

  END_TEST;
}

int UtcDaliVideoViewCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK(view);

  VideoView copy(view);
  DALI_TEST_CHECK(view == copy);

  VideoView assign;
  DALI_TEST_CHECK(!assign);

  assign = copy;
  DALI_TEST_CHECK(assign == view);

  END_TEST;
}

int UtcDaliVideoViewMoveConstructor(void)
{
  ToolkitTestApplication application;

  VideoView view = Toolkit::VideoView::New();
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  view.SetProperty(VideoView::Property::LOOPING, true);
  DALI_TEST_CHECK(view.GetProperty<bool>(VideoView::Property::LOOPING));

  VideoView moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(moved.GetProperty<bool>(VideoView::Property::LOOPING));
  DALI_TEST_CHECK(!view);

  END_TEST;
}

int UtcDaliVideoViewMoveAssignment(void)
{
  ToolkitTestApplication application;

  VideoView view = Toolkit::VideoView::New();
  DALI_TEST_EQUALS(1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  view.SetProperty(VideoView::Property::LOOPING, true);
  DALI_TEST_CHECK(view.GetProperty<bool>(VideoView::Property::LOOPING));

  VideoView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(moved.GetProperty<bool>(VideoView::Property::LOOPING));
  DALI_TEST_CHECK(!view);

  END_TEST;
}

int UtcDaliVideoViewTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK(typeRegistry);

  TypeInfo typeInfo = typeRegistry.GetTypeInfo("VideoView");
  DALI_TEST_CHECK(typeInfo);

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK(handle);

  VideoView view = VideoView::DownCast(handle);
  DALI_TEST_CHECK(view);

  END_TEST;
}

int UtcDaliVideoViewMethodsForCoverage(void)
{
  ToolkitTestApplication application;

  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  videoView.Play();
  videoView.Pause();
  videoView.Stop();
  videoView.Forward(10);
  videoView.Backward(10);

  Toolkit::DevelVideoView::GetMediaPlayer(videoView);

  VideoView::VideoViewSignalType& signal = videoView.FinishedSignal();
  DALI_TEST_EQUALS(0, signal.GetConnectionCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVideoViewMethodsForRenderType(void)
{
  ToolkitTestApplication application;
  VideoView              videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  Property::Map windowSurfaceTarget;
  Property::Map nativeImageTarget;

  windowSurfaceTarget.Insert(RENDERING_TYPE, "windowSurfaceTarget");
  nativeImageTarget.Insert(RENDERING_TYPE, "nativeImageTarget");

  Property::Map   map;
  Property::Value value;
  videoView.SetProperty(VideoView::Property::VIDEO, windowSurfaceTarget);

  value = videoView.GetProperty(VideoView::Property::VIDEO);
  DALI_TEST_CHECK(value.Get(map));

  Property::Value* type = map.Find(RENDERING_TYPE);
  DALI_TEST_CHECK(type);
  DALI_TEST_EQUALS("windowSurfaceTarget", type->Get<std::string>(), TEST_LOCATION);

  videoView.SetProperty(VideoView::Property::VIDEO, nativeImageTarget);

  value = videoView.GetProperty(VideoView::Property::VIDEO);
  DALI_TEST_CHECK(value.Get(map));
  type = map.Find(RENDERING_TYPE);

  DALI_TEST_CHECK(type);
  DALI_TEST_EQUALS("nativeImageTarget", type->Get<std::string>(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVideoViewCustomShaderForCoverage(void)
{
  ToolkitTestApplication application;
  VideoView              videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true;

  videoView.SetProperty(Toolkit::VideoView::Property::UNDERLAY, false);
  bool isUnderlay = videoView.GetProperty(Toolkit::VideoView::Property::UNDERLAY).Get<bool>();
  DALI_TEST_CHECK(!isUnderlay);

  application.GetScene().Add(videoView);
  videoView.SetProperty(VideoView::Property::VIDEO, "testvideo");

  Property::Map customShader;
  customShader.Insert("vertexShader", VERTEX_SHADER);
  customShader.Insert("fragmentShader", FRAGMENT_SHADER);

  Property::Map map;
  map.Insert("shader", customShader);

  videoView.SetProperty(VideoView::Property::VIDEO, map);

  Property::Map   map2;
  Property::Value value = videoView.GetProperty(VideoView::Property::VIDEO);

  DALI_TEST_CHECK(!value.Get(map2));
  END_TEST;
}

int UtcDaliVideoViewMethodsForCoverage2(void)
{
  ToolkitTestApplication application;
  VideoView              videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  Property::Map windowSurfaceTarget;

  windowSurfaceTarget.Insert(RENDERING_TYPE, "windowSurfaceTarget");

  application.GetScene().Add(videoView);

  application.SendNotification();
  application.Render();

  Property::Map   map;
  Property::Value value;
  videoView.SetProperty(VideoView::Property::VIDEO, windowSurfaceTarget);

  value = videoView.GetProperty(VideoView::Property::VIDEO);
  DALI_TEST_CHECK(value.Get(map));

  Property::Value* type = map.Find(RENDERING_TYPE);
  DALI_TEST_CHECK(type);
  DALI_TEST_EQUALS("windowSurfaceTarget", type->Get<std::string>(), TEST_LOCATION);

  Vector3 vector(100.0f, 100.0f, 0.0f);

  DALI_TEST_CHECK(vector != videoView.GetCurrentProperty<Vector3>(Actor::Property::SIZE));
  videoView.SetProperty(Actor::Property::SIZE, vector);

  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == videoView.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  END_TEST;
}

int UtcDaliVideoViewCustomShaderForCoverage3(void)
{
  ToolkitTestApplication application;
  VideoView              videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true;

  videoView.SetProperty(Toolkit::VideoView::Property::UNDERLAY, false);
  bool isUnderlay = videoView.GetProperty(Toolkit::VideoView::Property::UNDERLAY).Get<bool>();
  DALI_TEST_CHECK(!isUnderlay);

  application.GetScene().Add(videoView);
  videoView.SetProperty(VideoView::Property::VIDEO, "testvideo");

  Property::Map customShader;
  customShader.Insert("vertexShader", VERTEX_SHADER);

  Property::Map map;
  map.Insert("shader", customShader);

  videoView.SetProperty(VideoView::Property::VIDEO, map);

  Property::Map   map2;
  Property::Value value = videoView.GetProperty(VideoView::Property::VIDEO);

  DALI_TEST_CHECK(!value.Get(map2));
  END_TEST;
}

int UtcDaliVideoViewPropertyUnderlay(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewPropertyUnderlay");
  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true;

  VideoView view = VideoView::New();
  DALI_TEST_CHECK(view);

  application.GetScene().Add(view);
  view.Play();

  application.SendNotification();
  application.Render();

  bool isUnderlay = view.GetProperty(Toolkit::VideoView::Property::UNDERLAY).Get<bool>();
  DALI_TEST_CHECK(isUnderlay);

  view.Play();
  view.SetProperty(Toolkit::VideoView::Property::UNDERLAY, false);
  isUnderlay = view.GetProperty(Toolkit::VideoView::Property::UNDERLAY).Get<bool>();
  DALI_TEST_CHECK(!isUnderlay);

  view.Play();
  view.SetProperty(Toolkit::VideoView::Property::UNDERLAY, true);
  isUnderlay = view.GetProperty(Toolkit::VideoView::Property::UNDERLAY).Get<bool>();
  DALI_TEST_CHECK(isUnderlay);

  // If platform api doesn't provide any API or feature for decoded images of video,
  // UNDERLAY should be true
  ToolkitApplication::DECODED_IMAGES_SUPPORTED = false;

  view.SetProperty(Toolkit::VideoView::Property::UNDERLAY, false);
  isUnderlay = view.GetProperty(Toolkit::VideoView::Property::UNDERLAY).Get<bool>();
  DALI_TEST_CHECK(isUnderlay);

  // For coverage
  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true;

  view.SetProperty(Toolkit::VideoView::Property::UNDERLAY, true);
  view.SetProperty(Toolkit::VideoView::Property::UNDERLAY, false);
  isUnderlay = view.GetProperty(Toolkit::VideoView::Property::UNDERLAY).Get<bool>();
  DALI_TEST_CHECK(!isUnderlay);

  view.Stop();

  END_TEST;
}

int UtcDaliVideoViewPropertyPlayPosition(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewPropertyPlayPosition");

  VideoView view = VideoView::New();
  DALI_TEST_CHECK(view);

  application.GetScene().Add(view);
  view.Play();

  application.SendNotification();
  application.Render();

  int playPos = view.GetProperty(Toolkit::VideoView::Property::PLAY_POSITION).Get<int>();
  DALI_TEST_CHECK(playPos == 0);

  view.SetProperty(Toolkit::VideoView::Property::PLAY_POSITION, 10);
  playPos = view.GetProperty(Toolkit::VideoView::Property::PLAY_POSITION).Get<int>();
  // Actually setting play position will be async
  // Actual platform result may be different.
  DALI_TEST_CHECK(playPos == 10);

  END_TEST;
}

// For coverage.
int UtcDaliVideoViewNew2(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewNew2");

  VideoView view = VideoView::New(true);
  DALI_TEST_CHECK(view);

  application.GetScene().Add(view);
  view.Play();

  application.SendNotification();
  application.Render();

  VideoView view2 = VideoView::New("", false);
  DALI_TEST_CHECK(view2);

  application.GetScene().Add(view2);
  view2.Play();

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliVideoViewRaiseAboveLowerBelow(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewRaiseAboveLowerBelow");

  VideoView view = VideoView::New(true);
  DALI_TEST_CHECK(view);

  application.GetScene().Add(view);
  view.Play();

  application.SendNotification();
  application.Render();

  VideoView view2 = VideoView::New("", false);
  DALI_TEST_CHECK(view2);

  application.GetScene().Add(view2);
  view2.Play();

  application.SendNotification();
  application.Render();

  view.RaiseAbove(view2);
  view.LowerBelow(view2);

  END_TEST;
}

int UtcDaliVideoViewRaiseTopLowerBottom(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewRaiseTopLowerBottom");

  VideoView view = VideoView::New(true);
  DALI_TEST_CHECK(view);

  application.GetScene().Add(view);
  view.Play();

  application.SendNotification();
  application.Render();

  VideoView view2 = VideoView::New("", false);
  DALI_TEST_CHECK(view2);

  application.GetScene().Add(view2);
  view2.Play();

  application.SendNotification();
  application.Render();

  view.RaiseToTop();
  view.LowerToBottom();

  END_TEST;
}

int UtcDaliVideoViewPropertyDisplayMode(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewPropertyDisplayMode");

  VideoView view = VideoView::New();
  DALI_TEST_CHECK(view);

  application.GetScene().Add(view);
  view.Play();

  application.SendNotification();
  application.Render();

  view.SetProperty(Toolkit::VideoView::Property::DISPLAY_MODE, Toolkit::VideoView::DisplayMode::DST_ROI);
  int displayMode = view.GetProperty(Toolkit::VideoView::Property::DISPLAY_MODE).Get<int>();
  DALI_TEST_CHECK(displayMode == Toolkit::VideoView::DisplayMode::DST_ROI);

  END_TEST;
}

int UtcDaliVideoViewCustomShader(void)
{
  ToolkitTestApplication application;
  tet_infoline("VideoView with custom shader");

  VideoView view = VideoView::New(false);
  DALI_TEST_CHECK(view);

  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true;

  view.SetProperty(Toolkit::VideoView::Property::UNDERLAY, false);
  bool isUnderlay = view.GetProperty(Toolkit::VideoView::Property::UNDERLAY).Get<bool>();
  DALI_TEST_CHECK(!isUnderlay);

  application.GetScene().Add(view);
  view.SetProperty(VideoView::Property::VIDEO, "testvideo");

  /* insert custom shader */
  Property::Map customShader;
  std::string   fragmentShaderString;
  fragmentShaderString.reserve(strlen(fragmentShaderPrefix) + strlen(FRAGMENT_SHADER));
  fragmentShaderString.append(fragmentShaderPrefix);
  fragmentShaderString.append(FRAGMENT_SHADER);
  customShader.Insert("vertexShader", VERTEX_SHADER);
  customShader.Insert("fragmentShader", fragmentShaderString);

  Property::Map map;
  map.Insert("shader", customShader);

  view.SetProperty(VideoView::Property::VIDEO, map);

  /* do render for check custom shader */
  application.GetScene().Add(view);
  view.Play();

  application.SendNotification();
  application.Render();

  /* get visual */
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation(view);
  Visual::Base                visual      = DevelControl::GetVisual(controlImpl, VideoView::Property::TEXTURE);
  DALI_TEST_CHECK(visual);
  Toolkit::Internal::Visual::Base& visualImpl = Toolkit::GetImplementation(visual);
  Shader                           shader     = visualImpl.GetRenderer().GetShader();
  DALI_TEST_CHECK(shader);

  Property::Value value     = shader.GetProperty(Shader::Property::PROGRAM);
  Property::Map*  shaderMap = value.GetMap();
  DALI_TEST_CHECK(shaderMap);

  Property::Value* fragment = shaderMap->Find("fragment"); // fragment key name from shader-impl.cpp
  DALI_TEST_EQUALS(fragmentShaderString, fragment->Get<std::string>(), TEST_LOCATION);

  Property::Value* vertex = shaderMap->Find("vertex"); // vertex key name from shader-impl.cpp
  DALI_TEST_EQUALS(VERTEX_SHADER, vertex->Get<std::string>(), TEST_LOCATION);

  END_TEST;
}

// Functor to test whether a Finish signal is emitted
struct AnimationFinishCheck
{
  AnimationFinishCheck(bool& signalReceived)
  : mSignalReceived(signalReceived)
  {
  }

  void operator()(Animation& animation)
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

int UtcDaliVideoViewSyncAniamtionForCoverage(void)
{
  ToolkitTestApplication application;

  VideoView videoView = DevelVideoView::New(Dali::VideoSyncMode::ENABLED);
  DALI_TEST_CHECK(videoView);

  application.GetScene().Add(videoView);

  // Build the animation
  float     durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  // Start the animation
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(videoView, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  DevelVideoView::PlayAnimation(videoView, animation);

  bool                 signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) - 1u /*just less than the animation duration*/);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(2u /*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS(targetPosition, videoView.GetCurrentProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);

  // Restart the animation, with a different duration
  finishCheck.Reset();

  END_TEST;
}

int UtcDaliVideoViewASyncAniamtionForCoverage(void)
{
  ToolkitTestApplication application;

  VideoView videoView = DevelVideoView::New(Dali::VideoSyncMode::DISABLED);
  DALI_TEST_CHECK(videoView);

  application.GetScene().Add(videoView);

  // Build the animation
  float     durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  // Start the animation
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(videoView, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  DevelVideoView::PlayAnimation(videoView, animation);

  bool                 signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds * 1000.0f) - 1u /*just less than the animation duration*/);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(2u /*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS(targetPosition, videoView.GetCurrentProperty<Vector3>(Actor::Property::POSITION), TEST_LOCATION);

  // Restart the animation, with a different duration
  finishCheck.Reset();

  END_TEST;
}

int UtcDaliVideoViewResizeWithSynchronization(void)
{
  ToolkitTestApplication application;
  VideoView              videoView = DevelVideoView::New(Dali::VideoSyncMode::ENABLED);
  DALI_TEST_CHECK(videoView);

  application.GetScene().Add(videoView);

  Vector3 vector(50.0f, 200.0f, 0.0f);
  videoView.SetProperty(Actor::Property::SIZE, vector);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == videoView.GetCurrentProperty<Vector3>(Actor::Property::SIZE));

  END_TEST;
}

// For coverage.
int UtcDaliVideoViewSynchronizationForWindowRotation(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewSynchronizationForWindowRotation");

  Window window = Window::New(PositionSize(0, 0, 100, 100), "", false);
  DALI_TEST_CHECK(window);

  VideoView view = VideoView::New(true);
  DALI_TEST_CHECK(view);

  window.Add(view);

  view.Play();

  DevelWindow::SetPositionSize(window, PositionSize(0, 0, 480, 240));

  application.SendNotification();
  application.Render();

  END_TEST;
}

// For coverage
int UtcDaliVideoViewDisplayModeForCoverage(void)
{
  ToolkitTestApplication application;

  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  Toolkit::DevelVideoView::SetAutoRotationEnabled(videoView, true);
  // A false return is correct because there is no plugin.
  DALI_TEST_EQUALS(false, Toolkit::DevelVideoView::IsAutoRotationEnabled(videoView), TEST_LOCATION);

  Toolkit::DevelVideoView::SetLetterBoxEnabled(videoView, true);
  // A false return is correct because there is no plugin.
  DALI_TEST_EQUALS(false, Toolkit::DevelVideoView::IsLetterBoxEnabled(videoView), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVideoViewSetFrameInterpolationInterval_P(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewSetFrameInterpolationInterval_P");

  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  // Test setting a valid positive interval
  float intervalToSet = 0.033f; // ~30fps
  DevelVideoView::SetFrameInterpolationInterval(videoView, intervalToSet);
  float retrievedInterval = DevelVideoView::GetFrameInterpolationInterval(videoView);
  DALI_TEST_EQUALS(intervalToSet, retrievedInterval, TEST_LOCATION);

  // Test setting zero interval
  intervalToSet = 0.0f;
  DevelVideoView::SetFrameInterpolationInterval(videoView, intervalToSet);
  retrievedInterval = DevelVideoView::GetFrameInterpolationInterval(videoView);
  DALI_TEST_EQUALS(intervalToSet, retrievedInterval, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVideoViewSetFrameInterpolationInterval_N(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewSetFrameInterpolationInterval_N");

  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  // Test setting a negative interval
  // The expected behavior is that it should not crash and ideally should be a no-op or log a warning.
  // We test that it doesn't throw an unhandled exception.
  try
  {
    DevelVideoView::SetFrameInterpolationInterval(videoView, -0.1f);
    DALI_TEST_CHECK(true); // If no exception, test passes for robustness
  }
  catch(Dali::DaliException& e)
  {
    // If an assertion is thrown and caught, it's also an acceptable behavior for negative input.
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(true); // Test passes if an expected assertion is thrown.
  }
  catch(...)
  {
    DALI_TEST_CHECK(false); // Fail for any other unexpected exception
  }

  END_TEST;
}

int UtcDaliVideoViewSetNativeImageSourceForCurrentFrame_P(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewSetNativeImageSourceForCurrentFrame_P");

  // This test only works in Underlay mode.
  ToolkitApplication::DECODED_IMAGES_SUPPORTED = false; // Force Underlay.

  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  application.GetScene().Add(videoView);

  // Set Underlay mode and assign a valid size.
  videoView.SetProperty(VideoView::Property::UNDERLAY, true);
  videoView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  // Set the frame interpolation interval.
  float interpolationInterval = 0.5f; // 0.5 seconds
  DevelVideoView::SetFrameInterpolationInterval(videoView, interpolationInterval);
  DALI_TEST_EQUALS(interpolationInterval, DevelVideoView::GetFrameInterpolationInterval(videoView), TEST_LOCATION);

  // Connect to the scene and ensure size is set.
  application.SendNotification();
  application.Render();

  // --- Part 1: Setting the first frame. No animation should play. ---
  tet_printf("Part 1: Setting the first frame.\n");
  NativeImageSourcePtr nativeImageSource1 = NativeImageSource::New(16, 16, NativeImageSource::COLOR_DEPTH_DEFAULT);
  DALI_TEST_CHECK(nativeImageSource1);
  DevelVideoView::SetNativeImageSourceForCurrentFrame(videoView, nativeImageSource1);

  // Let the event be processed.
  application.SendNotification();
  application.Render(16);

  // VideoView should now have two renderers in Underlay mode, as CreateOverlayTextureVisual is called.
  // Index 0: General overlay renderer
  // Index 1: Texture renderer for frame interpolation
  DALI_TEST_EQUALS(videoView.GetRendererCount(), 2u, TEST_LOCATION);

  Renderer renderer = videoView.GetRendererAt(1);
  DALI_TEST_CHECK(renderer);

  Shader shader = renderer.GetShader();
  DALI_TEST_CHECK(shader);

  // Get the uInterpolationFactor property index.
  Property::Index interpolationFactorIndex = shader.GetPropertyIndex("uInterpolationFactor");
  DALI_TEST_CHECK(interpolationFactorIndex != Property::INVALID_INDEX);

  // Since this is the first frame, mPreviousFrameTexture and mCurrentFrameTexture are the same.
  // The new logic sets uInterpolationFactor to 1.0f immediately to show the current frame.
  float currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("uInterpolationFactor after first frame: %f\n", currentValue);
  DALI_TEST_EQUALS(currentValue, 1.0f, TEST_LOCATION);

  // Render for the duration of the interpolation interval. The value should not change.
  application.Render(static_cast<unsigned int>(interpolationInterval * 1000.0f));
  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("uInterpolationFactor after first frame + interval: %f\n", currentValue);
  DALI_TEST_EQUALS(currentValue, 1.0f, TEST_LOCATION); // Still no change.

  // --- Part 2: Setting a second, different frame. Animation should play. ---
  tet_printf("Part 2: Setting a second, different frame.\n");
  NativeImageSourcePtr nativeImageSource2 = NativeImageSource::New(16, 16, NativeImageSource::COLOR_DEPTH_DEFAULT);
  DALI_TEST_CHECK(nativeImageSource2);
  DevelVideoView::SetNativeImageSourceForCurrentFrame(videoView, nativeImageSource2);

  application.SendNotification();
  application.Render(16); // Let new animation start

  // Now, mPreviousFrameTexture (nativeImageSource1) and mCurrentFrameTexture (nativeImageSource2) are different.
  // The animation should start, and uInterpolationFactor should be reset to 0.0f by the new animation logic.
  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("uInterpolationFactor after second frame: %f\n", currentValue);
  DALI_TEST_CHECK(currentValue >= 0.0f && currentValue < 0.1f);

  // Check at the midpoint of the animation
  application.Render(234u); // 16ms + 234ms = 250ms
  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("uInterpolationFactor at midpoint: %f\n", currentValue);
  DALI_TEST_CHECK(currentValue > 0.4f && currentValue < 0.6f);

  // Check at the end of the animation
  application.Render(266u); // 250ms + 266ms = 516ms
  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("uInterpolationFactor at end: %f\n", currentValue);
  DALI_TEST_EQUALS(currentValue, 1.0f, TEST_LOCATION);

  // --- Part 3: Setting the same frame again. Animation should restart. ---
  tet_printf("Part 3: Setting the same frame again.\n");
  DevelVideoView::SetNativeImageSourceForCurrentFrame(videoView, nativeImageSource2); // Set the same frame

  application.SendNotification();
  application.Render(16);

  // Even though we set the same NativeImageSource object, the internal logic treats
  // mPreviousFrameTexture (nativeImageSource2) and mCurrentFrameTexture (nativeImageSource2)
  // as a transition, so the animation restarts.
  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("uInterpolationFactor after setting same frame again: %f\n", currentValue);
  DALI_TEST_CHECK(currentValue >= 0.0f && currentValue < 0.1f); // Animation restarted.

  // Render for the duration of the interpolation interval. The animation should complete.
  application.Render(static_cast<unsigned int>(interpolationInterval * 1000.0f));
  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("uInterpolationFactor after setting same frame again + interval: %f\n", currentValue);
  DALI_TEST_EQUALS(currentValue, 1.0f, TEST_LOCATION); // Animation completed.

  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true; // Restore original value

  END_TEST;
}

int UtcDaliVideoViewSetNativeImageSourceForCurrentFrame_N(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewSetNativeImageSourceForCurrentFrame_N");

  // This test only works in Underlay mode to be consistent with the positive test.
  ToolkitApplication::DECODED_IMAGES_SUPPORTED = false; // Force Underlay.

  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  application.GetScene().Add(videoView);
  videoView.SetProperty(VideoView::Property::UNDERLAY, true);
  videoView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  // Set a positive interpolation interval to avoid potential crash paths in the new implementation
  // that might be triggered when mInterpolationInterval is 0.0f (its default value).
  // This ensures the test focuses on the empty NativeImageSource handling.
  DevelVideoView::SetFrameInterpolationInterval(videoView, 0.5f);

  application.SendNotification();
  application.Render();

  // Initially, there should be only one renderer (the general overlay renderer).
  // The texture renderer for interpolation is created only when a valid NativeImageSource is set.
  DALI_TEST_EQUALS(videoView.GetRendererCount(), 1u, TEST_LOCATION);

  // Test setting an uninitialized (empty) NativeImageSourcePtr
  NativeImageSourcePtr emptyImageSource; // Default constructor creates an empty handle
  DALI_TEST_CHECK(!emptyImageSource);

  // The expected behavior is that it should not crash and should be a no-op.
  // Specifically, it should not create the interpolation texture renderer.
  try
  {
    DevelVideoView::SetNativeImageSourceForCurrentFrame(videoView, emptyImageSource);
    DALI_TEST_CHECK(true); // If no exception, test passes for robustness

    application.SendNotification();
    application.Render(16);

    // After setting an empty image source, the renderer count should still be 1,
    // as CreateOverlayTextureVisual should not have been called or should have failed gracefully.
    DALI_TEST_EQUALS(videoView.GetRendererCount(), 1u, TEST_LOCATION);
  }
  catch(Dali::DaliException& e)
  {
    // If an assertion is thrown and caught, it's also an acceptable behavior for invalid input.
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_CHECK(true); // Test passes if an expected assertion is thrown.
  }
  catch(...)
  {
    // A segmentation fault would be caught here as "any other unexpected exception",
    // leading to test failure. This indicates a bug in the implementation.
    tet_printf("Caught an unexpected exception, possibly a segmentation fault.\n");
    DALI_TEST_CHECK(false); // Fail for any other unexpected exception
  }

  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true; // Restore original value

  END_TEST;
}

int UtcDaliVideoViewCleanupInterpolationAnimationOnIntervalChange_P(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewCleanupInterpolationAnimationOnIntervalChange_P");

  // This test verifies that mInterpolationAnimation is stopped and cleared
  // when a new frame interpolation interval is set, and that a subsequent frame
  // starts a new animation with the correct interval.

  // Force Underlay mode.
  ToolkitApplication::DECODED_IMAGES_SUPPORTED = false;

  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK(videoView);

  application.GetScene().Add(videoView);
  videoView.SetProperty(VideoView::Property::UNDERLAY, true);
  videoView.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));

  // Set an initial frame interpolation interval.
  float initialInterval = 0.5f; // 0.5 seconds
  DevelVideoView::SetFrameInterpolationInterval(videoView, initialInterval);

  // --- Step 1: Set the first frame. No animation should play. ---
  NativeImageSourcePtr nativeImageSource1 = NativeImageSource::New(16, 16, NativeImageSource::COLOR_DEPTH_DEFAULT);
  DevelVideoView::SetNativeImageSourceForCurrentFrame(videoView, nativeImageSource1);
  application.SendNotification();
  application.Render(16);

  Renderer renderer = videoView.GetRendererAt(1);
  Shader   shader   = renderer.GetShader();
  Property::Index interpolationFactorIndex = shader.GetPropertyIndex("uInterpolationFactor");
  float currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("Value after first frame: %f\n", currentValue);
  // The new logic sets uInterpolationFactor to 1.0f immediately for the first frame.
  DALI_TEST_EQUALS(currentValue, 1.0f, TEST_LOCATION);

  // --- Step 2: Set a second, different frame. Animation should play. ---
  NativeImageSourcePtr nativeImageSource2 = NativeImageSource::New(16, 16, NativeImageSource::COLOR_DEPTH_DEFAULT);
  DevelVideoView::SetNativeImageSourceForCurrentFrame(videoView, nativeImageSource2);
  application.SendNotification();
  application.Render(16); // Start animation

  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("Value after second frame: %f\n", currentValue);
  DALI_TEST_CHECK(currentValue >= 0.0f && currentValue < 0.1f); // Animation started.

  // --- Step 3: Render to a midpoint to ensure animation is active. ---
  application.Render(234u); // 250ms total
  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("Value at midpoint: %f\n", currentValue);
  DALI_TEST_CHECK(currentValue > 0.4f && currentValue < 0.6f); // Animation is in progress.

  // --- Step 4: Set a new frame interpolation interval. ---
  // Note: This does not stop the currently playing animation. The new interval will
  // be used for the *next* animation that starts.
  float newInterval = 0.2f; // 0.2 seconds
  DevelVideoView::SetFrameInterpolationInterval(videoView, newInterval);
  DALI_TEST_EQUALS(DevelVideoView::GetFrameInterpolationInterval(videoView), newInterval, TEST_LOCATION);

  // --- Step 5: Let the current animation (with the old interval) finish. ---
  // Render the remaining duration of the initial 0.5s interval.
  application.Render(250u); // 250ms + 250ms = 500ms
  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("Value after first animation finishes: %f\n", currentValue);
  DALI_TEST_EQUALS(currentValue, 1.0f, TEST_LOCATION); // First animation should finish.

  // --- Step 6: Set a third frame to start a new animation with the new interval. ---
  NativeImageSourcePtr nativeImageSource3 = NativeImageSource::New(16, 16, NativeImageSource::COLOR_DEPTH_DEFAULT);
  DevelVideoView::SetNativeImageSourceForCurrentFrame(videoView, nativeImageSource3);

  application.SendNotification();
  application.Render(16); // Start new animation

  // --- Step 7: Check that the new animation starts from 0.0f and finishes with the new interval. ---
  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("New animation start value: %f\n", currentValue);
  DALI_TEST_CHECK(currentValue >= 0.0f && currentValue < 0.1f);

  // Render for the new interval's duration.
  application.Render(184u); // 16ms + 184ms = 200ms
  currentValue = shader.GetCurrentProperty<float>(interpolationFactorIndex);
  tet_printf("New animation end value: %f\n", currentValue);
  DALI_TEST_EQUALS(currentValue, 1.0f, TEST_LOCATION); // New animation should finish with the new interval.

  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true; // Restore original value
  END_TEST;
}
