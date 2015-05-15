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
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/mesh-actor.h>
#include <dali/devel-api/modeling/material.h>

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

int UtcDaliBubbleEmitterGetRootActor(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterGetRootActor " );

  Image shapeImage = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage, 200, Vector2( 5.f, 10.f ));

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

int UtcDaliBubbleEmitterSetShapeImage(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterSetShapeImage " );

  Image shapeImage1 = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage1, 200, Vector2( 5.f, 10.f ));

  Actor root = emitter.GetRootActor();
  MeshActor bubbleMesh = MeshActor::DownCast( root.GetChildAt( 0 ) );
  Material material = bubbleMesh.GetMaterial();

  DALI_TEST_CHECK( material.GetDiffuseTexture() == shapeImage1 );

  Image shapeImage2 = CreateSolidColorImage( application, Color::RED, 8, 8 );
  emitter.SetShapeImage( shapeImage2 );

  DALI_TEST_CHECK( material.GetDiffuseTexture() == shapeImage2 );
  END_TEST;
}

int UtcDaliBubbleEmitterSetBubbleScale(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterSetBubbleScale " );

  Image shapeImage1 = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage1, 200, Vector2( 5.f, 10.f ));

  Actor root = emitter.GetRootActor();
  MeshActor bubbleMesh = MeshActor::DownCast( root.GetChildAt( 0 ) );
  ShaderEffect effect = bubbleMesh.GetShaderEffect();
  DALI_TEST_CHECK( effect );

  Property::Index scalePropertyIndex = effect.GetPropertyIndex( "uDynamicScale" );
  float scaleValue;
  (effect.GetProperty(scalePropertyIndex)).Get( scaleValue );
  DALI_TEST_EQUALS(scaleValue, 1.f, TEST_LOCATION );

  emitter.SetBubbleScale( 2.f );
  application.SendNotification();
  application.Render();
  (effect.GetProperty(scalePropertyIndex)).Get( scaleValue );
  DALI_TEST_EQUALS(scaleValue, 2.f, TEST_LOCATION );

  emitter.SetBubbleScale( 0.5f );
  application.SendNotification();
  application.Render();
  (effect.GetProperty(scalePropertyIndex)).Get( scaleValue );
  DALI_TEST_EQUALS(scaleValue, 0.5f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBubbleEmitterSetBubbleDensity01(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterSetBubbleDensity " );

  Image shapeImage1 = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage1, 200, Vector2( 5.f, 10.f ));

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

  Image shapeImage1 = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage1, 200, Vector2( 5.f, 10.f ));

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

  Image shapeImage1 = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage1, 200, Vector2( 5.f, 10.f ));

  Actor root = emitter.GetRootActor();
  MeshActor bubbleMesh = MeshActor::DownCast( root.GetChildAt( 0 ) );

  BlendingFactor::Type srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha;

  emitter.SetBlendMode( true );
  bubbleMesh.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
  DALI_TEST_CHECK( srcFactorRgb == BlendingFactor::SRC_ALPHA );
  DALI_TEST_CHECK( destFactorRgb == BlendingFactor::ONE );
  DALI_TEST_CHECK( srcFactorAlpha == BlendingFactor::ZERO );
  DALI_TEST_CHECK( destFactorAlpha == BlendingFactor::ONE );

  emitter.SetBlendMode( false );
  bubbleMesh.GetBlendFunc( srcFactorRgb, destFactorRgb, srcFactorAlpha, destFactorAlpha );
  DALI_TEST_CHECK( srcFactorRgb == BlendingFactor::SRC_ALPHA );
  DALI_TEST_CHECK( destFactorRgb == BlendingFactor::ONE_MINUS_SRC_ALPHA );
  DALI_TEST_CHECK( srcFactorAlpha == BlendingFactor::ONE );
  DALI_TEST_CHECK( destFactorAlpha == BlendingFactor::ONE_MINUS_SRC_ALPHA );
  END_TEST;
}

