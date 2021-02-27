/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/public-api/controls/video-view/video-view.h>
#include <dali-toolkit/devel-api/controls/video-view/video-view-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/adaptor-framework/video-sync-mode.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* const TEST_FILE( "test.mp4" );
const char* const VOLUME_LEFT( "volumeLeft" );
const char* const VOLUME_RIGHT( "volumeRight" );
const char* const RENDERING_TYPE( "renderingTarget" );
const char* const DUMMY_STRING( "dummy string" );

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
  attribute mediump vec2 aPosition;\n
  varying mediump vec2 vTexCoord;\n
  uniform mediump mat4 uMvpMatrix;\n
  uniform mediump vec3 uSize;\n
  \n
  void main()\n
  {\n
    mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n
    vertexPosition.xyz *= uSize;\n
    vertexPosition = uMvpMatrix * vertexPosition;\n
    \n
    vTexCoord = aPosition + vec2(0.5);\n
    gl_Position = vertexPosition;\n
  }\n
);

const char* fragmentShaderPrefix( "#extension GL_OES_EGL_image_external:require\n" );
const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform samplerExternalOES sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
  }\n
);

}

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
    view.Add( a );
    DALI_TEST_CHECK( false );
  }
  catch (Dali::DaliException& e)
  {
    // Tests that a negative test of an assertion succeeds
    DALI_TEST_PRINT_ASSERT( e );
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
  DALI_TEST_CHECK( view );

  const std::string url( DUMMY_STRING );
  Toolkit::VideoView view2 = Toolkit::VideoView::New( url );
  DALI_TEST_CHECK( view2 );
  END_TEST;
}

// Positive test case for a method
int UtcDaliVideoViewDownCast(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewDownCast");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  BaseHandle handle(view);

  Toolkit::VideoView view2 = Toolkit::VideoView::DownCast( handle );
  DALI_TEST_CHECK( view );
  DALI_TEST_CHECK( view2 );
  DALI_TEST_CHECK( view == view2 );
  END_TEST;
}

// Positive test case for a method
int UtcDaliVideoViewProperty1(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty1");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  std::string file;
  view.SetProperty( VideoView::Property::VIDEO, TEST_FILE );
  Property::Value val = view.GetProperty( VideoView::Property::VIDEO );
  DALI_TEST_CHECK( val.Get( file ) );
  DALI_TEST_CHECK( file == TEST_FILE );
  END_TEST;
}

// Positive test case for a method
int UtcDaliVideoViewProperty1b(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty1");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );
  Integration::Scene stage = application.GetScene();

  std::string file;
  Property::Map map;

  view.SetProperty( VideoView::Property::VIDEO, Property::Map()
                    .Add("rendererType", "IMAGE")
                    .Add("url", "video.mpg") // Note, videoView doesn't use this url
                    .Add("RENDERING_TARGET", "windowSurfaceTarget" )
                    .Add("width", 100)
                    .Add("height", 100) );

  stage.Add( view );

  Property::Value val = view.GetProperty( VideoView::Property::VIDEO );
  Property::Map* resultMap = val.GetMap();

  DALI_TEST_CHECK( resultMap );
  Property::Value* value = resultMap->Find("url");
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<std::string>(), "video.mpg", TEST_LOCATION );

  stage.Remove( view );

  END_TEST;
}

int UtcDaliVideoViewProperty2(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty2");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  bool looping;
  Property::Value val = view.GetProperty( VideoView::Property::LOOPING );
  DALI_TEST_CHECK( val.Get( looping ) );
  DALI_TEST_CHECK( !looping );

  view.SetProperty( VideoView::Property::LOOPING, true );
  val = view.GetProperty( VideoView::Property::LOOPING );
  DALI_TEST_CHECK( val.Get( looping ) );
  DALI_TEST_CHECK( looping );
  END_TEST;
}

int UtcDaliVideoViewProperty3(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty3");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  bool muted;
  Property::Value val = view.GetProperty( VideoView::Property::MUTED );
  DALI_TEST_CHECK( val.Get( muted ) );
  DALI_TEST_CHECK( !muted );

  view.SetProperty( VideoView::Property::MUTED, true );
  val = view.GetProperty( VideoView::Property::MUTED );
  DALI_TEST_CHECK( val.Get( muted ) );
  DALI_TEST_CHECK( muted );

  END_TEST;
}

