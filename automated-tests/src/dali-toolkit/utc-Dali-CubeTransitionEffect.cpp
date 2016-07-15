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
#include <dali/devel-api/adaptor-framework/bitmap-loader.h>
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
  TransitionCompletedCallback( bool& signalReceived, CubeTransitionEffect& effect, Texture& image )
  : mSignalVerified( signalReceived ),
    mCurrentEffect( effect ),
    mActorTransitTo( image )
  {
  }

  void Callback( CubeTransitionEffect effect, Texture image )
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
  Texture&               mActorTransitTo;
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

  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 40, 40 );

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  Stage::GetCurrent().Add( waveEffect );
  waveEffect.SetCurrentTexture( texture );
  waveEffect.SetTargetTexture( texture );

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
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 40, 40 );

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  Stage::GetCurrent().Add( waveEffect );

  waveEffect.SetTransitionDuration( TRANSITION_DURATION );
  waveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  DALI_TEST_CHECK( !waveEffect.IsTransitioning() );

  waveEffect.SetCurrentTexture( texture );
  waveEffect.SetTargetTexture( texture );
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

  crossEffect.SetCurrentTexture( texture );
  crossEffect.SetTargetTexture( texture );
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

  foldEffect.SetCurrentTexture( texture );
  foldEffect.SetTargetTexture( texture );
  //transition is started
  foldEffect.StartTransition(true);
  DALI_TEST_CHECK(foldEffect.IsTransitioning() );
  //transition is finished
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK( !foldEffect.IsTransitioning() );

  END_TEST;
}