int UtcDaliBubbleEmitterEmitBubble(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterEmitBubble " );

  Image shapeImage1 = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage1, 200, Vector2( 5.f, 10.f ));

  Actor root = emitter.GetRootActor();
  MeshActor bubbleMesh = MeshActor::DownCast( root.GetChildAt( 0 ) );
  ShaderEffect effect = bubbleMesh.GetShaderEffect();
  DALI_TEST_CHECK( effect );

  Property::Index propertyIndex0 = effect.GetPropertyIndex( "uPercentage[0]" );
  Property::Index propertyIndex1 = effect.GetPropertyIndex( "uPercentage[1]" );
  float value0, value1;

  Animation animation = Animation::New( 0.5f );
  emitter.EmitBubble( animation, Vector2(40.f,40.f), Vector2(-5.f,-5.f), Vector2(30.f,30.f) );
  emitter.EmitBubble( animation, Vector2(10.f,10.f), Vector2(5.f,5.f), Vector2(30.f,30.f) );
  (effect.GetProperty(propertyIndex0)).Get( value0 );
  (effect.GetProperty(propertyIndex1)).Get( value1 );
  DALI_TEST_EQUALS(value0, 0.f, TEST_LOCATION );
  DALI_TEST_EQUALS(value1, 0.f, TEST_LOCATION );

  animation.Play();

  Wait(application, 300);
  (effect.GetProperty(propertyIndex0)).Get( value0 );
  (effect.GetProperty(propertyIndex1)).Get( value1 );
  DALI_TEST_CHECK( value0 >= 0.6f );
  DALI_TEST_CHECK( value1 >= 0.6f );

  Wait(application, 600);
  (effect.GetProperty(propertyIndex0)).Get( value0 );
  (effect.GetProperty(propertyIndex1)).Get( value1 );
  DALI_TEST_EQUALS(value0, 1.f, TEST_LOCATION );
  DALI_TEST_EQUALS(value1, 1.f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBubbleEmitterStartExplosion(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterStartExplosion " );

  Image shapeImage1 = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage1, 200, Vector2( 5.f, 10.f ));
  Actor root = emitter.GetRootActor();
  MeshActor bubbleMesh = MeshActor::DownCast( root.GetChildAt( 0 ) );
  ShaderEffect effect = bubbleMesh.GetShaderEffect();
  DALI_TEST_CHECK( effect );

  Property::Index propertyIndex = effect.GetPropertyIndex( "uMagnification" );
  float value;
  (effect.GetProperty(propertyIndex)).Get( value );
  DALI_TEST_EQUALS(value, 1.f, TEST_LOCATION );

  emitter.StartExplosion( 0.4, 4.f );

  Wait(application, 200); // 0.2s
  (effect.GetProperty(propertyIndex)).Get( value );
  DALI_TEST_CHECK( value >= 2.f );

  Wait(application, 100); // 0.3s
  (effect.GetProperty(propertyIndex)).Get( value );
  DALI_TEST_CHECK( value >= 3.f );

  Wait(application, 100); // 0.4s
  (effect.GetProperty(propertyIndex)).Get( value );
  DALI_TEST_EQUALS(value, 1.f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBubbleEmitterRestore(void)
{
  ToolkitTestApplication application;
  tet_infoline( " UtcDaliBubbleEmitterRestore " );

  Image shapeImage1 = CreateSolidColorImage( application, Color::GREEN, 5, 5 );
  BubbleEmitter emitter = BubbleEmitter::New( Vector2(50.f,50.f),shapeImage1, 200, Vector2( 5.f, 10.f ));
  Actor root = emitter.GetRootActor();
  MeshActor bubbleMesh = MeshActor::DownCast( root.GetChildAt( 0 ) );
  ShaderEffect effect = bubbleMesh.GetShaderEffect();
  DALI_TEST_CHECK( effect );

  Property::Index percentagePropertyIndex = effect.GetPropertyIndex( "uPercentage[0]" );
  float percentage;

  Animation animation = Animation::New( 0.5f );
  emitter.EmitBubble( animation, Vector2(40.f,40.f), Vector2(-5.f,-5.f), Vector2(30.f,30.f) );
  (effect.GetProperty(percentagePropertyIndex)).Get( percentage );
  DALI_TEST_EQUALS(percentage, 0.f, TEST_LOCATION );

  animation.Play();
  Wait(application, 200);
  animation.Clear();

  (effect.GetProperty(percentagePropertyIndex)).Get( percentage );
  DALI_TEST_CHECK( percentage < 0.5f && percentage >= 0.4);

  emitter.Restore();
  application.SendNotification();
  application.Render();

  (effect.GetProperty(percentagePropertyIndex)).Get( percentage );
  DALI_TEST_EQUALS(percentage, 1.f, TEST_LOCATION );
  END_TEST;
}
