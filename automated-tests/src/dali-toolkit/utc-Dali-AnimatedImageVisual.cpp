
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
 */

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-timer.h>
#include <toolkit-event-thread-callback.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/animated-image-visual-actions-devel.h>
#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

void dali_animated_image_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_animated_image_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR  "/application-icon-%02d.png";
const char* TEST_GIF_FILE_NAME = TEST_RESOURCE_DIR "/anim.gif";
}


void CopyUrlsIntoArray( Property::Array& urls, int startIndex=0 )
{
  for( int i=20+startIndex;i<=30;++i)
  {
    char* url;
    if(asprintf(&url, TEST_IMAGE_FILE_NAME, i) > 0)
    {
      Property::Value value(url);
      urls.Add(value);
      free(url);
    }
  }
}

int UtcDaliAnimatedImageVisualGetPropertyMap01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedImageVisualGetPropertyMap" );

  // request AnimatedImageVisual with a property map
  VisualFactory factory = VisualFactory::Get();
  Visual::Base animatedImageVisual = factory.CreateVisual(
    Property::Map()
    .Add( Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE )
    .Add( ImageVisual::Property::URL, TEST_GIF_FILE_NAME )
    .Add( ImageVisual::Property::PIXEL_AREA, Vector4() )
    .Add( ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT )
    .Add( ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT ));

  Property::Map resultMap;
  animatedImageVisual.CreatePropertyMap( resultMap );
  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find( Toolkit::Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::ANIMATED_IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL,  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_GIF_FILE_NAME );

  // request AnimatedImageVisual with an URL
  Visual::Base animatedImageVisual2 = factory.CreateVisual( TEST_GIF_FILE_NAME, ImageDimensions() );
  resultMap.Clear();
  animatedImageVisual2.CreatePropertyMap( resultMap );
  // check the property values from the returned map from a visual
  value = resultMap.Find( Toolkit::Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::ANIMATED_IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL,  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_GIF_FILE_NAME );

  END_TEST;
}


int UtcDaliAnimatedImageVisualGetPropertyMap02(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedImageVisualGetPropertyMap for multi image" );

  // request AnimatedImageVisual with a property map
  VisualFactory factory = VisualFactory::Get();
  Property::Array urls;
  CopyUrlsIntoArray( urls );

  Visual::Base animatedImageVisual = factory.CreateVisual(
    Property::Map()
    .Add( Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE )
    .Add( "url", urls )
    .Add( "batchSize", 4 )
    .Add( "cacheSize", 8 )
    .Add( "loopCount", 10 )
    .Add( "frameDelay", 200 )
    .Add( "pixelArea", Vector4() )
    .Add( "wrapModeU", WrapMode::REPEAT )
    .Add( "wrapModeV", WrapMode::DEFAULT ));

  Property::Map resultMap;
  animatedImageVisual.CreatePropertyMap( resultMap );
  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find( Toolkit::Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::ANIMATED_IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL, "url" );
  DALI_TEST_CHECK( value );
  Property::Array* resultUrls = value->GetArray();
  DALI_TEST_CHECK( resultUrls );
  DALI_TEST_EQUALS( resultUrls->Count(), urls.Count(), TEST_LOCATION );

  value = resultMap.Find( ImageVisual::Property::BATCH_SIZE, "batchSize" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), 4, TEST_LOCATION );

  value = resultMap.Find( ImageVisual::Property::CACHE_SIZE, "cacheSize" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), 8, TEST_LOCATION );

  value = resultMap.Find( Toolkit::DevelImageVisual::Property::LOOP_COUNT, "loopCount" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), 10, TEST_LOCATION );

  value = resultMap.Find( ImageVisual::Property::FRAME_DELAY, "frameDelay" );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get<int>(), 200, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimatedImageVisualGetPropertyMap03(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedImageVisualGetPropertyMap" );

  // request AnimatedImageVisual with a property map
  VisualFactory factory = VisualFactory::Get();
  Visual::Base animatedImageVisual = factory.CreateVisual(
    Property::Map()
    .Add( Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE )
    .Add( ImageVisual::Property::URL, TEST_GIF_FILE_NAME )
    .Add( ImageVisual::Property::BATCH_SIZE, 1 )
    .Add( ImageVisual::Property::CACHE_SIZE, 1 )
    .Add( ImageVisual::Property::SYNCHRONOUS_LOADING, false ));

  Property::Map resultMap;
  animatedImageVisual.CreatePropertyMap( resultMap );

  // check the property values from the returned map from a visual
  Property::Value* value = resultMap.Find( Toolkit::Visual::Property::TYPE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == Visual::ANIMATED_IMAGE );

  value = resultMap.Find( ImageVisual::Property::URL,  Property::STRING );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<std::string>() == TEST_GIF_FILE_NAME );

  value = resultMap.Find( ImageVisual::Property::BATCH_SIZE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 2 );

  value = resultMap.Find( ImageVisual::Property::CACHE_SIZE,  Property::INTEGER );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get<int>() == 2 );

  END_TEST;
}


