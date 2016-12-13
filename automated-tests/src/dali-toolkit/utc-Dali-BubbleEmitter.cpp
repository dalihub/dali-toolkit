/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/bubble-effect/bubble-emitter.h>

using namespace Dali;
using namespace Dali::Toolkit;

void utc_dali_toolkit_bubble_emitter_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_bubble_emitter_cleanup(void)
{
  test_return_value = TET_PASS;
}


namespace
{

const int RENDER_FRAME_INTERVAL = 16;

static bool gObjectCreatedCallBackCalled;
static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

/*
 * Simulate time passed by.
 *
 * @note this will always process at least 1 frame (1/60 sec)
 *
 * @param application Test application instance
 * @param duration Time to pass in milliseconds.
 * @return The actual time passed in milliseconds
 */
static int Wait(ToolkitTestApplication& application, int duration = 0)
{
  int time = 0;

  for(int i = 0; i <= ( duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
}

static Texture CreateSolidColorTexture( ToolkitTestApplication& application, const Vector4& color, unsigned int width, unsigned int height )
{
  Texture texture = Texture::New( TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height );
  return texture;
}
}//namespace


int UtcDaliBubbleEmitterNew(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBubbleEmitterNew ");

  // Test default constructor
  BubbleEmitter emitter;
  DALI_TEST_CHECK( !emitter );

  // Test object creation
  Texture shapeImage = CreateSolidColorTexture( application, Color::GREEN, 5, 5 );
  emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 200, Vector2( 5.f, 10.f ));
  DALI_TEST_CHECK( emitter );

  // Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );
  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 200, Vector2( 5.f, 10.f ));
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );

  // Test copy constructor
  BubbleEmitter emitterCopy( emitter );
  DALI_TEST_CHECK( emitterCopy );

  // Test down cast
  Handle handleEmitter;
  handleEmitter = emitter;
  BubbleEmitter downCastEmitter = BubbleEmitter::DownCast( handleEmitter );
  DALI_TEST_CHECK( downCastEmitter );
  END_TEST;
}

