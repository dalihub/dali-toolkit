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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali/devel-api/rendering/renderer.h>
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

static Image CreateSolidColorImage( ToolkitTestApplication& application, const Vector4& color, unsigned int width, unsigned int height )
{
  BufferImage imageData = BufferImage::New( width, height, Pixel::RGBA8888 );

  // Create the image
  PixelBuffer* pixbuf = imageData.GetBuffer();
  unsigned int size = width * height;

  for( size_t i = 0; i < size; i++ )
    {
      pixbuf[i*4+0] = 0xFF * color.r;
      pixbuf[i*4+1] = 0xFF * color.g;
      pixbuf[i*4+2] = 0xFF * color.b;
      pixbuf[i*4+3] = 0xFF * color.a;
    }
  imageData.Update();

  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE );
  application.SendNotification();
  application.Render(RENDER_FRAME_INTERVAL);
  application.Render(RENDER_FRAME_INTERVAL);
  application.SendNotification();

  return imageData;
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
  Image shapeImage = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
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

  Image shapeImage = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
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

  Image shapeImage = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 270, Vector2( 5.f, 10.f ));

  Actor root = emitter.GetRootActor();
  DALI_TEST_CHECK( root );
  DALI_TEST_CHECK( root.GetChildCount() == 3 );
  END_TEST;
}

int UtcDaliBubbleEmitterSetBackground(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterSetBackground " );

  Image shapeImage = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 200, Vector2( 5.f, 10.f ));

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  unsigned int taskCount = taskList.GetTaskCount();

  Image bgImage = CreateSolidColorImage( application, Color::RED, 50, 50 );
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

  Image shapeImage = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 150, Vector2( 5.f, 10.f ));
  Actor root = emitter.GetRootActor();
  Stage::GetCurrent().Add( root );

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

  Image shapeImage = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
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

  Image shapeImage = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
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

int UtcDaliBubbleEmitterSetBlendMode(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterSetBlendMode " );

  Image shapeImage = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 150, Vector2( 5.f, 10.f ));
  Actor root = emitter.GetRootActor();
  Stage::GetCurrent().Add( root );

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  Wait(application);
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA, glAbstraction.GetLastBlendFuncSrcRgb(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_ALPHA, glAbstraction.GetLastBlendFuncDstRgb(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE, glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_ALPHA, glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION );

  emitter.SetBlendMode( true );
  Wait(application);
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA, glAbstraction.GetLastBlendFuncSrcRgb(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE, glAbstraction.GetLastBlendFuncDstRgb(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ZERO, glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE, glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION );

  emitter.SetBlendMode( false );
  Wait(application);
  DALI_TEST_EQUALS( (GLenum)GL_SRC_ALPHA, glAbstraction.GetLastBlendFuncSrcRgb(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_ALPHA, glAbstraction.GetLastBlendFuncDstRgb(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE, glAbstraction.GetLastBlendFuncSrcAlpha(), TEST_LOCATION );
  DALI_TEST_EQUALS( (GLenum)GL_ONE_MINUS_SRC_ALPHA, glAbstraction.GetLastBlendFuncDstAlpha(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliBubbleEmitterEmitBubble(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterEmitBubble " );

  Image shapeImage1 = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage1, 200, Vector2( 5.f, 10.f ));

  Actor root = emitter.GetRootActor();
  Actor bubbleMesh = root.GetChildAt( 0 );
  Stage::GetCurrent().Add( root );
  DALI_TEST_CHECK( bubbleMesh );

  Property::Index propertyIndex0 = bubbleMesh.GetPropertyIndex( "uPercentage[0]" );
  Property::Index propertyIndex1 = bubbleMesh.GetPropertyIndex( "uPercentage[1]" );
  float value0, value1;

  Animation animation = Animation::New( 0.5f );
  emitter.EmitBubble( animation, Vector2(40.f,40.f), Vector2(-5.f,-5.f), Vector2(30.f,30.f) );
  emitter.EmitBubble( animation, Vector2(10.f,10.f), Vector2(5.f,5.f), Vector2(30.f,30.f) );
  (bubbleMesh.GetProperty(propertyIndex0)).Get( value0 );
  (bubbleMesh.GetProperty(propertyIndex1)).Get( value1 );
  DALI_TEST_EQUALS(value0, 0.f, TEST_LOCATION );
  DALI_TEST_EQUALS(value1, 0.f, TEST_LOCATION );

  animation.Play();

  Wait(application, 300);
  propertyIndex0 = bubbleMesh.GetPropertyIndex( "uPercentage[0]" );
  propertyIndex1 = bubbleMesh.GetPropertyIndex( "uPercentage[1]" );
  (bubbleMesh.GetProperty(propertyIndex0)).Get( value0 );
  (bubbleMesh.GetProperty(propertyIndex1)).Get( value1 );
  DALI_TEST_CHECK( value0 >= 0.6f );
  DALI_TEST_CHECK( value1 >= 0.6f );

  Wait(application,500);
  (bubbleMesh.GetProperty(propertyIndex0)).Get( value0 );
  (bubbleMesh.GetProperty(propertyIndex1)).Get( value1 );
  DALI_TEST_EQUALS(value0, 1.f, TEST_LOCATION );
  DALI_TEST_EQUALS(value1, 1.f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBubbleEmitterRestore(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterRestore " );

  Image shapeImage = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 90, Vector2( 5.f, 10.f ));
  Actor root = emitter.GetRootActor();
  Stage::GetCurrent().Add( root );

  Actor bubbleMesh = root.GetChildAt( 0 );
  Renderer renderer = bubbleMesh.GetRendererAt( 0 );
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
  DALI_TEST_EQUALS( startEndPosValue.x, 40.f, TEST_LOCATION );
  DALI_TEST_EQUALS( startEndPosValue.y, 40.f, TEST_LOCATION );

  animation.Play();
  Wait(application, 200);
  animation.Clear();

  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uPercentage[0]", percentageValue ) );
  DALI_TEST_CHECK( percentageValue < 0.5f && percentageValue >= 0.4);

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uStartEndPosition[0]", startEndPosValue ) );
  DALI_TEST_EQUALS( startEndPosValue.x, 40.f, TEST_LOCATION );
  DALI_TEST_EQUALS( startEndPosValue.y, 40.f, TEST_LOCATION );

  emitter.Restore();
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uPercentage[0]", percentageValue ) );
  DALI_TEST_EQUALS( percentageValue, 0.f, TEST_LOCATION );

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uStartEndPosition[0]", startEndPosValue ) );
  DALI_TEST_EQUALS( startEndPosValue,  Vector4::ZERO, TEST_LOCATION );
  END_TEST;
}