int UtcDaliAnimatedImageVisualSynchronousLoading(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE );
    propertyMap.Insert(ImageVisual::Property::URL, TEST_GIF_FILE_NAME );
    propertyMap.Insert( ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert( ImageVisual::Property::CACHE_SIZE, 2);
    propertyMap.Insert( ImageVisual::Property::FRAME_DELAY, 20);
    propertyMap.Insert( ImageVisual::Property::SYNCHRONOUS_LOADING, true);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base visual = factory.CreateVisual( propertyMap );

    DummyControl dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Stage::GetCurrent().Add( dummyControl );

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( Test::GetTimerCount(), 1, TEST_LOCATION );
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 2, TEST_LOCATION );

    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::JUMP_TO, 3 );

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 3, TEST_LOCATION );

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliAnimatedImageVisualJumpToAction(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();

  Property::Array urls;
  CopyUrlsIntoArray( urls );

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE );
    propertyMap.Insert( ImageVisual::Property::URL, Property::Value(urls) );
    propertyMap.Insert( ImageVisual::Property::BATCH_SIZE, 4);
    propertyMap.Insert( ImageVisual::Property::CACHE_SIZE, 12);
    propertyMap.Insert( ImageVisual::Property::FRAME_DELAY, 20);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base visual = factory.CreateVisual( propertyMap );

    DummyControl dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Stage::GetCurrent().Add( dummyControl );
    application.SendNotification();
    application.Render(20);

    tet_infoline( "Ready the visual after the visual is on stage" );
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 4 ), true, TEST_LOCATION );

    tet_infoline( "Test that a timer has been started" );
    DALI_TEST_EQUALS( Test::GetTimerCount(), 1, TEST_LOCATION );

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS( gl.GetLastGenTextureId(), 4, TEST_LOCATION );

    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::STOP, Property::Map() );

    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 4, TEST_LOCATION );

    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::JUMP_TO, 20 );

    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 4, TEST_LOCATION );

    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::JUMP_TO, 6 );

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 6 ), true, TEST_LOCATION );
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 4, TEST_LOCATION );

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliAnimatedImageVisualStopBehavior(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();

  Property::Array urls;
  CopyUrlsIntoArray( urls );

  {
    Property::Map propertyMap;
    propertyMap.Insert( Visual::Property::TYPE, Visual::IMAGE );
    propertyMap.Insert( ImageVisual::Property::URL, Property::Value(urls) );
    propertyMap.Insert( DevelImageVisual::Property::STOP_BEHAVIOR, DevelImageVisual::StopBehavior::FIRST_FRAME);
    propertyMap.Insert( ImageVisual::Property::BATCH_SIZE, 4);
    propertyMap.Insert( ImageVisual::Property::CACHE_SIZE, 8);
    propertyMap.Insert( ImageVisual::Property::FRAME_DELAY, 20);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base visual = factory.CreateVisual( propertyMap );

    // Expect that a batch of 4 textures has been requested. These will be serially loaded
    // below.

    DummyControl dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Stage::GetCurrent().Add( dummyControl );
    application.SendNotification();
    application.Render(20);

    tet_infoline( "Ready the visual after the visual is on stage" );
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 4 ), true, TEST_LOCATION );

    tet_infoline( "Test that a timer has been started" );
    DALI_TEST_EQUALS( Test::GetTimerCount(), 1, TEST_LOCATION );

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS( gl.GetLastGenTextureId(), 4, TEST_LOCATION );

    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::STOP, Property::Map() );

    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 4, TEST_LOCATION );

    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::JUMP_TO, 1 );

    // Expect the second batch has been requested
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 4 ), true, TEST_LOCATION );

    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 4, TEST_LOCATION );

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliAnimatedImageVisualAnimatedImage01(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::ANIMATED_IMAGE );
    propertyMap.Insert( ImageVisual::Property::URL, TEST_GIF_FILE_NAME );
    propertyMap.Insert( ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert( ImageVisual::Property::CACHE_SIZE, 4);
    propertyMap.Insert( ImageVisual::Property::FRAME_DELAY, 20);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base visual = factory.CreateVisual( propertyMap );

    // Expect that a batch of 4 textures has been requested. These will be serially loaded
    // below.

    DummyControl dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Stage::GetCurrent().Add( dummyControl );

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS( gl.GetLastGenTextureId(), 2, TEST_LOCATION );

    tet_infoline( "Test that a timer has been started" );

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    Test::EmitGlobalTimerSignal();

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render(20);

    DALI_TEST_EQUALS( gl.GetLastGenTextureId(), 4, TEST_LOCATION );

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(20);
  DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliAnimatedImageVisualMultiImage01(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();

  Property::Array urls;
  CopyUrlsIntoArray( urls );

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE );
    propertyMap.Insert( ImageVisual::Property::URL, Property::Value(urls) );
    propertyMap.Insert( ImageVisual::Property::BATCH_SIZE, 4);
    propertyMap.Insert( ImageVisual::Property::CACHE_SIZE, 8);
    propertyMap.Insert( ImageVisual::Property::FRAME_DELAY, 100);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base visual = factory.CreateVisual( propertyMap );

    // Expect that a batch of 4 textures has been requested. These will be serially loaded
    // below.

    DummyControl dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Stage::GetCurrent().Add( dummyControl );
    application.SendNotification();
    application.Render(16);

    tet_infoline( "Ready the visual after the visual is on stage" );
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 4 ), true, TEST_LOCATION );

    tet_infoline( "Test that a timer has been started" );
    DALI_TEST_EQUALS( Test::GetTimerCount(), 1, TEST_LOCATION );

    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS( gl.GetLastGenTextureId(), 4, TEST_LOCATION );
    DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

    tet_infoline( "Test that after 1 tick, and file loads completed, that we have 7 textures" );
    Test::EmitGlobalTimerSignal();

    // Expect the second batch has been requested
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 4 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 7, TEST_LOCATION );


    tet_infoline( "Test that after 2 ticks that we have 6 textures" );

    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 6, TEST_LOCATION );

    tet_infoline("And that at least 2 textures were requested");
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_LOCATION );
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 8, TEST_LOCATION );


    tet_infoline( "Test that after 3rd tick that we have 7 textures and 1 request" );
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 7, TEST_LOCATION );

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 8, TEST_LOCATION );

    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimatedImageVisualMultiImage02(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();

  tet_infoline( "Test that the animated visual still works with zero sized cache" );

  {
    Property::Array urls;
    CopyUrlsIntoArray( urls );

    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE );
    propertyMap.Insert( ImageVisual::Property::URL, Property::Value(urls) );
    propertyMap.Insert( ImageVisual::Property::BATCH_SIZE, 2);
    propertyMap.Insert( ImageVisual::Property::CACHE_SIZE, 2);
    propertyMap.Insert( ImageVisual::Property::FRAME_DELAY, 100);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base visual = factory.CreateVisual( propertyMap ); // TexMgr::Request load tId:0

    // Expect that each image is loaded each tick

    DummyControl dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Stage::GetCurrent().Add( dummyControl );
    application.SendNotification();
    application.Render(16);

    tet_infoline( "Ready the visual after the visual is on stage" );
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
    application.SendNotification();
    application.Render(16);//glGenTextures 1
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 1, TEST_LOCATION );

    tet_infoline( "Test that each tick, a new image is requested" );
    Test::EmitGlobalTimerSignal(); // TexMgr::Remove tId:0
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1, 10 ), true, TEST_LOCATION );
    application.SendNotification();
    application.Render(16);//glGenTextures 2
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 1, TEST_LOCATION );

    tet_infoline( "Test that each tick, a new image is requested" );
    Test::EmitGlobalTimerSignal(); // Internal::~TextureSet()
    application.SendNotification();
    application.Render(16);//glDeleteTextures 2
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1, 10 ), true, TEST_LOCATION );
    application.SendNotification();
    application.Render(16);//glGenTextures 3
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 1, TEST_LOCATION );

    tet_infoline( "Test that each tick, a new image is requested" );
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);//glDeleteTextures 3
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1, 10 ), true, TEST_LOCATION );
    application.SendNotification();
    application.Render(16);//Gen4
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 1, TEST_LOCATION );
    dummyControl.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliAnimatedImageVisualMultiImage03(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();

  {
    Property::Array urls1, urls2;
    CopyUrlsIntoArray( urls1 );
    CopyUrlsIntoArray( urls2 );

    Property::Map animatedImageMap1;
    animatedImageMap1.Insert(Visual::Property::TYPE, Visual::IMAGE );
    animatedImageMap1.Insert( ImageVisual::Property::URL, Property::Value(urls1) );
    animatedImageMap1.Insert( ImageVisual::Property::BATCH_SIZE, 3);
    animatedImageMap1.Insert( ImageVisual::Property::CACHE_SIZE, 3);
    animatedImageMap1.Insert( ImageVisual::Property::FRAME_DELAY, 100);

    Property::Map animatedImageMap2;
    animatedImageMap2.Insert(Visual::Property::TYPE, Visual::IMAGE );
    animatedImageMap2.Insert( ImageVisual::Property::URL, Property::Value(urls2) );
    animatedImageMap2.Insert( ImageVisual::Property::BATCH_SIZE, 2);
    animatedImageMap2.Insert( ImageVisual::Property::CACHE_SIZE, 2);
    animatedImageMap2.Insert( ImageVisual::Property::FRAME_DELAY, 100);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base animatedImageVisual1 = factory.CreateVisual( animatedImageMap1 );

    tet_infoline( "Create two image views with the same URLs, offset by 1 frame.");

    DummyControl dummyControl1 = DummyControl::New(true);
    Impl::DummyControl& dummyImpl1 = static_cast<Impl::DummyControl&>(dummyControl1.GetImplementation());
    dummyImpl1.RegisterVisual( DummyControl::Property::TEST_VISUAL, animatedImageVisual1 );
    dummyControl1.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Stage::GetCurrent().Add( dummyControl1 );

    application.SendNotification();
    application.Render(16);

    tet_infoline( "Ready the requested image after the first visual is on stage" );
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 3 ), true, TEST_LOCATION );
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 3, TEST_LOCATION );

    Visual::Base animatedImageVisual2 = factory.CreateVisual( animatedImageMap2 );
    DummyControl dummyControl2 = DummyControl::New(true);
    Impl::DummyControl& dummyImpl2 = static_cast<Impl::DummyControl&>(dummyControl2.GetImplementation());
    dummyImpl2.RegisterVisual( DummyControl::Property::TEST_VISUAL, animatedImageVisual2 );
    dummyControl2.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Stage::GetCurrent().Add( dummyControl2 );
    application.SendNotification();
    application.Render(16);

    tet_infoline( "The texture cache should be holding the requested images; check that the renderer has a texture" );
    TextureSet ts = dummyControl2.GetRendererAt(0).GetTextures();
    Texture t1 = ts.GetTexture( 0 );
    DALI_TEST_EQUALS( ts.GetTextureCount(), 1, TEST_LOCATION );

    tet_infoline( "Test that on the first tick, 1 new image is requested" );
    Test::EmitGlobalTimerSignal(); // Both visuals should tick

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 3, TEST_LOCATION );

    ts = dummyControl2.GetRendererAt(0).GetTextures();
    Texture t2 = ts.GetTexture( 0 );
    DALI_TEST_CHECK( t1 != t2 );

    dummyControl1.Unparent();
    dummyControl2.Unparent();
  }
  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliAnimatedImageVisualMultiImage04(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Test that if the cache size is the same as the number of urls, that once the cache is full, no new images are loaded" );

  Property::Array urls;
  CopyUrlsIntoArray( urls );

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE );
    propertyMap.Insert( ImageVisual::Property::URL, Property::Value(urls) );
    propertyMap.Insert( ImageVisual::Property::BATCH_SIZE, 6);
    propertyMap.Insert( ImageVisual::Property::CACHE_SIZE, 11);
    propertyMap.Insert( ImageVisual::Property::FRAME_DELAY, 100);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base visual = factory.CreateVisual( propertyMap );

    tet_infoline( "Expect that a batch of 7 textures has been requested." );

    DummyControl dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Stage::GetCurrent().Add( dummyControl );
    application.SendNotification();
    application.Render(16);

    tet_infoline( "Wait for the first batch to complete" );
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 6 ), true, TEST_LOCATION );

    tet_infoline( "Test that a timer has been started" );
    DALI_TEST_EQUALS( Test::GetTimerCount(), 1, TEST_LOCATION );

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS( gl.GetLastGenTextureId(), 6, TEST_LOCATION );
    tet_infoline( "Test that after 1 tick, and 5 file loads completed, that we have 11 textures" );
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    // Expect the second batch has been requested
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 5 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 11, TEST_LOCATION );

    tet_infoline( "Test that after 2 ticks that we have 11 textures and no requests" );

    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1, 5 ), false, TEST_LOCATION );
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 11, TEST_LOCATION );

    tet_infoline( "Test that after 3rd tick that we have 11 textures and no requests" );
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1, 5 ), false, TEST_LOCATION );
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 11, TEST_LOCATION );

    dummyControl.Unparent();
  }

  tet_infoline("Test that removing the visual from stage deletes all textures");
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliAnimatedImageVisualMultiImage05(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();

  tet_infoline( "Test that if the cache size is the same as the number of urls, that removing a partially loaded visual removes all textures" );

  Property::Array urls;
  CopyUrlsIntoArray( urls );

  {
    Property::Map propertyMap;
    propertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE );
    propertyMap.Insert( ImageVisual::Property::URL, Property::Value(urls) );
    propertyMap.Insert( ImageVisual::Property::BATCH_SIZE, 4);
    propertyMap.Insert( ImageVisual::Property::CACHE_SIZE, 11);
    propertyMap.Insert( ImageVisual::Property::FRAME_DELAY, 100);

    VisualFactory factory = VisualFactory::Get();
    Visual::Base visual = factory.CreateVisual( propertyMap );

    tet_infoline( "Expect that a batch of 4 textures has been requested." );

    DummyControl dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    Stage::GetCurrent().Add( dummyControl );
    application.SendNotification();
    application.Render(16);

    tet_infoline( "Wait for the first batch to complete" );
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 4 ), true, TEST_LOCATION );

    tet_infoline( "Test that a timer has been started" );
    DALI_TEST_EQUALS( Test::GetTimerCount(), 1, TEST_LOCATION );

    application.SendNotification();
    application.Render(16);

    tet_infoline( "Test that a timer has been started" );
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    dummyControl.Unparent();
  }

  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 0, TEST_LOCATION );

  tet_infoline( "Test that pending batch of image loads are cancelled instead of uploaded");
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 4 ), true, TEST_LOCATION );
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 0, TEST_LOCATION );

  END_TEST;
}

