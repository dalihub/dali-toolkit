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
#include <vector>
#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-timer.h>
#include <toolkit-event-thread-callback.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/dali-toolkit.h>
#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

void dali_image_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_image_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
const char* TEST_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR  "/gallery-small-1.jpg";
const char* TEST_BROKEN_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR  "/a-random-nonimage.jpg";
const char* TEST_LARGE_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR "/tbcol.png";
const char* TEST_SMALL_IMAGE_FILE_NAME = TEST_RESOURCE_DIR "/icon-edit.png";
const char* TEST_REMOTE_IMAGE_FILE_NAME = "https://www.tizen.org/sites/all/themes/tizen_theme/logo.png";
const char* TEST_INVALID_FILE_NAME =  TEST_RESOURCE_DIR  "/invalid.jpg";
const char* TEST_REMOTE_INVALID_FILE_NAME = "https://www.tizen.org/invalid.png";
const char* TEST_MASK_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR "/mask.png";
const char* TEST_ROTATED_IMAGE =  TEST_RESOURCE_DIR  "/keyboard-Landscape.jpg";


bool gResourceReadySignalFired = false;
std::vector<int> gReadyIds = {};
void ResourceReadySignal( Control control )
{
  gResourceReadySignalFired = true;
  gReadyIds.push_back(control.GetProperty< int >( Actor::Property::ID ));
}
void ClearReadyIds()
{
  gReadyIds.clear();
}

Actor CreateActorWithImageVisual(const Property::Map& map)
{
  VisualFactory factory = VisualFactory::Get();
  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  Visual::Base visual = factory.CreateVisual( map );
  DALI_TEST_CHECK( visual );
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  return actor;
}


Visual::Base CreateVisualWithPolicy( const char* url, Property::Index key, const Property::Value& value )
{
  VisualFactory factory = VisualFactory::Get();

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  url );
  propertyMap.Insert( ImageVisual::Property::DESIRED_WIDTH,   20 );
  propertyMap.Insert( ImageVisual::Property::DESIRED_HEIGHT,   30 );
  propertyMap.Insert( key , value );

  return factory.CreateVisual( propertyMap );
}

} // namespace

void TestVisualRender( ToolkitTestApplication& application,
                       DummyControl& actor,
                       Visual::Base& visual,
                       std::size_t expectedSamplers = 0,
                       ImageDimensions imageDimensions = ImageDimensions(),
                       Integration::ResourcePointer resourcePtr = Integration::ResourcePointer())
{
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  if( resourcePtr )
  {
    // set the image size, for test case, this needs to be set before loading started
    application.GetPlatform().SetClosestImageSize(  Vector2(imageDimensions.GetWidth(), imageDimensions.GetHeight()) );
  }

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  application.GetScene().Add( actor );

  application.SendNotification(); // Send messages to update
  application.Render();           // process update and render
  application.SendNotification(); // process any signals to event

  if( resourcePtr )
  {
    DALI_TEST_EQUALS( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceSynchronouslyFunc ), true, TEST_LOCATION);
  }

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
}

static void TestMixColor( Visual::Base visual, Property::Index mixColorIndex, const Vector4& testColor )
{
  Property::Map map;
  visual.CreatePropertyMap(map);
  Property::Value* value = map.Find( mixColorIndex );
  DALI_TEST_CHECK( value );
  Vector3 mixColor1;
  DALI_TEST_CHECK( value->Get( mixColor1 ) );
  DALI_TEST_EQUALS( mixColor1, Vector3(testColor), 0.001, TEST_LOCATION );

  value = map.Find( Visual::Property::MIX_COLOR );
  DALI_TEST_CHECK( value );
  Vector4 mixColor2;
  DALI_TEST_CHECK( value->Get( mixColor2 ) );
  DALI_TEST_EQUALS( mixColor2, testColor, 0.001, TEST_LOCATION );

  value = map.Find( Visual::Property::OPACITY );
  DALI_TEST_CHECK( value );
  float opacity;
  DALI_TEST_CHECK( value->Get( opacity ) );
  DALI_TEST_EQUALS( opacity, testColor.a, 0.001, TEST_LOCATION );
}



int UtcDaliImageVisualPropertyMap(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with a Property::Map" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );
  factory.SetPreMultiplyOnLoad( true );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_LARGE_IMAGE_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  application.GetScene().Add( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  auto renderer = actor.GetRendererAt(0);
  auto preMultipliedIndex = renderer.GetPropertyIndex( "preMultipliedAlpha" );
  DALI_TEST_CHECK( preMultipliedIndex != Property::INVALID_INDEX );
  auto preMultipliedAlpha = renderer.GetProperty<float>( preMultipliedIndex );
  auto preMultipliedAlpha2 = renderer.GetProperty<bool>( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );
  DALI_TEST_EQUALS( preMultipliedAlpha, 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( preMultipliedAlpha2, true, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  application.GetScene().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}


int UtcDaliImageVisualNoPremultipliedAlpha01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual without pre-multiplied alpha" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );
  factory.SetPreMultiplyOnLoad( false );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_LARGE_IMAGE_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  application.GetScene().Add( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  auto renderer = actor.GetRendererAt(0);
  auto preMultipliedIndex = renderer.GetPropertyIndex( "preMultipliedAlpha" );
  DALI_TEST_CHECK( preMultipliedIndex != Property::INVALID_INDEX );
  auto preMultipliedAlpha = renderer.GetProperty<bool>( preMultipliedIndex );
  auto preMultipliedAlpha2 = renderer.GetProperty<bool>( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );

  DALI_TEST_EQUALS( preMultipliedAlpha, false, TEST_LOCATION );
  DALI_TEST_EQUALS( preMultipliedAlpha2, false, TEST_LOCATION );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  application.GetScene().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}


int UtcDaliImageVisualNoPremultipliedAlpha02(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with no alpha channel" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_IMAGE_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  application.GetScene().Add( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  auto renderer = actor.GetRendererAt(0);
  auto preMultipliedIndex = renderer.GetPropertyIndex( "preMultipliedAlpha" );
  DALI_TEST_CHECK( preMultipliedIndex != Property::INVALID_INDEX );
  auto preMultipliedAlpha = renderer.GetProperty<bool>( preMultipliedIndex );
  auto preMultipliedAlpha2 = renderer.GetProperty<bool>( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );

  DALI_TEST_EQUALS( preMultipliedAlpha, false, TEST_LOCATION );
  DALI_TEST_EQUALS( preMultipliedAlpha2, false, TEST_LOCATION );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  int srcFactorRgb    = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  int destFactorRgb   = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  int srcFactorAlpha  = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  int destFactorAlpha = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );
  DALI_TEST_CHECK( srcFactorRgb == BlendFactor::SRC_ALPHA );
  DALI_TEST_CHECK( destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA );
  DALI_TEST_CHECK( srcFactorAlpha == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA );

  textureTrace.Reset();

  // Make a new visual with the same image
  Visual::Base newVisual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( newVisual );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  DummyControl newActor = DummyControl::New();
  DummyControlImpl& newDummyImpl = static_cast< DummyControlImpl& >( newActor.GetImplementation() );
  newDummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, newVisual );

  newActor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( newActor.GetRendererCount(), 0u, TEST_LOCATION );

  application.GetScene().Add( newActor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( newActor.GetRendererCount(), 1u, TEST_LOCATION );
  auto newRenderer = newActor.GetRendererAt( 0 );
  preMultipliedIndex = newRenderer.GetPropertyIndex( "preMultipliedAlpha" );
  DALI_TEST_CHECK( preMultipliedIndex != Property::INVALID_INDEX );
  preMultipliedAlpha = newRenderer.GetProperty< bool >( preMultipliedIndex );
  preMultipliedAlpha2 = newRenderer.GetProperty< bool >( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );

  DALI_TEST_EQUALS( preMultipliedAlpha, false, TEST_LOCATION );
  DALI_TEST_EQUALS( preMultipliedAlpha2, false, TEST_LOCATION );

  srcFactorRgb    = newRenderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  destFactorRgb   = newRenderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  srcFactorAlpha  = newRenderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  destFactorAlpha = newRenderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );
  DALI_TEST_CHECK( srcFactorRgb == BlendFactor::SRC_ALPHA );
  DALI_TEST_CHECK( destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA );
  DALI_TEST_CHECK( srcFactorAlpha == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA );

  application.GetScene().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}


int UtcDaliImageVisualRemoteImageLoad(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request remote image visual with a Property::Map" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_REMOTE_IMAGE_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  application.GetScene().Add( actor );
  application.SendNotification();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  application.GetScene().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliImageVisualTextureReuse1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request remote image visual with a Property::Map; request a second visual with the same property map - should reuse texture" );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_LARGE_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::RELEASE_POLICY,  ImageVisual::ReleasePolicy::DETACHED );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  Actor actor = CreateActorWithImageVisual( propertyMap );
  application.GetScene().Add( actor );
  application.SendNotification();

  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( drawTrace.FindMethod("DrawArrays"), true, TEST_LOCATION );
  textureTrace.Reset();
  drawTrace.Reset();

  Actor actor2 = CreateActorWithImageVisual( propertyMap );
  application.GetScene().Add(actor2);

  application.SendNotification(); // Send messages to update
  application.Render();           // process update and render
  application.SendNotification(); // process any signals to event

  DALI_TEST_EQUALS( actor2.GetRendererCount(), 1u, TEST_LOCATION );

  tet_infoline("Test that 2 draw calls occur with no new texture gens/binds, i.e. both\n"
               "draw calls use the same texture as the previous draw call\n" );

  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawArrays"), 2, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.CountMethod("BindTexture"), 0, TEST_LOCATION );

  tet_infoline("Test that removing 1 actor doesn't delete the texture\n");

  application.GetScene().Remove( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );

  tet_infoline("Test that removing last actor does delete the texture\n");

  application.GetScene().Remove( actor2 ); // Detaches remaining ImageVisual
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor2.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 1, TEST_LOCATION );

  END_TEST;
}


