/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/devel-api/transition-effects/cube-transition-effect.h>
#include <dali-toolkit/devel-api/transition-effects/cube-transition-cross-effect.h>
#include <dali-toolkit/devel-api/transition-effects/cube-transition-fold-effect.h>
#include <dali-toolkit/devel-api/transition-effects/cube-transition-wave-effect.h>
#include <dali/public-api/images/buffer-image.h>



using namespace Dali;
using namespace Dali::Toolkit;


namespace
{
const unsigned int NUM_ROWS = 16;
const unsigned int NUM_COLUMNS = 10;
const Vector2 VIEW_AREA_SIZE( 480.0f, 800.0f );
const float TRANSITION_DURATION = 0.5f;
const float CUBE_DISPLACEMENT = 55.f;
const Vector2 PAN_POSITION1( VIEW_AREA_SIZE.x * 0.75f, VIEW_AREA_SIZE.y * 0.25f );
const Vector2 PAN_DISPLACEMENT1( -5.f, 5.f );
const Vector2 PAN_POSITION2( VIEW_AREA_SIZE.x * 0.25f, VIEW_AREA_SIZE.y * 0.75f );
const Vector2 PAN_DISPLACEMENT2( 5.f, 5.f );
const Vector4 FULL_BRIGHTNESS(1.f,1.f,1.f,1.f);
const Vector4 HALF_BRIGHTNESS(0.5f, 0.5f, 0.5f, 1.f);
const int RENDER_FRAME_INTERVAL = 16;
static const float FLT_EPISILON = 0.0001f;
static const float EPISILON = 0.05f;
const float TRANSITION_BEFORE_END_DURATION = TRANSITION_DURATION - 0.05f;

static bool gObjectCreatedCallBackCalled;
static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

/**
 * Simulate time passed by, waiting for certain process to finish
 * @param[in] application Test application instance
 * @param[in] durationToPass Time to pass in milliseconds.
 */
void Wait(ToolkitTestApplication& application, float durationToPass)
{
  int duration = static_cast<int>(durationToPass*1000.f);
  // wait 2 more frames to compensate the two frames used by the image waiting for the loading succeeded signal
  for(int i = 0; i <=  duration/RENDER_FRAME_INTERVAL+2 ; i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
  }
}



//Callback class to test whether transition completed signal is emitted when the transition animation is finished
class TransitionCompletedCallback : public Dali::ConnectionTracker
{
public:
  TransitionCompletedCallback( bool& signalReceived, CubeTransitionEffect& effect, Image& image )
  : mSignalVerified( signalReceived ),
    mCurrentEffect( effect ),
    mActorTransitTo( image )
  {
  }

  void Callback( CubeTransitionEffect effect, Image image )
  {
    tet_infoline( "Verifying TransitionCompletedSignal" );

    if( mCurrentEffect == effect && mActorTransitTo == image )
    {
      mSignalVerified = true;
    }
  }

  void Reset()
  {
    mSignalVerified = false;
  }

  bool&                  mSignalVerified;
  CubeTransitionEffect&  mCurrentEffect;
  Image&                 mActorTransitTo;
};

} // namespace