void TestLoopCount( ToolkitTestApplication &application, DummyControl &dummyControl, uint16_t frameCount, uint16_t loopCount, const char * location )
{
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();

  textureTrace.Enable(true);
  Stage::GetCurrent().Add( dummyControl );

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_INNER_LOCATION( location ) );

  application.SendNotification();
  application.Render();

  tet_infoline( "Test that a timer has been created" );
  DALI_TEST_EQUALS( Test::GetTimerCount(), 1, TEST_INNER_LOCATION( location ) );

  for ( uint16_t i = 0; i < loopCount; i++ )
  {
    for ( uint16_t j = 0; j < frameCount; j++ )
    {
      if( i == 0 && j == 0 )
      {
        continue; // Because first frame is already showed and we call 2nd frame at the first time of timer animation.
      }
      tet_printf( "Test that after %u ticks, and we have %u frame \n", j + 1u, j + 1u );
      Test::EmitGlobalTimerSignal();
      application.SendNotification();
      application.Render(16);

      DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_INNER_LOCATION( location ) );

      application.SendNotification();
      application.Render();
      DALI_TEST_EQUALS( gl.GetNumGeneratedTextures(), 2, TEST_INNER_LOCATION( location ) );
      DALI_TEST_EQUALS( Test::AreTimersRunning(), true, TEST_INNER_LOCATION( location ) );
    }
    tet_printf( "Test Loop %u \n\n", i + 1u );
  }

  tet_printf( "Test that after %u loops, and we have no frame. Timer should stop \n", loopCount );
  Test::EmitGlobalTimerSignal();
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS( Test::AreTimersRunning(), false, TEST_INNER_LOCATION( location ) );

  dummyControl.Unparent();
}