int UtcDaliImageVisualTextureReuse2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request remote image visual with a Property::Map; request a second visual with the same url but different property map - should create new texture" );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_REMOTE_IMAGE_FILE_NAME );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  Actor actor = CreateActorWithImageVisual( propertyMap );
  application.GetScene().Add( actor );
  application.SendNotification();

  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( drawTrace.FindMethod("DrawArrays"), true, TEST_LOCATION );
  textureTrace.Reset();
  drawTrace.Reset();

  propertyMap.Insert( ImageVisual::Property::SAMPLING_MODE, Dali::SamplingMode::NEAREST );
  propertyMap.Insert( ImageVisual::Property::DESIRED_WIDTH, 100 );
  propertyMap.Insert( ImageVisual::Property::DESIRED_HEIGHT, 100 );
  Actor actor2 = CreateActorWithImageVisual( propertyMap );
  application.GetScene().Add(actor2);

  application.SendNotification();

  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor2.GetRendererCount(), 1u, TEST_LOCATION );

  tet_infoline("Test that 2 draw calls occur with 1 new texture gen/bind, i.e. both "
               "renderers are using different textures\n" );

  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( drawTrace.CountMethod("DrawArrays"), 2, TEST_LOCATION );
  TraceCallStack::NamedParams tex1;
  tex1["texture"] = "1";
  TraceCallStack::NamedParams tex2;
  tex1["texture"] = "2";
  DALI_TEST_EQUALS( textureTrace.FindMethodAndParams("BindTexture", tex1), true, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethodAndParams("BindTexture", tex2), true, TEST_LOCATION );

  tet_infoline("Test that removing 1 actor deletes it's texture\n");

  application.GetScene().Remove( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 1, TEST_LOCATION );

  tet_infoline("Test that removing last actor deletes it's texture\n");

  application.GetScene().Remove( actor2 );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor2.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 2, TEST_LOCATION );

  END_TEST;
}