int UtcDaliVideoViewProperty4(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliVideoViewProperty4");

  Toolkit::VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  float left, right;
  left = right = 0.f;

  Property::Map map;
  map.Insert( VOLUME_LEFT, 0.5f );
  map.Insert( VOLUME_RIGHT, 0.5f );

  view.SetProperty( VideoView::Property::VOLUME, map );
  Property::Value val = view.GetProperty( VideoView::Property::VOLUME );

  Property::Map map2;
  DALI_TEST_CHECK( val.Get( map2 ) );

  Property::Value* volumeLeft = map2.Find( VOLUME_LEFT );
  Property::Value* volumeRight = map2.Find( VOLUME_RIGHT );

  DALI_TEST_CHECK( volumeLeft && volumeLeft->Get( left ) );
  DALI_TEST_CHECK( volumeRight && volumeRight->Get( right ) );
  DALI_TEST_CHECK( left == 0.5f );
  DALI_TEST_CHECK( right == 0.5f );

  END_TEST;
}

int UtcDaliVideoViewCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  VideoView view = Toolkit::VideoView::New();
  DALI_TEST_CHECK( view );

  VideoView copy( view );
  DALI_TEST_CHECK( view == copy );

  VideoView assign;
  DALI_TEST_CHECK( !assign );

  assign = copy;
  DALI_TEST_CHECK( assign == view );

  END_TEST;
}