//Test common codes in base class
int UtcDaliCubeTransitionEffectSetCurrentTexture(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionEffectSetCurrentTexture ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 40, 40 );

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  waveEffect.SetCurrentTexture( texture );

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
int UtcDaliCubeTransitionEffectSetTargetTexture(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliCubeTransitionEffectSetTargetTexture ");

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 40, 40 );
  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  Stage::GetCurrent().Add( waveEffect );

  waveEffect.SetCurrentTexture( texture );
  waveEffect.SetTargetTexture( texture );

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

  BitmapLoader loader = BitmapLoader::New( "Image.jpg" );
  loader.Load();
  PixelData pixelData = loader.GetPixelData();
  Texture texture = Texture::New( TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight() );
  texture.Upload( pixelData );

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  waveEffect.SetTransitionDuration( TRANSITION_DURATION );
  waveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  waveEffect.SetCurrentTexture( texture );

  Stage::GetCurrent().Add( waveEffect );

  application.SendNotification();
  application.Render();

  waveEffect.StartTransition( true );

  Actor cube = waveEffect.GetChildAt(0).GetChildAt(0);

  //check the cube rotation value and color values just before the end of different transitions
  waveEffect.SetTargetTexture( texture );
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  waveEffect.SetTargetTexture( texture );
  waveEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  waveEffect.SetTargetTexture( texture );
  waveEffect.StartTransition(false);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  waveEffect.SetTargetTexture( texture );
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

  BitmapLoader loader = BitmapLoader::New( "Image.jpg" );
  loader.Load();
  PixelData pixelData = loader.GetPixelData();
  Texture texture = Texture::New( TextureType::TEXTURE_2D, pixelData.GetPixelFormat(), pixelData.GetWidth(), pixelData.GetHeight() );
  texture.Upload( pixelData );

  CubeTransitionEffect crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  crossEffect.SetSize( VIEW_AREA_SIZE );
  crossEffect.SetTransitionDuration( TRANSITION_DURATION );
  crossEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  crossEffect.SetCurrentTexture( texture );
  crossEffect.SetTargetTexture( texture );

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


  crossEffect.SetTargetTexture( texture );
  crossEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::XAXIS), EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );


  crossEffect.SetTargetTexture( texture );
  crossEffect.StartTransition(false);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::XAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  crossEffect.SetTargetTexture( texture );
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
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 40, 40 );
  CubeTransitionEffect foldEffect = CubeTransitionFoldEffect::New( NUM_ROWS, NUM_COLUMNS );
  foldEffect.SetSize( VIEW_AREA_SIZE );
  foldEffect.SetTransitionDuration( TRANSITION_DURATION );
  foldEffect.SetCurrentTexture( texture );
  foldEffect.SetTargetTexture( texture );

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

  foldEffect.SetTargetTexture( texture );
  foldEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );


  foldEffect.SetTargetTexture( texture );
  foldEffect.StartTransition(false);
  Wait( application, TRANSITION_BEFORE_END_DURATION );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( -Dali::ANGLE_90,  Vector3::YAXIS), EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), HALF_BRIGHTNESS, EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(),FULL_BRIGHTNESS, EPISILON, TEST_LOCATION );

  foldEffect.SetTargetTexture( texture );
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
  Texture firstTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 30, 30 );
  Texture secondTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 20, 20 );
  Texture thirdTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 40, 40 );

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
  Texture actorTransitTo;
  TransitionCompletedCallback callback(signalVerified, currentEffect, actorTransitTo);
  waveEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );
  crossEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );
  foldEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );

  //check that the wave effect is used to transit to secondTexture
  currentEffect = waveEffect;
  actorTransitTo = secondTexture;
  waveEffect.SetCurrentTexture( firstTexture );
  waveEffect.SetTargetTexture( secondTexture );
  waveEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  //check that the wave effect is used to transit to thirdTexture
  actorTransitTo = thirdTexture;
  waveEffect.SetTargetTexture( thirdTexture );
  waveEffect.StartTransition(PAN_POSITION2, PAN_DISPLACEMENT2);
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  //check that the cross effect is used to transit to secondTexture
  currentEffect = crossEffect;
  actorTransitTo = secondTexture;
  crossEffect.SetCurrentTexture( thirdTexture );
  crossEffect.SetTargetTexture( secondTexture );
  crossEffect.StartTransition(true);
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  //check that the cross effect is used to transit to firstTexture
  actorTransitTo = firstTexture;
  crossEffect.SetTargetTexture( firstTexture );
  crossEffect.StartTransition(false);
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK(callback.mSignalVerified);
  callback.Reset();

  //check that the fold effect is used to transit to secondTexture
  currentEffect = foldEffect;
  actorTransitTo = secondTexture;
  foldEffect.SetCurrentTexture( firstTexture );
  foldEffect.SetTargetTexture( secondTexture );
  foldEffect.StartTransition();
  Wait( application, TRANSITION_DURATION );
  DALI_TEST_CHECK( callback.mSignalVerified );
  callback.Reset();

  //check that the fold effect is used to transit to thirdTexture
  actorTransitTo = thirdTexture;
  foldEffect.SetTargetTexture( thirdTexture );
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
  Texture firstTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 30, 30 );
  Texture secondTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 20, 20 );

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
  Texture actorTransitTo;
  TransitionCompletedCallback callback(signalVerified, currentEffect, actorTransitTo);
  waveEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );
  crossEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );
  foldEffect.TransitionCompletedSignal().Connect( &callback, &TransitionCompletedCallback::Callback );

  currentEffect = waveEffect;
  actorTransitTo = secondTexture;
  waveEffect.SetCurrentTexture( firstTexture );
  waveEffect.SetTargetTexture( secondTexture );
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
  actorTransitTo = firstTexture;
  crossEffect.SetCurrentTexture( secondTexture );
  crossEffect.SetTargetTexture( firstTexture );
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
  actorTransitTo = secondTexture;
  foldEffect.SetCurrentTexture( firstTexture );
  foldEffect.SetTargetTexture( secondTexture );
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
  Texture firstTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 30, 30 );
  Texture secondTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 20, 20 );

  CubeTransitionEffect waveEffect = CubeTransitionWaveEffect::New( NUM_ROWS, NUM_COLUMNS );
  waveEffect.SetSize( VIEW_AREA_SIZE );
  waveEffect.SetTransitionDuration( TRANSITION_DURATION );
  waveEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  waveEffect.SetCurrentTexture( firstTexture );
  waveEffect.SetTargetTexture( secondTexture );

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

  waveEffect.SetTargetTexture( firstTexture );
  waveEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_DURATION*0.4f );
  waveEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  waveEffect.SetTargetTexture( secondTexture );
  waveEffect.StartTransition(false);
  Wait( application, TRANSITION_DURATION*0.6f );
  waveEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  waveEffect.SetTargetTexture( firstTexture );
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
  Texture firstTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 30, 30 );
  Texture secondTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 20, 20 );

  CubeTransitionEffect crossEffect = CubeTransitionCrossEffect::New( NUM_ROWS, NUM_COLUMNS );
  crossEffect.SetSize( VIEW_AREA_SIZE );
  crossEffect.SetTransitionDuration( TRANSITION_DURATION );
  crossEffect.SetCubeDisplacement( CUBE_DISPLACEMENT );
  crossEffect.SetCurrentTexture( firstTexture );
  crossEffect.SetTargetTexture( secondTexture );

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

  crossEffect.SetTargetTexture( firstTexture );
  crossEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_DURATION*0.4f );
  crossEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  crossEffect.SetTargetTexture( secondTexture );
  crossEffect.StartTransition(false);
  Wait( application, TRANSITION_DURATION*0.6f );
  crossEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::ZERO), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  crossEffect.SetTargetTexture( firstTexture );
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
  Texture firstTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 30, 30 );
  Texture secondTexture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, 20, 20 );

  CubeTransitionEffect foldEffect = CubeTransitionFoldEffect::New( NUM_ROWS, NUM_COLUMNS );
  foldEffect.SetSize( VIEW_AREA_SIZE );
  foldEffect.SetTransitionDuration( TRANSITION_DURATION );
  foldEffect.SetCurrentTexture( firstTexture );
  foldEffect.SetTargetTexture( secondTexture );

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

  foldEffect.SetTargetTexture( firstTexture );
  foldEffect.StartTransition(PAN_POSITION1, PAN_DISPLACEMENT1);
  Wait( application, TRANSITION_DURATION*0.4f );
  foldEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::YAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::XAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  foldEffect.SetTargetTexture( secondTexture );
  foldEffect.StartTransition(false);
  Wait( application, TRANSITION_DURATION*0.6f );
  foldEffect.StopTransition();
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  DALI_TEST_EQUALS( cube1.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::YAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetCurrentOrientation(), Quaternion( Dali::ANGLE_0,  Vector3::XAXIS), FLT_EPISILON, TEST_LOCATION  );
  DALI_TEST_EQUALS( cube0.GetChildAt(0).GetCurrentColor(), FULL_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );
  DALI_TEST_EQUALS( cube0.GetChildAt(1).GetCurrentColor(), HALF_BRIGHTNESS, FLT_EPISILON, TEST_LOCATION );

  foldEffect.SetTargetTexture( firstTexture );
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