int UtcDaliImageVisualCustomWrapModePixelArea(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with a Property::Map, test custom wrap mode and pixel area with atlasing" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test wrap mode with atlasing. Image with a size smaller than 512*512 will be uploaded as a part of the atlas.
  const int width=34;
  const int height=34;
  const Vector4 pixelArea(-0.5f, -0.5f, 2.f, 2.f);

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_SMALL_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::DESIRED_WIDTH, width );
  propertyMap.Insert( ImageVisual::Property::DESIRED_HEIGHT, height );
  propertyMap.Insert( ImageVisual::Property::SYNCHRONOUS_LOADING, true );
  propertyMap.Insert( ImageVisual::Property::PIXEL_AREA, pixelArea );
  propertyMap.Insert( ImageVisual::Property::WRAP_MODE_U, WrapMode::MIRRORED_REPEAT );
  propertyMap.Insert( ImageVisual::Property::WRAP_MODE_V, WrapMode::REPEAT );
  propertyMap.Insert( ImageVisual::Property::ATLASING, true );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Enable( true );

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetProperty( Actor::Property::SIZE, Vector2(2000, 2000) );
  actor.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);
  application.GetScene().Add( actor );

  // loading started
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  // WITH atlasing, the wrapping is handled manually in shader, so the following gl function should not be called
  std::stringstream out;
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_WRAP_S << ", " << GL_MIRRORED_REPEAT;
  DALI_TEST_CHECK( !texParameterTrace.FindMethodAndParams("TexParameteri", out.str()) );
  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_WRAP_T << ", " << GL_REPEAT;
  DALI_TEST_CHECK( !texParameterTrace.FindMethodAndParams("TexParameteri", out.str()) );

  // test the uniforms which used to handle the wrap mode
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  Property::Value pixelAreaValue = renderer.GetProperty( renderer.GetPropertyIndex( "pixelArea" ) );
  DALI_TEST_EQUALS( pixelAreaValue.Get<Vector4>(), pixelArea, TEST_LOCATION );
  Vector4 pixelAreaUniform;
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "pixelArea", pixelAreaUniform ) );
  DALI_TEST_EQUALS( pixelArea, pixelAreaUniform, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  Property::Value wrapModeValue = renderer.GetProperty( renderer.GetPropertyIndex( "wrapMode" ) );
  Vector2 wrapMode( WrapMode::MIRRORED_REPEAT-1, WrapMode::REPEAT-1 );
  DALI_TEST_EQUALS( wrapModeValue.Get<Vector2>(), wrapMode, TEST_LOCATION );
  Vector2 wrapModeUniform;
  DALI_TEST_CHECK( gl.GetUniformValue<Vector2>( "wrapMode", wrapModeUniform ) );
  DALI_TEST_EQUALS( wrapMode, wrapModeUniform, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  actor.Unparent( );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliImageVisualCustomWrapModeNoAtlas(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with a Property::Map, test custom wrap mode and pixel area without atlasing" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test wrap mode without atlasing. Image with a size bigger than 512*512 will NOT be uploaded as a part of the atlas.
  const int width=600;
  const int height=600;
  const Vector4 pixelArea(-0.5f, -0.5f, 2.f, 2.f);

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_LARGE_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::DESIRED_WIDTH, width );
  propertyMap.Insert( ImageVisual::Property::DESIRED_HEIGHT, height );
  propertyMap.Insert( ImageVisual::Property::SYNCHRONOUS_LOADING, true );
  propertyMap.Insert( ImageVisual::Property::PIXEL_AREA, pixelArea );
  propertyMap.Insert( ImageVisual::Property::WRAP_MODE_U, WrapMode::MIRRORED_REPEAT );
  propertyMap.Insert( ImageVisual::Property::WRAP_MODE_V, WrapMode::REPEAT );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Enable( true );

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetProperty( Actor::Property::SIZE, Vector2(2000, 2000) );
  actor.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);
  application.GetScene().Add( actor );

  // loading started
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  // WITHOUT atlasing, the wrapping is handled by setting gl texture parameters
  std::stringstream out;
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_WRAP_S << ", " << GL_MIRRORED_REPEAT;
  DALI_TEST_CHECK( texParameterTrace.FindMethodAndParams("TexParameteri", out.str()) );
  out.str("");
  out << GL_TEXTURE_2D << ", " << GL_TEXTURE_WRAP_T << ", " << GL_REPEAT;
  DALI_TEST_CHECK( texParameterTrace.FindMethodAndParams("TexParameteri", out.str()) );

  // test the uniforms which used to handle the wrap mode
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  Property::Value pixelAreaValue = renderer.GetProperty( renderer.GetPropertyIndex( "pixelArea" ) );
  DALI_TEST_EQUALS( pixelAreaValue.Get<Vector4>(), pixelArea, TEST_LOCATION );
  Vector4 pixelAreaUniform;
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "pixelArea", pixelAreaUniform ) );
  DALI_TEST_EQUALS( pixelArea, pixelAreaUniform, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  Property::Index wrapModeIndex = renderer.GetPropertyIndex( "wrapMode" );
  DALI_TEST_CHECK(wrapModeIndex == Property::INVALID_INDEX);

  actor.Unparent();
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliImageVisualAnimateMixColor(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Animate mix color" );

  application.GetPlatform().SetClosestImageSize( Vector2(100, 100) );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::IMAGE);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME );
  propertyMap.Insert("mixColor", Color::BLUE);
  propertyMap.Insert(ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  Visual::Base visual = factory.CreateVisual( propertyMap );

  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2(2000, 2000) );
  actor.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);
  actor.SetProperty( Actor::Property::COLOR,Color::BLACK);
  application.GetScene().Add(actor);

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index index = renderer.GetPropertyIndex( Visual::Property::MIX_COLOR );
  Property::Value blendModeValue = renderer.GetProperty( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( blendModeValue.Get<int>(), (int)BlendMode::AUTO, TEST_LOCATION );

  tet_infoline("Test that the renderer has the mixColor property");
  DALI_TEST_CHECK( index != Property::INVALID_INDEX );

  const Vector4 TARGET_MIX_COLOR( 1.0f, 0.0f, 0.0f, 0.5f );

  Property::Map map;
  map["target"] = "testVisual";
  map["property"] = "mixColor";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = TARGET_MIX_COLOR;
  map["animator"] = Property::Map()
    .Add("alphaFunction", "LINEAR")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.0f)
         .Add("duration", 4.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  Animation animation = dummyImpl.CreateTransition( transition );

  animation.AnimateTo( Property(actor, Actor::Property::COLOR), Color::WHITE );
  animation.Play();

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace( true );
  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;

  application.SendNotification();
  application.Render(0); // Ensure animation starts
  application.Render(2000u); // Halfway point
  Vector3 testColor( 1.0f, 0.0f, 0.5f );

  // uColor.a should be actor's alpha * mixColor.a.
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>( "uColor", Vector4( 0.5f, 0.5f, 0.5f, 0.75f ) ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector3>( "mixColor", testColor ), true, TEST_LOCATION );

  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  glEnableStack.Reset();

  application.Render(2000u); // Halfway point between blue and white

  DALI_TEST_EQUALS( actor.GetCurrentProperty< Vector4 >( Actor::Property::COLOR ), Color::WHITE, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>( "uColor", Vector4( 1.0f, 1.0f, 1.0f, 0.5f ) ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector3>( "mixColor", Vector3( TARGET_MIX_COLOR ) ), true, TEST_LOCATION );

  // GL_BLEND should not be changed: Keep enabled
  DALI_TEST_CHECK( !glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );
  DALI_TEST_CHECK( !glEnableStack.FindMethodAndParams( "Disable", blendStr.str().c_str() ) );

  TestMixColor( visual, Visual::Property::MIX_COLOR, TARGET_MIX_COLOR );

  END_TEST;
}

int UtcDaliImageVisualAnimateOpacity(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Animate image visual opacity" );

  application.GetPlatform().SetClosestImageSize( Vector2(100, 100) );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::IMAGE);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME );
  propertyMap.Insert("opacity", 0.5f);
  propertyMap.Insert(ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  Visual::Base visual = factory.CreateVisual( propertyMap );

  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2(2000, 2000) );
  actor.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);
  actor.SetProperty( Actor::Property::COLOR,Color::BLACK);
  application.GetScene().Add(actor);

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace( true );
  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  {
    tet_infoline( "Test that the opacity can be increased to full via animation, and that the blend mode is set appropriately at the start and end of the animation." );

    Property::Map map;
    map["target"] = "testVisual";
    map["property"] = "opacity";
    map["targetValue"] = 1.0f;
    map["animator"] = Property::Map()
      .Add("alphaFunction", "LINEAR")
      .Add("timePeriod", Property::Map()
           .Add("delay", 0.0f)
           .Add("duration", 4.0f));

    Dali::Toolkit::TransitionData transition = TransitionData::New( map );
    Animation animation = dummyImpl.CreateTransition( transition );
    animation.Play();

    glEnableStack.Reset();

    application.SendNotification();
    application.Render(0);     // Ensure animation starts
    application.Render(2000u); // Halfway point through animation
    application.SendNotification(); // Handle any signals

    Vector4 color;
    DALI_TEST_CHECK( application.GetGlAbstraction().GetUniformValue< Vector4 >( "uColor", color ) );
    DALI_TEST_EQUALS( color.a, 0.75f, TEST_LOCATION );

    application.Render(2001u); // end
    application.SendNotification(); // ensure animation finished signal is sent

    DALI_TEST_CHECK( application.GetGlAbstraction().GetUniformValue< Vector4 >( "uColor", color ) );
    DALI_TEST_EQUALS( color.a, 1.0f, TEST_LOCATION );

    DALI_TEST_CHECK( !glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );
    DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Disable", blendStr.str().c_str() ) );
  }


  {
    tet_infoline( "Test that the opacity can be reduced via animation, and that the blend mode is set appropriately at the start and end of the animation." );

    Property::Map map;
    map["target"] = "testVisual";
    map["property"] = Visual::Property::OPACITY;
    map["targetValue"] = 0.1f;
    map["animator"] = Property::Map()
      .Add("alphaFunction", "LINEAR")
      .Add("timePeriod", Property::Map()
           .Add("delay", 0.0f)
           .Add("duration", 4.0f));

    Dali::Toolkit::TransitionData transition = TransitionData::New( map );
    Animation animation = dummyImpl.CreateTransition( transition );
    animation.Play();

    glEnableStack.Reset();

    application.SendNotification();
    application.Render(0);     // Ensure animation starts
    application.Render(2000u); // Halfway point
    application.SendNotification();

    Vector4 color;
    DALI_TEST_CHECK( application.GetGlAbstraction().GetUniformValue< Vector4 >( "uColor", color ) );
    DALI_TEST_EQUALS( color.a, 0.55f, TEST_LOCATION );

    DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

    glEnableStack.Reset();

    application.Render(2016u); // end
    application.SendNotification();

    DALI_TEST_CHECK( application.GetGlAbstraction().GetUniformValue< Vector4 >( "uColor", color ) );
    DALI_TEST_EQUALS( color.a, 0.1f, TEST_LOCATION );

    // GL_BLEND should not be changed: Keep enabled
    DALI_TEST_CHECK( !glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );
    DALI_TEST_CHECK( !glEnableStack.FindMethodAndParams( "Disable", blendStr.str().c_str() ) );
  }

  END_TEST;
}