int UtcDaliBubbleEmitterDownCast01(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBubbleEmitterDownCast01 ");

  Texture shapeImage = CreateSolidColorTexture( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 200, Vector2( 5.f, 10.f ));

  BaseHandle handle(emitter);
  BubbleEmitter emitter2 = BubbleEmitter::DownCast(handle);
  DALI_TEST_EQUALS( (bool)emitter2, true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBubbleEmitterDownCast02(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBubbleEmitterDownCast02 ");

  Handle handle = Handle::New(); // Create a custom object
  BubbleEmitter emitter = BubbleEmitter::DownCast(handle);
  DALI_TEST_EQUALS( (bool)emitter, false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBubbleEmitterGetRootActor(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterGetRootActor " );

  Texture shapeImage = CreateSolidColorTexture( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 270, Vector2( 5.f, 10.f ));

  Actor root = emitter.GetRootActor();
  DALI_TEST_CHECK( root );
  DALI_TEST_CHECK( root.GetChildCount() == 0 );
  END_TEST;
}

int UtcDaliBubbleEmitterSetBackground(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterSetBackground " );

  Texture shapeImage = CreateSolidColorTexture( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 200, Vector2( 5.f, 10.f ));

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  unsigned int taskCount = taskList.GetTaskCount();

  Texture bgImage = CreateSolidColorTexture( application, Color::RED, 50, 50 );
  emitter.SetBackground( bgImage, Vector3(0.f, 0.f, 0.5f) );

  DALI_TEST_CHECK( taskList.GetTaskCount() == taskCount+1 );

  Wait(application, 500);
  DALI_TEST_CHECK( taskList.GetTaskCount() == taskCount );
  END_TEST;
}

//TODO: test case for BubbleEmitter::SetShapeImage(Image)
// To test that the bubble-shape image is successfully switched in the sampler
/*int UtcDaliBubbleEmitterSetShapeImage(void)
{
}*/

int UtcDaliBubbleEmitterSetBubbleScale(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterSetBubbleScale " );

  Texture shapeImage = CreateSolidColorTexture( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 150, Vector2( 5.f, 10.f ));
  DALI_TEST_CHECK(emitter);
  Actor root = emitter.GetRootActor();
  Stage::GetCurrent().Add( root );
  root.SetPosition( Vector3::ZERO );
  root.SetParentOrigin( ParentOrigin::CENTER );
  root.SetAnchorPoint( AnchorPoint::CENTER );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  Wait(application);

  float scaleValue;
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uDynamicScale", scaleValue ) );
  DALI_TEST_EQUALS( scaleValue, 1.f, TEST_LOCATION );

  emitter.SetBubbleScale( 2.f );
  Wait(application);
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uDynamicScale", scaleValue ) );
  DALI_TEST_EQUALS( scaleValue, 2.f, TEST_LOCATION );

  emitter.SetBubbleScale( 0.5f );
  Wait(application);
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uDynamicScale", scaleValue ) );
  DALI_TEST_EQUALS( scaleValue, 0.5f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliBubbleEmitterSetBubbleDensity01(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterSetBubbleDensity " );

  Texture shapeImage = CreateSolidColorTexture( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 200, Vector2( 5.f, 10.f ));

  try
  {
    emitter.SetBubbleDensity( 3.f );
    DALI_TEST_CHECK(true);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "density>0 && density<=9", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliBubbleEmitterSetBubbleDensity02(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterSetBubbleDensity " );

  Texture shapeImage = CreateSolidColorTexture( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 200, Vector2( 5.f, 10.f ));

  try
  {
    emitter.SetBubbleDensity( 10.f );
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT(e, "density>0 && density<=9", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliBubbleEmitterEmitBubble(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterEmitBubble " );

  Texture shapeImage1 = CreateSolidColorTexture( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage1, 200, Vector2( 5.f, 10.f ));

  Actor root = emitter.GetRootActor();
  Renderer bubbleRenderer = root.GetRendererAt( 0 );
  Stage::GetCurrent().Add( root );
  DALI_TEST_CHECK( bubbleRenderer );

  Property::Index propertyIndex0 = bubbleRenderer.GetPropertyIndex( "uPercentage[0]" );
  Property::Index propertyIndex1 = bubbleRenderer.GetPropertyIndex( "uPercentage[1]" );
  float value0, value1;

  Animation animation = Animation::New( 0.5f );
  emitter.EmitBubble( animation, Vector2(40.f,40.f), Vector2(-5.f,-5.f), Vector2(30.f,30.f) );
  emitter.EmitBubble( animation, Vector2(10.f,10.f), Vector2(5.f,5.f), Vector2(30.f,30.f) );
  (bubbleRenderer.GetProperty(propertyIndex0)).Get( value0 );
  (bubbleRenderer.GetProperty(propertyIndex1)).Get( value1 );
  DALI_TEST_EQUALS(value0, 0.f, TEST_LOCATION );
  DALI_TEST_EQUALS(value1, 0.f, TEST_LOCATION );

  animation.Play();

  Wait(application, 300);
  propertyIndex0 = bubbleRenderer.GetPropertyIndex( "uPercentage[0]" );
  propertyIndex1 = bubbleRenderer.GetPropertyIndex( "uPercentage[1]" );
  (bubbleRenderer.GetProperty(propertyIndex0)).Get( value0 );
  (bubbleRenderer.GetProperty(propertyIndex1)).Get( value1 );
  DALI_TEST_CHECK( value0 >= 0.6f );
  DALI_TEST_CHECK( value1 >= 0.6f );

  Wait(application,500);
  (bubbleRenderer.GetProperty(propertyIndex0)).Get( value0 );
  (bubbleRenderer.GetProperty(propertyIndex1)).Get( value1 );
  DALI_TEST_EQUALS(value0, 1.f, TEST_LOCATION );
  DALI_TEST_EQUALS(value1, 1.f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBubbleEmitterRestore(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterRestore " );

  Vector2 movementArea(50.f,50.f);
  Texture shapeImage = CreateSolidColorTexture( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( movementArea,shapeImage, 90, Vector2( 5.f, 10.f ));
  Actor root = emitter.GetRootActor();
  Stage::GetCurrent().Add( root );
  root.SetPosition( Vector3::ZERO );
  root.SetParentOrigin( ParentOrigin::CENTER );
  root.SetAnchorPoint( AnchorPoint::CENTER );

  Renderer renderer = root.GetRendererAt( 0 );
  DALI_TEST_CHECK( renderer );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  float percentageValue;
  Vector4 startEndPosValue;

  Animation animation = Animation::New( 0.5f );
  emitter.EmitBubble( animation, Vector2(40.f,40.f), Vector2(-5.f,-5.f), Vector2(30.f,30.f) );

  Wait(application);

  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uPercentage[0]", percentageValue ) );
  DALI_TEST_EQUALS( percentageValue, 0.f, TEST_LOCATION );

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uStartEndPosition[0]", startEndPosValue ) );
  DALI_TEST_EQUALS( startEndPosValue.x, 40.f - movementArea.x*0.5f, TEST_LOCATION );
  DALI_TEST_EQUALS( startEndPosValue.y, 40.f- movementArea.x*0.5f, TEST_LOCATION );

  animation.Play();
  Wait(application, 200);
  animation.Clear();

  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uPercentage[0]", percentageValue ) );
  DALI_TEST_CHECK( percentageValue < 0.5f && percentageValue >= 0.4);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uStartEndPosition[0]", startEndPosValue ) );
  DALI_TEST_EQUALS( startEndPosValue.x, 40.f- movementArea.x*0.5f, TEST_LOCATION );
  DALI_TEST_EQUALS( startEndPosValue.y, 40.f- movementArea.x*0.5f, TEST_LOCATION );

  emitter.Restore();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uPercentage[0]", percentageValue ) );
  DALI_TEST_EQUALS( percentageValue, 0.f, TEST_LOCATION );

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uStartEndPosition[0]", startEndPosValue ) );
  DALI_TEST_EQUALS( startEndPosValue,  Vector4::ZERO, TEST_LOCATION );

  END_TEST;
}