void cube_transition_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void cube_transition_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliCubeTransitionWaveEffectNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionWaveEffectNew ");

  CubeTransitionEffect waveEffect;

  DALI_TEST_CHECK( !waveEffect );

  waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );

  DALI_TEST_CHECK( waveEffect );

  waveEffect.Reset();

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
    waveEffect.SetSize( VIEW_AREA_SIZE );
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliCubeTransitionCrossEffectNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionCrossEffectNew ");

  CubeTransitionEffect crossEffect;

  DALI_TEST_CHECK( !crossEffect );

  crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  crossEffect.SetSize( VIEW_AREA_SIZE );

  DALI_TEST_CHECK( crossEffect );

  crossEffect.Reset();

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    CubeTransitionEffect crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
    crossEffect.SetSize( VIEW_AREA_SIZE );
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliCubeTransitionFoldEffectNew(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliCubeTransitionFoldEffectNew " );

  CubeTransitionEffect foldEffect;

  DALI_TEST_CHECK( !foldEffect );

  foldEffect = CubeTransitionFoldEffect::New( NUM_ROWS, NUM_COLUMNS );
  foldEffect.SetSize( VIEW_AREA_SIZE );

  DALI_TEST_CHECK( foldEffect );

  foldEffect.Reset();

  //Additional check to ensure object is created by checking if it is registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    CubeTransitionEffect foldEffect = CubeTransitionFoldEffect::New( NUM_ROWS, NUM_COLUMNS );
    foldEffect.SetSize( VIEW_AREA_SIZE );
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliCubeTransitionEffectSetGetTransitionDuration(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionEffectSetGetTransitionDuration ");

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetTransitionDuration( TRANSITION_DURATION );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  DALI_TEST_EQUALS( TRANSITION_DURATION, waveEffect.GetTransitionDuration(), TEST_LOCATION );

  CubeTransitionEffect crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  crossEffect.SetTransitionDuration( TRANSITION_DURATION );
  crossEffect.SetSize( VIEW_AREA_SIZE );
  DALI_TEST_EQUALS( TRANSITION_DURATION, crossEffect.GetTransitionDuration(), TEST_LOCATION );

  CubeTransitionEffect foldEffect = CubeTransitionFoldEffect::New( NUM_ROWS, NUM_COLUMNS );
  foldEffect.SetSize( VIEW_AREA_SIZE );
  foldEffect.SetTransitionDuration( TRANSITION_DURATION );
  DALI_TEST_EQUALS( TRANSITION_DURATION, foldEffect.GetTransitionDuration(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliCubeTransitionEffectSetGetCubeDisplacement(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionEffectSetGetTransitionDuration ");

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS);
  waveEffect.SetSize( VIEW_AREA_SIZE );
  waveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  DALI_TEST_EQUALS( CUBE_DISPLACEMENT, waveEffect.GetCubeDisplacement(), TEST_LOCATION );

  CubeTransitionEffect crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  crossEffect.SetSize( VIEW_AREA_SIZE );
  crossEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  DALI_TEST_EQUALS( CUBE_DISPLACEMENT, crossEffect.GetCubeDisplacement(), TEST_LOCATION );

  //Cube displacement is not used in CubeTransitionFoldEffect
  END_TEST;
}

//Test common codes in base class
int UtcDaliCubeTransitionEffectGetRoot(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionEffectGetRoot ");

  unsigned int totalNum = NUM_ROWS*NUM_COLUMNS;

  Image image = BufferImage::New( 40, 40 ) ;

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  Stage::GetCurrent().Add( waveEffect );
  waveEffect.SetCurrentImage( image );
  waveEffect.SetTargetImage( image );

  application.SendNotification();
  application.Render();

  waveEffect.StartTransition();

  Wait( application, TRANSITION_DURATION * 0.5f );

  // check that we have a total of NUM_ROWS*NUM_COLUMNS cubes;
  Actor boxesRoot = waveEffect.GetChildAt(0);
  DALI_TEST_CHECK( totalNum == boxesRoot.GetChildCount() );

  // check that every cube has two children
  DALI_TEST_CHECK( 2 == boxesRoot.GetChildAt(0).GetChildCount() );
  DALI_TEST_CHECK( 2 == boxesRoot.GetChildAt(totalNum/2).GetChildCount() );
  DALI_TEST_CHECK( 2 == boxesRoot.GetChildAt(totalNum-1).GetChildCount() );
  END_TEST;
}

int UtcDaliCubeTransitionEffectIsTransitioning(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionEffectIsTransiting ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image image = BufferImage::New( 40, 40 ) ;

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  Stage::GetCurrent().Add( waveEffect );

  waveEffect.SetTransitionDuration( TRANSITION_DURATION );
  waveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  DALI_TEST_CHECK( !waveEffect.IsTransitioning() );

  waveEffect.SetCurrentImage( image );
  waveEffect.SetTargetImage( image );
  //transition is started
  waveEffect.StartTransition();
  DALI_TEST_CHECK( waveEffect.IsTransitioning() );
  //transition is finished
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK( !waveEffect.IsTransitioning() );

  CubeTransitionEffect crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  crossEffect.SetSize( VIEW_AREA_SIZE );
  Stage::GetCurrent().Add( crossEffect );

  crossEffect.SetTransitionDuration( TRANSITION_DURATION );
  crossEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  DALI_TEST_CHECK( !crossEffect.IsTransitioning() );

  crossEffect.SetCurrentImage( image );
  crossEffect.SetTargetImage( image );
  //transition is started
  crossEffect.StartTransition(false);
  DALI_TEST_CHECK( crossEffect.IsTransitioning() );
  //transition is finished
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK( !crossEffect.IsTransitioning() );

  CubeTransitionEffect foldEffect = CubeTransitionFoldEffect::New( NUM_ROWS, NUM_COLUMNS );
  foldEffect.SetSize( VIEW_AREA_SIZE );
  Stage::GetCurrent().Add( foldEffect );

  foldEffect.SetTransitionDuration( TRANSITION_DURATION );
  DALI_TEST_CHECK( !foldEffect.IsTransitioning() );

  foldEffect.SetCurrentImage( image );
  foldEffect.SetTargetImage( image );
  //transition is started
  foldEffect.StartTransition(true);
  DALI_TEST_CHECK(foldEffect.IsTransitioning() );
  //transition is finished
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK( !foldEffect.IsTransitioning() );

  END_TEST;
}

//Test common codes in base class
int UtcDaliCubeTransitionEffectSetCurrentImage(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionEffectSetCurrentImage ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image image = BufferImage::New( 40, 40 ) ;

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  waveEffect.SetCurrentImage( image );

  Stage::GetCurrent().Add( waveEffect );

  application.SendNotification();
  application.Render();

  waveEffect.StartTransition();

  // the current image content is set to the tiles facing the camera
  Actor currentTile = waveEffect.GetChildAt(0).GetChildAt(0).GetChildAt(0);
  Actor targetTile = waveEffect.GetChildAt(0).GetChildAt(0).GetChildAt(1);

  //check the pixel area set to the cube
  Vector4 pixelAreaDef( 0.f, 0.f, 1.f / NUM_COLUMNS, 1.f / NUM_ROWS);

  Property::Index textureRectIndex = currentTile.GetPropertyIndex( "uTextureRect" );
  DALI_TEST_CHECK( textureRectIndex != Property::INVALID_INDEX );
  Property::Value textureRectValue = currentTile.GetProperty( textureRectIndex );
  DALI_TEST_CHECK( textureRectValue.GetType() == Property::VECTOR4 );
  Vector4 pixelArea;
  DALI_TEST_CHECK( textureRectValue.Get( pixelArea ) );

  DALI_TEST_EQUALS( pixelAreaDef, pixelArea, FLT_EPISILON, TEST_LOCATION );

  END_TEST;
}

//Test common codes in base class
int UtcDaliCubeTransitionEffectSetTargetImage(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionEffectSetTargetImage ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image image = BufferImage::New( 30, 30 );

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  Stage::GetCurrent().Add( waveEffect );

  waveEffect.SetCurrentImage( image );
  waveEffect.SetTargetImage( image );

  Stage::GetCurrent().Add( waveEffect );

  application.SendNotification();
  application.Render();

  waveEffect.StartTransition();

  // the target image content is set to the tiles currently invisible to the camera
  Actor tile = waveEffect.GetChildAt(0).GetChildAt(0).GetChildAt(1);

  //check the pixel area set to the cube
  Vector4 pixelAreaDef( 0.f, 0.f, 1.f / NUM_COLUMNS, 1.f / NUM_ROWS);

  Property::Index textureRectIndex = tile.GetPropertyIndex( "uTextureRect" );
  DALI_TEST_CHECK( textureRectIndex != -1 );
  Property::Value textureRectValue = tile.GetProperty( textureRectIndex );
  DALI_TEST_CHECK( textureRectValue.GetType() == Property::VECTOR4 );
  Vector4 pixelArea;
  DALI_TEST_CHECK( textureRectValue.Get( pixelArea ) );

  DALI_TEST_EQUALS( pixelAreaDef, pixelArea, FLT_EPISILON, TEST_LOCATION );

  END_TEST;
}

int UtcDaliCubeTransitionWaveEffectStartTransition(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionWaveEffectStartTransition ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image image = ResourceImage::New( "Image.jpg" );

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  waveEffect.SetTransitionDuration( TRANSITION_DURATION );
  waveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  waveEffect.SetCurrentImage( image );

  Stage::GetCurrent().Add( waveEffect );

  application.SendNotification();
  application.Render();

  waveEffect.StartTransition( true );

  Actor cube = waveEffect.GetChildAt(0).GetChildAt(0);

  //check the cube rotation value and color values just before the end of different transitions
  waveEffect.SetTargetImage( image );
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  waveEffect.SetTargetImage( image );
  waveEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  waveEffect.SetTargetImage( image );
  waveEffect.StartTransition(false);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  waveEffect.SetTargetImage( image );
  waveEffect.StartTransition(PAN_POSITION2, PAN_DISPLACEMENT2);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCubeTransitionCrossEffectStartTransition(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionCrossEffectStartTransition ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image image = ResourceImage::New( "Image.jpg" );

  CubeTransitionEffect crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  crossEffect.SetSize( VIEW_AREA_SIZE );
  crossEffect.SetTransitionDuration( TRANSITION_DURATION );
  crossEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  crossEffect.SetCurrentImage( image );
  crossEffect.SetTargetImage( image );

  Stage::GetCurrent().Add( crossEffect );

  application.SendNotification();
  application.Render();

  crossEffect.StartTransition(true);

  Actor cube0 = crossEffect.GetChildAt(0).GetChildAt(0);
  Actor cube1 = crossEffect.GetChildAt(0).GetChildAt(1);

  //check the cube rotation value and color values just before the end of different transitions
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::XAXIS), EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );


  crossEffect.SetTargetImage( image );
  crossEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::XAXIS), EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );


  crossEffect.SetTargetImage( image );
  crossEffect.StartTransition(false);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::XAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  crossEffect.SetTargetImage( image );
  crossEffect.StartTransition(PAN_POSITION2, PAN_DISPLACEMENT2);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::XAXIS), EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCubeTransitionFoldEffectStartTransition(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionFoldEffectStartTransition ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image image = BufferImage::New( 30, 30 );

  CubeTransitionEffect foldEffect = CubeTransitionFoldEffect::New( NUM_ROWS, NUM_COLUMNS );
  foldEffect.SetSize( VIEW_AREA_SIZE );
  foldEffect.SetTransitionDuration( TRANSITION_DURATION );
  foldEffect.SetCurrentImage( image );
  foldEffect.SetTargetImage( image );

  Stage::GetCurrent().Add( foldEffect );

  application.SendNotification();
  application.Render();

  foldEffect.StartTransition(true);

  Actor cube0 = foldEffect.GetChildAt(0).GetChildAt(0);
  Actor cube1 = foldEffect.GetChildAt(0).GetChildAt(1);

  //check the cube rotation value and color values just before the end of different transitions
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  foldEffect.SetTargetImage( image );
  foldEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );


  foldEffect.SetTargetImage( image );
  foldEffect.StartTransition(false);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(),FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  foldEffect.SetTargetImage( image );
  foldEffect.StartTransition(PAN_POSITION2, PAN_DISPLACEMENT2);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCubeTransitionEffectSignalTransitionCompleted(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionEffectSignalTransitionCompleted ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image firstImage = BufferImage::New( 30, 30 );
  Image secondImage = BufferImage::New( 20, 20 );
  Image thirdImage = BufferImage::New( 40, 40 );

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  waveEffect.SetTransitionDuration( TRANSITION_DURATION );
  waveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  Stage::GetCurrent().Add( waveEffect );

  CubeTransitionEffect crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  crossEffect.SetSize( VIEW_AREA_SIZE );
  crossEffect.SetTransitionDuration( TRANSITION_DURATION );
  crossEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  Stage::GetCurrent().Add( crossEffect );

  CubeTransitionEffect foldEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  foldEffect.SetSize( VIEW_AREA_SIZE );
  foldEffect.SetTransitionDuration( TRANSITION_DURATION );
  Stage::GetCurrent().Add( foldEffect );

  bool signalVerified = false;
  CubeTransitionEffect currentEffect;
  Image actorTransitTo;
  TransitionCompletedCallback callback(signalVerified, currentEffect, actorTransitTo);
  waveEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );
  crossEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );
  foldEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );

  //check that the wave effect is used to transit to secondImage
  currentEffect = waveEffect;
  actorTransitTo = secondImage;
  waveEffect.SetCurrentImage( firstImage );
  waveEffect.SetTargetImage( secondImage );
  waveEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  //check that the wave effect is used to transit to thirdImage
  actorTransitTo = thirdImage;
  waveEffect.SetTargetImage( thirdImage );
  waveEffect.StartTransition(PAN_POSITION2, PAN_DISPLACEMENT2);
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  //check that the cross effect is used to transit to secondImage
  currentEffect = crossEffect;
  actorTransitTo = secondImage;
  crossEffect.SetCurrentImage( thirdImage );
  crossEffect.SetTargetImage( secondImage );
  crossEffect.StartTransition(true);
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  //check that the cross effect is used to transit to firstImage
  actorTransitTo = firstImage;
  crossEffect.SetTargetImage( firstImage );
  crossEffect.StartTransition(false);
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  //check that the fold effect is used to transit to secondImage
  currentEffect = foldEffect;
  actorTransitTo = secondImage;
  foldEffect.SetCurrentImage( firstImage );
  foldEffect.SetTargetImage( secondImage );
  foldEffect.StartTransition();
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK( callback.mSignalVerified );
  callback.Reset();

  //check that the fold effect is used to transit to thirdImage
  actorTransitTo = thirdImage;
  foldEffect.SetTargetImage( thirdImage );
  foldEffect.StartTransition( false );
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK( callback.mSignalVerified );
  END_TEST;
}