int UtcDaliImageVisualAnimateOpacity02(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Animate image visual opacity" );

  application.GetPlatform().SetClosestImageSize( Vector2(100, 100) );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::IMAGE);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME );
  propertyMap.Insert("opacity", 0.5f);
  propertyMap.Insert(ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  Visual::Base visual = factory.CreateVisual( propertyMap );

  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2(2000, 2000) );
  actor.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);
  actor.SetProperty( Actor::Property::COLOR,Color::BLACK);

  tet_infoline( "Test that the opacity doesn't animate when actor not staged" );

  Property::Array array;

  Property::Map map;
  map["target"] = "testVisual";
  map["property"] = "opacity";
  map["initialValue"] = 0.0f;
  map["targetValue"] = 1.0f;
  map["animator"] = Property::Map()
    .Add("alphaFunction", "LINEAR")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.0f)
         .Add("duration", 4.0f));

  Property::Map map2;
  map2["target"] = "testVisual";
  map2["property"] = "size";
  map2["targetValue"] = Vector2(1.0f, 1.0f);

  array.Add( map ).Add(map2);

  Dali::Toolkit::TransitionData transition = TransitionData::New( array );
  Animation animation = dummyImpl.CreateTransition( transition );

  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render(0);     // Ensure animation starts

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0);
  Property::Value blendModeValue = renderer.GetProperty( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( blendModeValue.Get<int>(), (int)BlendMode::AUTO, TEST_LOCATION );

  animation = dummyImpl.CreateTransition( transition );
  animation.Play();

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace( true );
  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;

  application.SendNotification();
  application.Render(0);     // Ensure animation starts
  application.Render(2000u); // Halfway point through animation
  application.SendNotification(); // Handle any signals

  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  Vector4 color;
  DALI_TEST_CHECK( application.GetGlAbstraction().GetUniformValue< Vector4 >( "uColor", color ) );
  DALI_TEST_EQUALS( color.a, 0.5f, TEST_LOCATION );

  glEnableStack.Reset();

  application.Render(2001u); // end
  application.SendNotification(); // ensure animation finished signal is sent

  DALI_TEST_CHECK( application.GetGlAbstraction().GetUniformValue< Vector4 >( "uColor", color ) );
  DALI_TEST_EQUALS( color.a, 1.0f, TEST_LOCATION );

  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Disable", blendStr.str().c_str() ) );

  END_TEST;
}