int UtcDaliAnimatedImageVisualLoopCount(void)
{
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliAnimatedImageVisualLoopCount" );

  {
    // request AnimatedImageVisual with a property map
    // Test with no (0) loop count
    VisualFactory factory = VisualFactory::Get();
    Visual::Base animatedImageVisual = factory.CreateVisual(
      Property::Map()
      .Add( Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE )
      .Add( ImageVisual::Property::URL, TEST_GIF_FILE_NAME )
      .Add( ImageVisual::Property::PIXEL_AREA, Vector4() )
      .Add( ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT )
      .Add( ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT )
      .Add( DevelImageVisual::Property::LOOP_COUNT, 0 ));

    DummyControl dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, animatedImageVisual );
    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    TestLoopCount( application, dummyControl, 4, 0, TEST_LOCATION );

    dummyImpl.UnregisterVisual( DummyControl::Property::TEST_VISUAL );
    animatedImageVisual.Reset();

    application.SendNotification();
    application.Render(16);

    // Test with no (1) loop count. Request AnimatedImageVisual with a property map
    animatedImageVisual = factory.CreateVisual(
      Property::Map()
      .Add( Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE )
      .Add( ImageVisual::Property::URL, TEST_GIF_FILE_NAME )
      .Add( ImageVisual::Property::PIXEL_AREA, Vector4() )
      .Add( ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT )
      .Add( ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT )
      .Add( DevelImageVisual::Property::LOOP_COUNT, 1 ));

    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, animatedImageVisual );

    TestLoopCount( application, dummyControl, 4, 1, TEST_LOCATION );

    dummyImpl.UnregisterVisual( DummyControl::Property::TEST_VISUAL );
    animatedImageVisual.Reset();

    application.SendNotification();
    application.Render(16);

    // Test with no (100) loop count. Request AnimatedImageVisual with a property map
    animatedImageVisual = factory.CreateVisual(
      Property::Map()
      .Add( Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE )
      .Add( ImageVisual::Property::URL, TEST_GIF_FILE_NAME )
      .Add( ImageVisual::Property::PIXEL_AREA, Vector4() )
      .Add( ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT )
      .Add( ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT )
      .Add( DevelImageVisual::Property::LOOP_COUNT, 100 ));

    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, animatedImageVisual );

    TestLoopCount( application, dummyControl, 4, 100, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliAnimatedImageVisualPlayback(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();

  tet_infoline( "UtcDaliAnimatedImageVisualPlayback" );

  {
    // request AnimatedImageVisual with a property map
    // Test with forever (-1) loop count
    VisualFactory factory = VisualFactory::Get();
    Visual::Base animatedImageVisual = factory.CreateVisual(
      Property::Map()
      .Add( Toolkit::Visual::Property::TYPE, Visual::ANIMATED_IMAGE )
      .Add( ImageVisual::Property::URL, TEST_GIF_FILE_NAME )
      .Add( ImageVisual::Property::PIXEL_AREA, Vector4() )
      .Add( ImageVisual::Property::WRAP_MODE_U, WrapMode::REPEAT )
      .Add( ImageVisual::Property::WRAP_MODE_V, WrapMode::DEFAULT )
      .Add( DevelImageVisual::Property::LOOP_COUNT, -1 ));

    DummyControl dummyControl = DummyControl::New(true);
    Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
    dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, animatedImageVisual );
    dummyControl.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    textureTrace.Enable(true);
    Stage::GetCurrent().Add( dummyControl );
    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render();

    tet_infoline( "Test that a timer has been created" );
    DALI_TEST_EQUALS( Test::GetTimerCount(), 1, TEST_LOCATION );

    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS( Test::AreTimersRunning(), true, TEST_LOCATION );

    Property::Map attributes;
    tet_infoline( "Test Pause action. Timer should stop after Pause action" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::PAUSE, attributes );
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( Test::AreTimersRunning(), false, TEST_LOCATION );

    tet_infoline( "Test Play action. Timer should Restart after Play action" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::PLAY, attributes );
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS( Test::AreTimersRunning(), true, TEST_LOCATION );

    tet_infoline( "Test Stop action. Timer should stop after Stop action" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::STOP, attributes );
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS( Test::AreTimersRunning(), false, TEST_LOCATION );

    tet_infoline( "Test Play action. Timer should Restart after Play action" );
    DevelControl::DoAction( dummyControl, DummyControl::Property::TEST_VISUAL, Dali::Toolkit::DevelAnimatedImageVisual::Action::PLAY, attributes );
    Test::EmitGlobalTimerSignal();
    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS( Test::AreTimersRunning(), true, TEST_LOCATION );

    dummyControl.Unparent();
  }

  END_TEST;
}