int UtcDaliCubeTransitionEffectPauseResumeTransition(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionEffectPauseResumeTransition ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image firstImage = BufferImage::New( 30, 30 );
  Image secondImage = BufferImage::New( 20, 20 );

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  waveEffect.SetTransitionDuration( TRANSITION_DURATION );
  waveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  Stage::GetCurrent().Add( waveEffect );

  CubeTransitionEffect crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  crossEffect.SetSize( VIEW_AREA_SIZE );
  crossEffect.SetTransitionDuration( TRANSITION_DURATION );
  crossEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  Stage::GetCurrent().Add( crossEffect );

  CubeTransitionEffect foldEffect = CubeTransitionFoldEffect::New( NUM_ROWS, NUM_COLUMNS );
  foldEffect.SetSize( VIEW_AREA_SIZE );
  foldEffect.SetTransitionDuration( TRANSITION_DURATION );
  Stage::GetCurrent().Add( foldEffect );

  bool signalVerified = false;
  CubeTransitionEffect currentEffect;
  Image actorTransitTo;
  TransitionCompletedCallback callback(signalVerified, currentEffect, actorTransitTo);
  waveEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );
  crossEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );
  foldEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );

  currentEffect = waveEffect;
  actorTransitTo = secondImage;
  waveEffect.SetCurrentImage( firstImage );
  waveEffect.SetTargetImage( secondImage );
  // start transition; transit for 0.5*duration; pause for 0.5*duration;
  // resume for 0.25*duration; pause for 0.25*duration; resume for another 0.25*duration;
  // only until now the transition finished signal can be received
  waveEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_DURATION*0.5f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  waveEffect.PauseTransition();
  Wait( application, TRANSITION_DURATION*0.5f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  waveEffect.ResumeTransition();
  Wait( application, TRANSITION_DURATION*0.25f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  waveEffect.PauseTransition();
  Wait( application, TRANSITION_DURATION*0.25f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  waveEffect.ResumeTransition();
  Wait( application, TRANSITION_DURATION*0.25f );
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  currentEffect = crossEffect;
  actorTransitTo = firstImage;
  crossEffect.SetCurrentImage( secondImage );
  crossEffect.SetTargetImage( firstImage );
  // start transition; transit for 0.25*duration; pause for 0.2*duration;
  // resume for 0.5*duration; pause for 0.2*duration; resume for another 0.25*duration;
  // only until now the transition finished signal can be received
  crossEffect.StartTransition(false);
  Wait( application, TRANSITION_DURATION*0.25f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  crossEffect.PauseTransition();
  Wait( application, TRANSITION_DURATION*0.2f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  crossEffect.ResumeTransition();
  Wait( application, TRANSITION_DURATION*0.5f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  crossEffect.PauseTransition();
  Wait( application, TRANSITION_DURATION*0.2f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  crossEffect.ResumeTransition();
  Wait( application, TRANSITION_DURATION*0.25f );
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  currentEffect = foldEffect;
  actorTransitTo = secondImage;
  foldEffect.SetCurrentImage( firstImage );
  foldEffect.SetTargetImage( secondImage );
  // start transition; transit for 0.5*duration; pause for 0.5*duration;
  // resume for 0.25*duration; pause for 0.25*duration; resume for another 0.25*duration;
  // only until now the transition finished signal can be received
  foldEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_DURATION*0.5f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  foldEffect.PauseTransition();
  Wait( application, TRANSITION_DURATION*0.5f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  foldEffect.ResumeTransition();
  Wait( application, TRANSITION_DURATION*0.25f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  foldEffect.PauseTransition();
  Wait( application, TRANSITION_DURATION*0.25f );
  DALI_TEST_CHECK(!callback.mSignalVerified);
  foldEffect.ResumeTransition();
  Wait( application, TRANSITION_DURATION*0.25f );
  DALI_TEST_CHECK(callback.mSignalVerified);
  END_TEST;
}

int UtcDaliCubeTransitionWaveEffectStopTransition(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionWaveEffectStopTransition ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image firstImage = BufferImage::New( 30, 30 );
  Image secondImage = BufferImage::New( 20, 20 );

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  waveEffect.SetTransitionDuration( TRANSITION_DURATION );
  waveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  waveEffect.SetCurrentImage( firstImage );
  waveEffect.SetTargetImage( secondImage );

  Stage::GetCurrent().Add( waveEffect );

  application.SendNotification();
  application.Render();

  waveEffect.StartTransition(true);

  Actor cube = waveEffect.GetChildAt(0).GetChildAt(0);

  //check the cube rotation value and color values reset after stopping different transitions in the middle
  Wait( application, TRANSITION_DURATION*0.2f );
  waveEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  waveEffect.SetTargetImage( firstImage );
  waveEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_DURATION*0.4f );
  waveEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  waveEffect.SetTargetImage( secondImage );
  waveEffect.StartTransition(false);
  Wait( application, TRANSITION_DURATION*0.6f );
  waveEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  waveEffect.SetTargetImage( firstImage );
  waveEffect.StartTransition(PAN_POSITION2, PAN_DISPLACEMENT2);
  Wait( application, TRANSITION_DURATION*0.8f );
  waveEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCubeTransitionCrossEffectStopTransition(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionCrossEffectStopTransition ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image firstImage = BufferImage::New( 30, 30 );
  Image secondImage = BufferImage::New( 20, 20 );

  CubeTransitionEffect crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  crossEffect.SetSize( VIEW_AREA_SIZE );
  crossEffect.SetTransitionDuration( TRANSITION_DURATION );
  crossEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  crossEffect.SetCurrentImage( firstImage );
  crossEffect.SetTargetImage( secondImage );

  Stage::GetCurrent().Add( crossEffect );

  application.SendNotification();
  application.Render();

  crossEffect.StartTransition(true);

  Actor cube0 = crossEffect.GetChildAt(0).GetChildAt(0);
  Actor cube1 = crossEffect.GetChildAt(0).GetChildAt(1);

  //check the cube rotation values and color values reset after stop the different transitions in the middle
  Wait( application, TRANSITION_DURATION*0.2f );
  crossEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  crossEffect.SetTargetImage( firstImage );
  crossEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_DURATION*0.4f );
  crossEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  crossEffect.SetTargetImage( secondImage );
  crossEffect.StartTransition(false);
  Wait( application, TRANSITION_DURATION*0.6f );
  crossEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  crossEffect.SetTargetImage( firstImage );
  crossEffect.StartTransition(PAN_POSITION2, PAN_DISPLACEMENT2);
  Wait( application, TRANSITION_DURATION*0.8f );
  crossEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::YAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::XAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  END_TEST;
}

int UtcDaliCubeTransitionFoldEffectStopTransition(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionFoldEffectStopTransition ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Image firstImage = BufferImage::New( 30, 30 );
  Image secondImage = BufferImage::New( 20, 20 );

  CubeTransitionEffect foldEffect = CubeTransitionFoldEffect::New( NUM_ROWS, NUM_COLUMNS );
  foldEffect.SetSize( VIEW_AREA_SIZE );
  foldEffect.SetTransitionDuration( TRANSITION_DURATION );
  foldEffect.SetCurrentImage( firstImage );
  foldEffect.SetTargetImage( secondImage );

  Stage::GetCurrent().Add( foldEffect );

  application.SendNotification();
  application.Render();

  foldEffect.StartTransition(true);

  Actor cube0 = foldEffect.GetChildAt(0).GetChildAt(0);
  Actor cube1 = foldEffect.GetChildAt(0).GetChildAt(1);

  //check the cube rotation values and color values after stop the different transitions in the middle
  Wait( application, TRANSITION_DURATION*0.2f );
  foldEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);

  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::YAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::XAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  foldEffect.SetTargetImage( firstImage );
  foldEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_DURATION*0.4f );
  foldEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::YAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::XAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  foldEffect.SetTargetImage( secondImage );
  foldEffect.StartTransition(false);
  Wait( application, TRANSITION_DURATION*0.6f );
  foldEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::YAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::XAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  foldEffect.SetTargetImage( firstImage );
  foldEffect.StartTransition(PAN_POSITION2, PAN_DISPLACEMENT2);
  Wait( application, TRANSITION_DURATION*0.8f );
  foldEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::YAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::YAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  END_TEST;
}