int UtcDaliImageVisualAnimatePixelArea(void)
{
  ToolkitTestApplication application;
  tet_infoline( "ImageVisual animate pixel area" );

  application.GetPlatform().SetClosestImageSize( Vector2(100, 100) );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::IMAGE);
  propertyMap.Insert(ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME );
  propertyMap.Insert("mixColor", Color::BLUE);
  propertyMap.Insert(ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  Visual::Base visual = factory.CreateVisual( propertyMap );

  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2(2000, 2000) );
  actor.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);
  actor.SetProperty( Actor::Property::COLOR,Color::BLACK);
  application.GetScene().Add(actor);

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index index = renderer.GetPropertyIndex( Visual::Property::MIX_COLOR );

  tet_infoline("Test that the renderer has the mixColor property");
  DALI_TEST_CHECK( index != Property::INVALID_INDEX );

  // TransitionData only takes string keys
  Property::Map map;
  map["target"] = "testVisual";
  map["property"] = "pixelArea";
  map["initialValue"] = Vector4( 0,0,0,1 );
  map["targetValue"] = Vector4( 0,0,1,1 ); // Animate width from zero to full
  map["animator"] = Property::Map()
    .Add("alphaFunction", "LINEAR")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.0f)
         .Add("duration", 4.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  Animation animation = dummyImpl.CreateTransition( transition );
  animation.AnimateTo( Property(actor, Actor::Property::COLOR), Color::WHITE );
  animation.Play();

  application.SendNotification();
  application.Render(0);     // Ensure animation starts
  application.Render(2000u); // Halfway point

  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>("pixelArea", Vector4(0.0f, 0.0f, 0.5f, 1.0f )), true, TEST_LOCATION );

  application.Render(2000u); // End of animation

  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>("pixelArea", Vector4( 0.0f, 0.0f, 1.0f, 1.0f )), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualTextureCancelRemoteLoad(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request remote image visual, then destroy visual to cancel load" );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_REMOTE_IMAGE_FILE_NAME );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  Actor actor = CreateActorWithImageVisual( propertyMap );
  application.GetScene().Add( actor );
  application.SendNotification();

  application.GetScene().Remove( actor );
  application.SendNotification();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), false, TEST_LOCATION );
  DALI_TEST_EQUALS( drawTrace.FindMethod("DrawArrays"), false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualTextureCancelAsyncLoad(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Load image asynchronously, cancel loading, then load again" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_IMAGE_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable( true );
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable( true );

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( Control::Property::BACKGROUND, visual );

  application.GetScene().Add( actor );

  // Cancel loading
  application.GetScene().Remove( actor );

  application.GetScene().Add( actor );

  // Create another visual with the same image
  visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  dummyImpl.RegisterVisual( Control::Property::BACKGROUND, visual );

  application.SendNotification();
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( drawTrace.FindMethod("DrawArrays"), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualSetInvalidAsyncImage(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with invalid images - should draw broken.png" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE, Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_INVALID_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  application.GetScene().Add( actor );

  application.SendNotification();
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  application.GetScene().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliImageVisualSetInvalidSyncImage(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with invalid images - should draw broken.png" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE, Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_INVALID_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::SYNCHRONOUS_LOADING, true );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  application.GetScene().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  application.GetScene().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliImageVisualSetInvalidRemoteImage(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with invalid images - should draw broken.png" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  // Local invalid file, asynchronous loading
  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE, Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_REMOTE_INVALID_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  application.GetScene().Add( actor );

  application.SendNotification();
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  application.GetScene().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliImageVisualAlphaMask(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with a Property::Map containing an Alpha mask" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_LARGE_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::ALPHA_MASK_URL, TEST_MASK_IMAGE_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  Property::Map testMap;
  visual.CreatePropertyMap(testMap);
  DALI_TEST_EQUALS(*testMap.Find(ImageVisual::Property::ALPHA_MASK_URL),Property::Value(TEST_MASK_IMAGE_FILE_NAME), TEST_LOCATION );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.IsResourceReady(), false, TEST_LOCATION );

  application.GetScene().Add( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 3 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.IsResourceReady(), true, TEST_LOCATION );

  dummyImpl.UnregisterVisual(  Control::CONTROL_PROPERTY_END_INDEX + 1 );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualSynchronousLoadAlphaMask(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with a Property::Map containing an Alpha mask with synchronous loading" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_LARGE_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::ALPHA_MASK_URL, TEST_MASK_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::SYNCHRONOUS_LOADING, true );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  Property::Map testMap;
  visual.CreatePropertyMap(testMap);
  DALI_TEST_EQUALS(*testMap.Find(ImageVisual::Property::ALPHA_MASK_URL),Property::Value(TEST_MASK_IMAGE_FILE_NAME), TEST_LOCATION );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.IsResourceReady(), false, TEST_LOCATION );

  application.GetScene().Add( actor );

  // Do not wait for any EventThreadTrigger in synchronous alpha mask.

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.IsResourceReady(), true, TEST_LOCATION );

  dummyImpl.UnregisterVisual(  Control::CONTROL_PROPERTY_END_INDEX + 1 );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualRemoteAlphaMask(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with a Property::Map containing an Alpha mask" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  const std::string MASK_IMAGE = TEST_REMOTE_IMAGE_FILE_NAME;

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::ALPHA_MASK_URL, MASK_IMAGE );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  Property::Map testMap;
  visual.CreatePropertyMap(testMap);

  DALI_TEST_EQUALS(*testMap.Find(ImageVisual::Property::ALPHA_MASK_URL),Property::Value(MASK_IMAGE), TEST_LOCATION );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  DALI_TEST_EQUALS( actor.IsResourceReady(), false, TEST_LOCATION );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  application.GetScene().Add( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 3 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.IsResourceReady(), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualAlphaMaskCrop(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with an Alpha mask and scale/cropping" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Toolkit::Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_LARGE_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::ALPHA_MASK_URL, TEST_MASK_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::MASK_CONTENT_SCALE, 1.6f );
  propertyMap.Insert( ImageVisual::Property::CROP_TO_MASK, true );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  Property::Map testMap;
  visual.CreatePropertyMap(testMap);
  DALI_TEST_EQUALS( *testMap.Find(ImageVisual::Property::ALPHA_MASK_URL),Property::Value(TEST_MASK_IMAGE_FILE_NAME), TEST_LOCATION );
  DALI_TEST_EQUALS( *testMap.Find(ImageVisual::Property::MASK_CONTENT_SCALE), Property::Value(1.6f), TEST_LOCATION );
  DALI_TEST_EQUALS( *testMap.Find(ImageVisual::Property::CROP_TO_MASK),Property::Value(true), TEST_LOCATION );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.IsResourceReady(), false, TEST_LOCATION );

  application.GetScene().Add( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 3 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  Vector2 size;
  visual.GetNaturalSize(size);

  DALI_TEST_EQUALS( size, Vector2( 100.0f, 100.0f ), 0.001f, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( actor.IsResourceReady(), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualReleasePolicy01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualReleasePolicy01 Detached Policy, disabling visual with this policy deletes texture" );

  Visual::Base imageVisual = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::DETACHED );
  DALI_TEST_CHECK( imageVisual );

  // Set up debug trace
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Register visual with control and ensure it has the only handle" );
  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual );
  imageVisual.Reset();

  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );

  application.GetScene().Add( actor );

  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  // Test renderer and texture created
  tet_infoline( "Confirm texture created" );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );

  tet_infoline( "Disable visual causing the texture to be deleted" );
  dummyImpl.EnableVisual( DummyControl::Property::TEST_VISUAL, false );

  application.SendNotification();
  application.Render(0);
  // Test renderer and textures removed.
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualReleasePolicy02(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualReleasePolicy02 Destroyed Policy, Texture should be deleted when visual destroyed" );

  Visual::Base imageVisual = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::DESTROYED );
  DALI_TEST_CHECK( imageVisual );

  // Setup debug trace
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Register visual with control and ensure it has the only handle" );
  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual );
  imageVisual.Reset(); // reduce ref count so only the control keeps the visual alive.

  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );

  application.GetScene().Add( actor );

  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  // Test renderer and texture created
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );


  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  tet_infoline( "Destroy visual by UnRegistering visual with control, check renderer is destroyed" );
  dummyImpl.UnregisterVisual( DummyControl::Property::TEST_VISUAL );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  application.SendNotification();
  application.Render();

  // Test texture removed after visual destroyed.
  tet_infoline( "Ensure texture is deleted after visual destroyed" );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualReleasePolicy03(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualReleasePolicy03 Never Policy, texture should not be deleted after visual destroyed" );

  Visual::Base imageVisual = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::NEVER );
  DALI_TEST_CHECK( imageVisual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Register visual with control and ensure it has the only handle" );
  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual );
  imageVisual.Reset(); // reduce ref count so only the control keeps the visual alive.

  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );

  application.GetScene().Add( actor );

  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  // Test renderer and texture created
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );

  tet_infoline( "Destroy visual by UnRegistering visual with control, check renderer is destroyed" );
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  dummyImpl.UnregisterVisual( DummyControl::Property::TEST_VISUAL );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure texture is not deleted as policy is set to NEVER" );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualReleasePolicy04(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualReleasePolicy04 Two visuals with different policies sharing a texture" );

  tet_infoline( "Create first visual with Never release policy" );
  Visual::Base imageVisualNever = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::NEVER );

  tet_infoline( "Create second visual with Destroyed release policy");
    Visual::Base imageVisualDestroyed = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::DESTROYED );

  // Set up trace debug
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Register visuals with control and ensure it has the only handles" );
  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisualNever );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL2, imageVisualDestroyed );
  imageVisualNever.Reset(); // reduce ref count so only the control keeps the visual alive.
  imageVisualDestroyed.Reset(); // reduce ref count so only the control keeps the visual alive.

  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );

  // Test initially zero renderers
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );

  application.GetScene().Add( actor );

  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  tet_infoline( "Ensure a texture is created, shared amongst both visuals.  Each visual has its own renderer" );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 2u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );

  // Test renderer removed when visual destroyed
  DALI_TEST_CHECK( actor.GetRendererCount() == 2u );
  dummyImpl.UnregisterVisual( DummyControl::Property::TEST_VISUAL2 );  // TEST_VISUAL2 no longer requires the texture as release policy DESTROYED
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  application.SendNotification();
  application.Render();

  // Test texture was not deleted as TEST_VISUAL release policy is NEVER so it is still required.
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );

  dummyImpl.UnregisterVisual( DummyControl::Property::TEST_VISUAL );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure a texture is not deleted as second visual used the NEVER release policy" );
  // Test texture was not deleted as TEST_VISUAL release policy is NEVER so it is still required.
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualReleasePolicy05(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualReleasePolicy05 Testing settung by string currents correct enum" );

  VisualFactory factory = VisualFactory::Get();

  Property::Map propertyMapNeverReleasePolicy;
  propertyMapNeverReleasePolicy.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMapNeverReleasePolicy.Insert( ImageVisual::Property::URL,  TEST_IMAGE_FILE_NAME );
  propertyMapNeverReleasePolicy.Insert( ImageVisual::Property::DESIRED_WIDTH,   20 );
  propertyMapNeverReleasePolicy.Insert( ImageVisual::Property::DESIRED_HEIGHT,   30 );
  propertyMapNeverReleasePolicy.Insert( "releasePolicy" , "never" );

  Visual::Base imageVisualNever = factory.CreateVisual( propertyMapNeverReleasePolicy );

  Property::Map resultMap;
  imageVisualNever.CreatePropertyMap( resultMap );
  DALI_TEST_CHECK( ! resultMap.Empty() );

  DALI_TEST_EQUALS( ( resultMap.Find( ImageVisual::Property::RELEASE_POLICY ) )->Get<int>(), (int)ImageVisual::ReleasePolicy::NEVER, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualReleasePolicy06(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualReleasePolicy06 Never Policy, texture should not be affected by Disabling and Enabling visual" );

  Visual::Base imageVisual= CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::NEVER );
  DALI_TEST_CHECK( imageVisual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Register visual with control and ensure it has the only handle" );
  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual );
  imageVisual.Reset(); // reduce ref count so only the control keeps the visual alive.

  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );

  application.GetScene().Add( actor );

  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  // Test renderer and texture created
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );
  textureTrace.Reset();

  tet_infoline( "Disable Visual and check texture not affected" );
  dummyImpl.EnableVisual( DummyControl::Property::TEST_VISUAL, false );
  application.SendNotification();
  application.Render(0);
  tet_infoline( "Check renderer is destroyed when visual off stage" );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );
  textureTrace.Reset();

  tet_infoline( "Re-enable Visual and check texture not affected" );
  dummyImpl.EnableVisual( DummyControl::Property::TEST_VISUAL, true );
  application.SendNotification();
  application.Render(0);
  tet_infoline( "Check texture not affected and renderer is destroyed when visual off stage" );
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualReleasePolicy07(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualReleasePolicy07 Two visuals with different policies sharing a texture DETACHED and DESTROYED" );

  tet_infoline( "Create first visual with DESTROYED release policy" );
  Visual::Base imageVisualDestroyed = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::DESTROYED );


  tet_infoline( "Create second visual with DETACHED release policy");
  Visual::Base imageVisualDetached = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::DETACHED );

  // Set up trace debug
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Register visuals with control and ensure it has the only handles" );
  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisualDestroyed );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL2, imageVisualDetached );
  imageVisualDestroyed.Reset(); // reduce ref count so only the control keeps the visual alive.
  imageVisualDetached.Reset(); // reduce ref count so only the control keeps the visual alive.

  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );

  // Test initially zero renderers
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );

  application.GetScene().Add( actor );

  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  tet_infoline( "Ensure a texture is created, shared amongst both visuals.  Each visual has its own renderer" );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 2u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );

  // Test renderer removed when visual destroyed
  DALI_TEST_CHECK( actor.GetRendererCount() == 2u );
  dummyImpl.EnableVisual( DummyControl::Property::TEST_VISUAL2, false );  // TEST_VISUAL2 no longer requires the texture as release policy DETACHED
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  application.SendNotification();
  application.Render();

  // Test texture was not deleted as TEST_VISUAL release policy is DESTROYED and is still required.
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );

  dummyImpl.EnableVisual( DummyControl::Property::TEST_VISUAL, false );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure a texture is not deleted as second visual used the DESTROYED release policy" );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualReleasePolicy08(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualReleasePolicy08 Ensure TextureSet is same after detach/attach on stage when texture used the DESTROYED release policy" );

  tet_infoline( "Create first visual with DESTROYED release policy" );
  Visual::Base imageVisualDestroyed = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::DESTROYED );

  // Set up trace debug
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Register visuals with control and ensure it has the only handles" );
  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisualDestroyed );
  imageVisualDestroyed.Reset(); // reduce ref count so only the control keeps the visual alive.

  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );

  // Test initially zero renderers
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );
  textureTrace.Reset();

  application.GetScene().Add( actor );

  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(0);
  tet_infoline( "Ensure a texture is created" );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );
  textureTrace.Reset();

  // Ensure TextureSet is same after detach/attach on stage when texture used the DESTROYED release policy
  // 1. Get TextureSet
  TextureSet textureSetBefore = actor.GetRendererAt( 0u ).GetTextures();

  // 2.Remove actor from stage. In this case, renderer also is deleted.
  tet_infoline( "Remove actor from stage" );
  application.GetScene().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure a texture is not deleted as visual used the DESTROYED release policy" );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );
  textureTrace.Reset();

  // 3.Add actor in stage. In this case, renderer is created.
  tet_infoline( "Add actor in stage" );
  application.GetScene().Add( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  application.SendNotification();
  application.Render();
  tet_infoline( "Ensure a texture is not created again" );
  DALI_TEST_EQUALS( textureTrace.CountMethod("GenTextures"), 0, TEST_LOCATION );
  textureTrace.Reset();

  // 4.Compare Texture with before and after. textureSet need to be same because release policy is the DESTROYED.
  tet_infoline( "Ensure a textureSet is not deleted because it is used the DESTROYED release policy" );
  TextureSet textureSetAfter = actor.GetRendererAt( 0u ).GetTextures();
  DALI_TEST_CHECK( textureSetBefore == textureSetAfter );
  textureSetBefore.Reset();
  textureSetAfter.Reset();

  dummyImpl.UnregisterVisual( DummyControl::Property::TEST_VISUAL );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualLoadPolicy01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualLoadPolicy01 Load a visual image before attaching to stage" );

  // Set up trace debug
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Create visual with IMMEDIATE load policy" );
  VisualFactory factory = VisualFactory::Get();

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::DESIRED_WIDTH,   20 );
  propertyMap.Insert( ImageVisual::Property::DESIRED_HEIGHT,   30 );
  propertyMap.Insert( "loadPolicy" , ImageVisual::LoadPolicy::IMMEDIATE );

  Visual::Base imageVisual = factory.CreateVisual( propertyMap );

  Property::Map resultMap;
  imageVisual.CreatePropertyMap( resultMap );
  DALI_TEST_CHECK( ! resultMap.Empty() );
  DALI_TEST_EQUALS( ( resultMap.Find( ImageVisual::Property::LOAD_POLICY ) )->Get<int>(), (int)ImageVisual::LoadPolicy::IMMEDIATE, TEST_LOCATION );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  // Ensure texture has been uploaded
  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure texture loading starts after visual created" );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );
  textureTrace.Reset();

  tet_infoline( "Register visuals with control and ensure it has the only handles" );
  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual );
  imageVisual.Reset(); // reduce ref count so only the control keeps the visual alive.

  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
  application.GetScene().Add( actor );
  tet_infoline( "Ensure nothing triggers another load as texure already loaded" );
  const unsigned int TIME_OUT_3_SECONDS = 3;
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1, TIME_OUT_3_SECONDS ), false, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );

  // Ensure texture is deleted when no longer needed (ref count was correct )
  dummyImpl.UnregisterVisual( DummyControl::Property::TEST_VISUAL );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualLoadPolicy02(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualLoadPolicy01 Load a visual image only after attached to stage" );

  // Set up trace debug
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Create visual with IMMEDIATE load policy" );
  Visual::Base imageVisual = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::LOAD_POLICY, ImageVisual::LoadPolicy::ATTACHED );

  const unsigned int TIME_OUT_3_SECONDS = 3;
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1, TIME_OUT_3_SECONDS ), false, TEST_LOCATION );

  // Act on meeage queue even although nothing expected to load
  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure texture is not generated yet" );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );
  textureTrace.Reset();

  tet_infoline( "Register visuals with control and ensure it has the only handles" );
  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual );
  imageVisual.Reset(); // reduce ref count so only the control keeps the visual alive.

  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
  application.GetScene().Add( actor );
  tet_infoline( "Allow image time to load" );
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  tet_infoline( "Ensure texture generated and renderer created" );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );

  // Ensure texture is delete when no longer needed
  dummyImpl.UnregisterVisual( DummyControl::Property::TEST_VISUAL );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualLoadPolicy03(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualLoadPolicy03 Load a visual image and receive ResourceReady Signal when loaded" );

  const bool VISUAL_NOT_ENABLED( false ); // Instead of just passing 'false' into an API.

  // Set up trace debug
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Create a control and connect to resource ready signal without adding to stage" );
  DummyControl actor = DummyControl::New(true);
  actor.ResourceReadySignal().Connect( &ResourceReadySignal);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );

  tet_infoline( "Create visual with IMMEDIATE load policy" );
  Visual::Base imageVisual = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::LOAD_POLICY, ImageVisual::LoadPolicy::IMMEDIATE );

  tet_infoline( "Registering visual allows control to get a signal once loaded even if visual not enabled( not staged )" );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual, VISUAL_NOT_ENABLED );
  imageVisual.Reset(); // reduce ref count so only the control keeps the visual alive.

  tet_infoline( "Allow image time to load resource" );
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
  application.SendNotification();
  application.Render();

  // Ensure texture has been uploaded
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualLoadPolicy04(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualLoadPolicy04 First part  Load a visual image before attaching to stage");
  tet_infoline( "Second part, Reuse the same image in aonther control and check resource ready signal fired" );

  const bool VISUAL_NOT_ENABLED( false ); // Instead of just passing false into an API.

  // Set up trace debug
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Create a control and connect to resource ready signal" );
  DummyControl actor = DummyControl::New(true);
  actor.ResourceReadySignal().Connect( &ResourceReadySignal);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );

  tet_infoline( "Create visual with IMMEDIATE load policy" );
  Visual::Base imageVisual = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::LOAD_POLICY, ImageVisual::LoadPolicy::IMMEDIATE );

  tet_infoline( "Registering visual allows control to get a signal once loaded even if visual not enabled( staged )" );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual, VISUAL_NOT_ENABLED );
  imageVisual.Reset(); // reduce ref count so only the control keeps the visual alive.

  tet_infoline( "Allow image time to load" );
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
  application.SendNotification();
  application.Render();

  tet_infoline( "Testing texture is loaded and resource ready signal fired" );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  tet_infoline( "Original control correctly signalled, now testing for signal with new Control reusing the image" );

  gResourceReadySignalFired = false; // Reset signal check ready for testing next Control
  Visual::Base imageVisual2 = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::LOAD_POLICY, ImageVisual::LoadPolicy::IMMEDIATE );
  DummyControl actor2 = DummyControl::New(true);
  Impl::DummyControl& dummyImpl2 = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  actor2.ResourceReadySignal().Connect( &ResourceReadySignal);

  tet_infoline( "Registering visual this should trigger the loading signal as is already image loaded for previous control" );
  dummyImpl2.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual2 );
  imageVisual2.Reset(); // reduce ref count so only the control keeps the visual alive.
  actor2.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 0 ), true, TEST_LOCATION ); // Not expecting any further loading as texture is being reused.
  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualLoadPolicy05(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualLoadPolicy05 LoadPolicy::ATTACHED (default) First part  Load a visual image before attaching to stage");
  tet_infoline( "Second part, Reuse the same image in aonther control and check resource ready signal fired" );
  // Set up trace debug
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  tet_infoline( "Create a control and connect to resource ready signal" );
  DummyControl actor = DummyControl::New(true);
  actor.ResourceReadySignal().Connect( &ResourceReadySignal);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
  application.GetScene().Add( actor );

  tet_infoline( "Create visual with ATTACHED load policy" );
  Visual::Base imageVisual = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::LOAD_POLICY, ImageVisual::LoadPolicy::ATTACHED );

  tet_infoline( "Registering visual allows control to get a signal once loaded" );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual );
  imageVisual.Reset(); // reduce ref count so only the control keeps the visual alive.

  tet_infoline( "Allow image time to load" );
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
  application.SendNotification();
  application.Render();

  tet_infoline( "Testing texture is loaded and resource ready signal fired" );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  tet_infoline( "Original control correctly signalled, now testing for signal with new Control reusing the image" );

  gResourceReadySignalFired = false; // Reset signal check ready for testing next Control
  Visual::Base imageVisual2 = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::LOAD_POLICY, ImageVisual::LoadPolicy::ATTACHED );
  DummyControl actor2 = DummyControl::New(true);
  Impl::DummyControl& dummyImpl2 = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  actor2.ResourceReadySignal().Connect( &ResourceReadySignal);

  tet_infoline( "Registering visual this should trigger the loading signal as is already image loaded for previous control" );
  dummyImpl2.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual2 );
  imageVisual2.Reset(); // reduce ref count so only the control keeps the visual alive.
  actor2.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 0 ), true, TEST_LOCATION ); // Not expecting any further loading as texture is being reused.
  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  END_TEST;
}