int UtcDaliVideoViewMoveConstructor(void)
{
  ToolkitTestApplication application;

  VideoView view = Toolkit::VideoView::New();
  DALI_TEST_EQUALS( 1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  view.SetProperty( VideoView::Property::LOOPING, true );
  DALI_TEST_CHECK( view.GetProperty<bool>( VideoView::Property::LOOPING ) );

  VideoView moved = std::move( view );
  DALI_TEST_CHECK( moved );
  DALI_TEST_EQUALS( 1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  DALI_TEST_CHECK( moved.GetProperty<bool>( VideoView::Property::LOOPING ) );
  DALI_TEST_CHECK( !view );

  END_TEST;
}

int UtcDaliVideoViewMoveAssignment(void)
{
  ToolkitTestApplication application;

  VideoView view = Toolkit::VideoView::New();
  DALI_TEST_EQUALS( 1, view.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  view.SetProperty( VideoView::Property::LOOPING, true );
  DALI_TEST_CHECK( view.GetProperty<bool>( VideoView::Property::LOOPING ) );

  VideoView moved;
  moved = std::move( view );
  DALI_TEST_CHECK( moved );
  DALI_TEST_EQUALS( 1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  DALI_TEST_CHECK( moved.GetProperty<bool>( VideoView::Property::LOOPING ) );
  DALI_TEST_CHECK( !view );

  END_TEST;
}

int UtcDaliVideoViewTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK( typeRegistry );

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "VideoView" );
  DALI_TEST_CHECK( typeInfo );

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );

  VideoView view = VideoView::DownCast( handle );
  DALI_TEST_CHECK( view );

  END_TEST;
}

int UtcDaliVideoViewMethodsForCoverage(void)
{
  ToolkitTestApplication application;

  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK( videoView );

  videoView.Play();
  videoView.Pause();
  videoView.Stop();
  videoView.Forward(10);
  videoView.Backward(10);

  Toolkit::DevelVideoView::GetMediaPlayer( videoView );

  VideoView::VideoViewSignalType& signal = videoView.FinishedSignal();
  DALI_TEST_EQUALS( 0, signal.GetConnectionCount(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliVideoViewMethodsForRenderType(void)
{
  ToolkitTestApplication application;
  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK( videoView );

  Property::Map windowSurfaceTarget;
  Property::Map nativeImageTarget;

  windowSurfaceTarget.Insert( RENDERING_TYPE, "windowSurfaceTarget" );
  nativeImageTarget.Insert( RENDERING_TYPE, "nativeImageTarget" );

  Property::Map map;
  Property::Value value;
  videoView.SetProperty( VideoView::Property::VIDEO, windowSurfaceTarget );

  value = videoView.GetProperty( VideoView::Property::VIDEO );
  DALI_TEST_CHECK( value.Get( map ) );

  Property::Value* type = map.Find( RENDERING_TYPE );
  DALI_TEST_CHECK( type );
  DALI_TEST_EQUALS( "windowSurfaceTarget", type->Get<std::string>(), TEST_LOCATION );

  videoView.SetProperty( VideoView::Property::VIDEO, nativeImageTarget );

  value = videoView.GetProperty( VideoView::Property::VIDEO );
  DALI_TEST_CHECK( value.Get( map ) );
  type = map.Find( RENDERING_TYPE );

  DALI_TEST_CHECK( type );
  DALI_TEST_EQUALS( "nativeImageTarget", type->Get<std::string>(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliVideoViewCustomShaderForCoverage(void)
{
  ToolkitTestApplication application;
  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK( videoView );

  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true;

  videoView.SetProperty( Toolkit::VideoView::Property::UNDERLAY, false );
  bool isUnderlay = videoView.GetProperty( Toolkit::VideoView::Property::UNDERLAY ).Get< bool >();
  DALI_TEST_CHECK( !isUnderlay );

  application.GetScene().Add( videoView );
  videoView.SetProperty( VideoView::Property::VIDEO, "testvideo" );

  Property::Map customShader;
  customShader.Insert( "vertexShader", VERTEX_SHADER );
  customShader.Insert( "fragmentShader", FRAGMENT_SHADER );

  Property::Map map;
  map.Insert( "shader", customShader );

  videoView.SetProperty( VideoView::Property::VIDEO, map );

  Property::Map map2;
  Property::Value value = videoView.GetProperty( VideoView::Property::VIDEO );

  DALI_TEST_CHECK( !value.Get( map2 ) );
  END_TEST;
}

int UtcDaliVideoViewMethodsForCoverage2(void)
{
  ToolkitTestApplication application;
  VideoView videoView = VideoView::New();
  DALI_TEST_CHECK( videoView );

  Property::Map windowSurfaceTarget;

  windowSurfaceTarget.Insert( RENDERING_TYPE, "windowSurfaceTarget" );

  application.GetScene().Add( videoView );

  application.SendNotification();
  application.Render();

  Property::Map map;
  Property::Value value;
  videoView.SetProperty( VideoView::Property::VIDEO, windowSurfaceTarget );

  value = videoView.GetProperty( VideoView::Property::VIDEO );
  DALI_TEST_CHECK( value.Get( map ) );

  Property::Value* type = map.Find( RENDERING_TYPE );
  DALI_TEST_CHECK( type );
  DALI_TEST_EQUALS( "windowSurfaceTarget", type->Get<std::string>(), TEST_LOCATION );

  Vector3 vector(100.0f, 100.0f, 0.0f);

  DALI_TEST_CHECK(vector != videoView.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ));
  videoView.SetProperty( Actor::Property::SIZE, vector );

  application.SendNotification();
  application.Render();

  // Check the size in the new frame
  DALI_TEST_CHECK(vector == videoView.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ));

  END_TEST;
}

int UtcDaliVideoViewPropertyUnderlay(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewPropertyUnderlay");
  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true;

  VideoView view = VideoView::New();
  DALI_TEST_CHECK( view );

  application.GetScene().Add( view );
  view.Play();

  application.SendNotification();
  application.Render();

  bool isUnderlay = view.GetProperty( Toolkit::VideoView::Property::UNDERLAY ).Get< bool >();
  DALI_TEST_CHECK( isUnderlay );

  view.Play();
  view.SetProperty( Toolkit::VideoView::Property::UNDERLAY, false );
  isUnderlay = view.GetProperty( Toolkit::VideoView::Property::UNDERLAY ).Get< bool >();
  DALI_TEST_CHECK( !isUnderlay );

  view.Play();
  view.SetProperty( Toolkit::VideoView::Property::UNDERLAY, true );
  isUnderlay = view.GetProperty( Toolkit::VideoView::Property::UNDERLAY ).Get< bool >();
  DALI_TEST_CHECK( isUnderlay );

  // If platform api doesn't provide any API or feature for decoded images of video,
  // UNDERLAY should be true
  ToolkitApplication::DECODED_IMAGES_SUPPORTED = false;

  view.SetProperty( Toolkit::VideoView::Property::UNDERLAY, false );
  isUnderlay = view.GetProperty( Toolkit::VideoView::Property::UNDERLAY ).Get< bool >();
  DALI_TEST_CHECK( isUnderlay );

  // For coverage
  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true;

  view.SetProperty( Toolkit::VideoView::Property::UNDERLAY, true );
  view.SetProperty( Toolkit::VideoView::Property::UNDERLAY, false );
  isUnderlay = view.GetProperty( Toolkit::VideoView::Property::UNDERLAY ).Get< bool >();
  DALI_TEST_CHECK( !isUnderlay );

  view.Stop();

  END_TEST;
}

int UtcDaliVideoViewPropertyPlayPosition(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewPropertyPlayPosition");

  VideoView view = VideoView::New();
  DALI_TEST_CHECK( view );

  application.GetScene().Add( view );
  view.Play();

  application.SendNotification();
  application.Render();

  int playPos = view.GetProperty( Toolkit::VideoView::Property::PLAY_POSITION ).Get< int >();
  DALI_TEST_CHECK( playPos == 0 );

  view.SetProperty( Toolkit::VideoView::Property::PLAY_POSITION, 10 );
  playPos = view.GetProperty( Toolkit::VideoView::Property::PLAY_POSITION ).Get< int >();
  // Actually setting play position will be async
  // Actual platform result may be different.
  DALI_TEST_CHECK( playPos == 10 );

  END_TEST;
}

// For coverage.
int UtcDaliVideoViewNew2(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewNew2");

  VideoView view = VideoView::New( true );
  DALI_TEST_CHECK( view );

  application.GetScene().Add( view );
  view.Play();

  application.SendNotification();
  application.Render();

  VideoView view2 = VideoView::New( "", false );
  DALI_TEST_CHECK( view2 );

  application.GetScene().Add( view2 );
  view2.Play();

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliVideoViewRaiseAboveLowerBelow(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewRaiseAboveLowerBelow");

  VideoView view = VideoView::New( true );
  DALI_TEST_CHECK( view );

  application.GetScene().Add( view );
  view.Play();

  application.SendNotification();
  application.Render();

  VideoView view2 = VideoView::New( "", false );
  DALI_TEST_CHECK( view2 );

  application.GetScene().Add( view2 );
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

  VideoView view = VideoView::New( true );
  DALI_TEST_CHECK( view );

  application.GetScene().Add( view );
  view.Play();

  application.SendNotification();
  application.Render();

  VideoView view2 = VideoView::New( "", false );
  DALI_TEST_CHECK( view2 );

  application.GetScene().Add( view2 );
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
  DALI_TEST_CHECK( view );

  application.GetScene().Add( view );
  view.Play();

  application.SendNotification();
  application.Render();

  view.SetProperty( Toolkit::VideoView::Property::DISPLAY_MODE, Toolkit::VideoView::DisplayMode::DST_ROI );
  int displayMode = view.GetProperty( Toolkit::VideoView::Property::DISPLAY_MODE ).Get< int >();
  DALI_TEST_CHECK( displayMode == Toolkit::VideoView::DisplayMode::DST_ROI );

  END_TEST;
}


int UtcDaliVideoViewCustomShader(void)
{
  ToolkitTestApplication application;
  tet_infoline( "VideoView with custom shader" );

  VideoView view = VideoView::New( false );
  DALI_TEST_CHECK( view );

  ToolkitApplication::DECODED_IMAGES_SUPPORTED = true;

  view.SetProperty( Toolkit::VideoView::Property::UNDERLAY, false );
  bool isUnderlay = view.GetProperty( Toolkit::VideoView::Property::UNDERLAY ).Get< bool >();
  DALI_TEST_CHECK( !isUnderlay );

  application.GetScene().Add( view );
  view.SetProperty( VideoView::Property::VIDEO, "testvideo" );

  /* insert custom shader */
  Property::Map customShader;
  std::string fragmentShaderString;
  fragmentShaderString.reserve( strlen( fragmentShaderPrefix ) + strlen( FRAGMENT_SHADER ) );
  fragmentShaderString.append( fragmentShaderPrefix );
  fragmentShaderString.append( FRAGMENT_SHADER );
  customShader.Insert( "vertexShader", VERTEX_SHADER );
  customShader.Insert( "fragmentShader", fragmentShaderString );

  Property::Map map;
  map.Insert( "shader", customShader );

  view.SetProperty( VideoView::Property::VIDEO, map );

  /* do render for check custom shader */
  application.GetScene().Add( view );
  view.Play();

  application.SendNotification();
  application.Render();

  /* get renderer */
  DALI_TEST_CHECK( view.GetRendererCount() == 1u );
  Renderer renderer = view.GetRendererAt( 0 );
  Shader shader = renderer.GetShader();
  DALI_TEST_CHECK( shader );

  Property::Value value = shader.GetProperty(Shader::Property::PROGRAM);
  Property::Map* shaderMap = value.GetMap();
  DALI_TEST_CHECK( shaderMap );

  Property::Value* fragment = shaderMap->Find( "fragment" ); // fragment key name from shader-impl.cpp
  DALI_TEST_EQUALS( fragmentShaderString, fragment->Get<std::string>(), TEST_LOCATION );

  Property::Value* vertex = shaderMap->Find( "vertex" ); // vertex key name from shader-impl.cpp
  DALI_TEST_EQUALS( VERTEX_SHADER, vertex->Get<std::string>(), TEST_LOCATION );

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
    if (!mSignalReceived)
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
    if (mSignalReceived)
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

  VideoView videoView = DevelVideoView::New( Dali::VideoSyncMode::ENABLED );
  DALI_TEST_CHECK( videoView );

  application.GetScene().Add( videoView );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  // Start the animation
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(videoView, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  DevelVideoView::PlayAnimation( videoView, animation );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) - 1u/*just less than the animation duration*/);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(2u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( targetPosition, videoView.GetCurrentProperty< Vector3 >( Actor::Property::POSITION ), TEST_LOCATION );

  // Restart the animation, with a different duration
  finishCheck.Reset();

  END_TEST;
}

int UtcDaliVideoViewASyncAniamtionForCoverage(void)
{
  ToolkitTestApplication application;

  VideoView videoView = DevelVideoView::New( Dali::VideoSyncMode::DISABLED );
  DALI_TEST_CHECK( videoView );

  application.GetScene().Add( videoView );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);

  // Start the animation
  Vector3 targetPosition(10.0f, 10.0f, 10.0f);
  animation.AnimateTo(Property(videoView, Actor::Property::POSITION), targetPosition, AlphaFunction::LINEAR);
  DevelVideoView::PlayAnimation( videoView, animation );

  bool signalReceived(false);
  AnimationFinishCheck finishCheck(signalReceived);
  animation.FinishedSignal().Connect(&application, finishCheck);

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) - 1u/*just less than the animation duration*/);

  // We didn't expect the animation to finish yet
  application.SendNotification();
  finishCheck.CheckSignalNotReceived();

  application.Render(2u/*just beyond the animation duration*/);

  // We did expect the animation to finish
  application.SendNotification();
  finishCheck.CheckSignalReceived();
  DALI_TEST_EQUALS( targetPosition, videoView.GetCurrentProperty< Vector3 >( Actor::Property::POSITION ), TEST_LOCATION );

  // Restart the animation, with a different duration
  finishCheck.Reset();

  END_TEST;
}

int UtcDaliVideoViewResizeWithSynchronization(void)
{
  ToolkitTestApplication application;
  VideoView videoView = DevelVideoView::New( Dali::VideoSyncMode::ENABLED );
  DALI_TEST_CHECK( videoView );

  application.GetScene().Add( videoView );

  Vector3 vector(50.0f, 200.0f, 0.0f);
  videoView.SetProperty( Actor::Property::SIZE, vector );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(vector == videoView.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ));

  END_TEST;
}

// For coverage.
int UtcDaliVideoViewSynchronizationForWindowRotation(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVideoViewSynchronizationForWindowRotation");

  Window window = Window::New(PositionSize(0,0,100,100) ,"", false);
  DALI_TEST_CHECK( window );

  VideoView view = VideoView::New( true );
  DALI_TEST_CHECK( view );

  window.Add( view );

  view.Play();

  DevelWindow::SetPositionSize(window,PositionSize(0,0,480, 240));

  application.SendNotification();
  application.Render();

  END_TEST;
}