int UtcDaliImageVisualOrientationCorrection(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualOrientationCorrection Enabling OrientationCorrection should rotate an image with exif (90deg) orientation data with requested" );

  VisualFactory factory = VisualFactory::Get();
  tet_infoline( "Create visual with Orientation correction set OFF" );
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_ROTATED_IMAGE );
  propertyMap.Insert( "orientationCorrection", false );
  Visual::Base imageVisual = factory.CreateVisual( propertyMap );

  tet_infoline( "Create control for visual, need to loaded it" );
  DummyControl actor = DummyControl::New(true);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  application.GetScene().Add( actor );

  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual );
  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  Vector2 originalImageSize;
  tet_infoline( "Get size of original visual to compare later with rotated image" );
  imageVisual.GetNaturalSize( originalImageSize );
  DALI_TEST_GREATER( originalImageSize.width, originalImageSize.height, TEST_LOCATION ); // Width and Height must be different for this test.
  imageVisual.Reset();  // remove handle so can unregister it and remove from cache
  dummyImpl.UnregisterVisual( DummyControl::Property::TEST_VISUAL );
  application.SendNotification();
  application.Render();

  tet_infoline( "Create visual with Orientation correction set ON " );
  propertyMap.Clear();
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_ROTATED_IMAGE );
  propertyMap.Insert( ImageVisual::Property::ORIENTATION_CORRECTION, true );
  imageVisual = factory.CreateVisual( propertyMap );

  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual );
  // Wait for image to load
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  Vector2 rotatedImageSize;
  imageVisual.GetNaturalSize( rotatedImageSize );
  tet_infoline( "Confirm that visual has rotated" );
  DALI_TEST_EQUALS( originalImageSize.width, rotatedImageSize.height , TEST_LOCATION );
  DALI_TEST_EQUALS( originalImageSize.height, rotatedImageSize.width , TEST_LOCATION );

  Property::Map resultMap;
  imageVisual.CreatePropertyMap( resultMap );

  // check the Property::ORIENTATION_CORRECTION value from the returned map
  Property::Value* typeValue = resultMap.Find( ImageVisual::Property::ORIENTATION_CORRECTION,  Property::BOOLEAN );
  DALI_TEST_EQUALS( typeValue->Get<bool>(), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualCustomShader(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualCustomShader Test custom shader" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map properties;
  Property::Map shader;
  const std::string vertexShader = "Foobar";
  const std::string fragmentShader = "Foobar";
  shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
  shader[Visual::Shader::Property::VERTEX_SHADER] = vertexShader;

  properties[Visual::Property::TYPE] = Visual::IMAGE;
  properties[Visual::Property::SHADER] = shader;
  properties[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;

  Visual::Base visual = factory.CreateVisual( properties );

  // trigger creation through setting on stage
  DummyControl dummy = DummyControl::New( true );
  Impl::DummyControl& dummyImpl = static_cast< Impl::DummyControl& >( dummy.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );

  dummy.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  dummy.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  application.GetScene().Add( dummy );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  Renderer renderer = dummy.GetRendererAt( 0 );
  Shader shader2 = renderer.GetShader();
  Property::Value value = shader2.GetProperty( Shader::Property::PROGRAM );
  Property::Map* map = value.GetMap();
  DALI_TEST_CHECK( map );

  Property::Value* fragment = map->Find( "fragment" ); // fragment key name from shader-impl.cpp
  DALI_TEST_EQUALS( fragmentShader, fragment->Get< std::string >(), TEST_LOCATION );

  Property::Value* vertex = map->Find( "vertex" ); // vertex key name from shader-impl.cpp
  DALI_TEST_EQUALS( vertexShader, vertex->Get< std::string >(), TEST_LOCATION );

  shader.Clear();

  shader[Visual::Shader::Property::HINTS] = Shader::Hint::OUTPUT_IS_TRANSPARENT;
  properties[Visual::Property::SHADER] = shader;

  Visual::Base visual1 = factory.CreateVisual( properties );

  // trigger creation through setting on stage
  DummyControl dummy1 = DummyControl::New( true );
  Impl::DummyControl& dummyImpl1 = static_cast< Impl::DummyControl& >( dummy1.GetImplementation() );
  dummyImpl1.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual1 );
  dummy1.SetProperty( Actor::Property::SIZE, Vector2( 200, 200 ) );
  dummy1.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
  application.GetScene().Add( dummy1 );

  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  glAbstraction.EnableEnableDisableCallTrace( true );

  application.SendNotification();
  application.Render();

  TraceCallStack& glEnableStack = glAbstraction.GetEnableDisableTrace();
  std::ostringstream blendStr;
  blendStr << GL_BLEND;
  DALI_TEST_CHECK( glEnableStack.FindMethodAndParams( "Enable", blendStr.str().c_str() ) );

  END_TEST;
}


void ResourceReadyLoadNext( Control control )
{
  static int callNumber = 0;

  gResourceReadySignalFired = true;
  gReadyIds.push_back(control.GetProperty< int >( Actor::Property::ID ));

  if( callNumber == 0 )
  {
    DALI_TEST_EQUALS( control.GetVisualResourceStatus(DummyControl::Property::TEST_VISUAL), Toolkit::Visual::ResourceStatus::FAILED, TEST_LOCATION );

    tet_infoline( "Create visual with loaded image from within the signal handler" );
    VisualFactory factory = VisualFactory::Get();
    Visual::Base imageVisual = factory.CreateVisual( TEST_IMAGE_FILE_NAME, ImageDimensions{20,30} );

    Impl::DummyControl& controlImpl = static_cast<Impl::DummyControl&>(control.GetImplementation());
    controlImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual ); // This should trigger another signal.
    callNumber = 1;
  }
  else
  {
    tet_infoline( "3rd signal called" );
    DALI_TEST_CHECK(true);
  }
}

int UtcDaliImageVisualLoadReady01(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliImageVisualLoadReady01");
  tet_infoline( "First part:  Load an image visual for one resource, then another image visual for a second resource.");
  tet_infoline( "Second part, In the ready signal for the second image visual, add a 3rd visual with the first URL" );
  tet_infoline( "Should get a ready signal for all three visuals");

  ClearReadyIds();

  tet_infoline( "Create a control and connect to resource ready signal" );
  DummyControl actor = DummyControl::New(true);
  int actor1Id = actor.GetProperty< int >( Actor::Property::ID );
  actor.ResourceReadySignal().Connect( &ResourceReadySignal);
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(actor.GetImplementation());
  actor.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
  application.GetScene().Add(actor);

  tet_infoline( "Create visual with IMMEDIATE load policy" );
  Visual::Base imageVisual1 = CreateVisualWithPolicy( TEST_IMAGE_FILE_NAME, ImageVisual::Property::LOAD_POLICY, ImageVisual::LoadPolicy::IMMEDIATE );

  tet_infoline( "Registering visual allows control to get a signal once loaded even if visual not enabled( staged )" );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual1 );


  tet_infoline( "Allow image time to load" );
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
  application.SendNotification();
  application.Render();

  tet_infoline( "Testing texture is loaded and resource ready signal fired" );
  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );
  DALI_TEST_EQUALS( gReadyIds[0], actor1Id, TEST_LOCATION );


  tet_infoline( "Original control correctly signalled, now testing failing image" );

  gResourceReadySignalFired = false; // Reset signal check ready for testing next Control
  ClearReadyIds();

  Visual::Base imageVisual2 = CreateVisualWithPolicy( TEST_BROKEN_IMAGE_FILE_NAME, ImageVisual::Property::LOAD_POLICY, ImageVisual::LoadPolicy::IMMEDIATE );

  DummyControl actor2 = DummyControl::New(true);
  int actor2Id = actor2.GetProperty< int >( Actor::Property::ID );
  Impl::DummyControl& dummyImpl2 = static_cast<Impl::DummyControl&>(actor2.GetImplementation());
  actor2.ResourceReadySignal().Connect( &ResourceReadyLoadNext);

  tet_infoline( "Registering visual this should trigger the ready signal when the image fails to load" );
  dummyImpl2.RegisterVisual( DummyControl::Property::TEST_VISUAL, imageVisual2 );

  actor2.SetProperty( Actor::Property::SIZE, Vector2(200.f, 200.f) );
  application.GetScene().Add(actor2);

  tet_infoline( "Wait for loading thread to finish");
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  DALI_TEST_EQUALS( gReadyIds[0], actor2Id, TEST_LOCATION);

  tet_infoline( "Check for 3rd signal");
  application.SendNotification();
  DALI_TEST_EQUALS( gReadyIds.size(), 2, TEST_LOCATION );
  DALI_TEST_EQUALS( gReadyIds[1], actor2Id, TEST_LOCATION);

  END_TEST;
}
