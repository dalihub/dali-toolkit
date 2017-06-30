/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/devel-api/images/nine-patch-image.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali-toolkit/devel-api/align-enums.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
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
const char* TEST_LARGE_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR "/tbcol.png";
const char* TEST_SMALL_IMAGE_FILE_NAME = TEST_RESOURCE_DIR "/icon-edit.png";
const char* TEST_REMOTE_IMAGE_FILE_NAME = "https://www.tizen.org/sites/all/themes/tizen_theme/logo.png";
const char* TEST_INVALID_FILE_NAME =  TEST_RESOURCE_DIR  "/invalid.jpg";
const char* TEST_REMOTE_INVALID_FILE_NAME = "https://www.tizen.org/invalid.png";
const char* TEST_MASK_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR "/mask.png";
}


Actor CreateActorWithImageVisual(const Property::Map& map)
{
  VisualFactory factory = VisualFactory::Get();
  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  Visual::Base visual = factory.CreateVisual( map );
  DALI_TEST_CHECK( visual );
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetSize( 200.f, 200.f );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  return actor;
}

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

  actor.SetSize( 200.f, 200.f );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Stage::GetCurrent().Add( actor );

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

  value = map.Find( DevelVisual::Property::MIX_COLOR );
  DALI_TEST_CHECK( value );
  Vector4 mixColor2;
  DALI_TEST_CHECK( value->Get( mixColor2 ) );
  DALI_TEST_EQUALS( mixColor2, testColor, 0.001, TEST_LOCATION );

  value = map.Find( DevelVisual::Property::OPACITY );
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

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
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

  actor.SetSize( 200.f, 200.f );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Stage::GetCurrent().Add( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
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
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_REMOTE_IMAGE_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetSize( 200.f, 200.f );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Stage::GetCurrent().Add( actor );
  application.SendNotification();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliImageVisualTextureReuse1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request remote image visual with a Property::Map; request a second visual with the same property map - should reuse texture" );

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_LARGE_IMAGE_FILE_NAME );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  Actor actor = CreateActorWithImageVisual( propertyMap );
  Stage::GetCurrent().Add( actor );
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
  Stage::GetCurrent().Add(actor2);

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

  Stage::GetCurrent().Remove( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 0, TEST_LOCATION );

  tet_infoline("Test that removing last actor does delete the texture\n");

  Stage::GetCurrent().Remove( actor2 );
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
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_REMOTE_IMAGE_FILE_NAME );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  Actor actor = CreateActorWithImageVisual( propertyMap );
  Stage::GetCurrent().Add( actor );
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
  Stage::GetCurrent().Add(actor2);

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

  Stage::GetCurrent().Remove( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 1, TEST_LOCATION );

  tet_infoline("Test that removing last actor deletes it's texture\n");

  Stage::GetCurrent().Remove( actor2 );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor2.GetRendererCount() == 0u );
  DALI_TEST_EQUALS( textureTrace.CountMethod("DeleteTextures"), 2, TEST_LOCATION );

  END_TEST;
}


int UtcDaliImageVisualImageHandle(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with an image handle" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME);
  Visual::Base visual = factory.CreateVisual( image );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  const int width=512;
  const int height=513;

  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, width, height,width, height );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual, 1u,
                    ImageDimensions(width, height),
                    Integration::ResourcePointer(bitmap) );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
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
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_SMALL_IMAGE_FILE_NAME );
  propertyMap.Insert( ImageVisual::Property::DESIRED_WIDTH, width );
  propertyMap.Insert( ImageVisual::Property::DESIRED_HEIGHT, height );
  propertyMap.Insert( ImageVisual::Property::SYNCHRONOUS_LOADING, true );
  propertyMap.Insert( ImageVisual::Property::PIXEL_AREA, pixelArea );
  propertyMap.Insert( ImageVisual::Property::WRAP_MODE_U, WrapMode::MIRRORED_REPEAT );
  propertyMap.Insert( ImageVisual::Property::WRAP_MODE_V, WrapMode::REPEAT );
  propertyMap.Insert( DevelImageVisual::Property::ATLASING, true );

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
  actor.SetSize(2000, 2000);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  Stage::GetCurrent().Add( actor );

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
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
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
  actor.SetSize(2000, 2000);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  Stage::GetCurrent().Add( actor );

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

  actor.SetSize(2000, 2000);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetColor(Color::BLACK);
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index index = DevelHandle::GetPropertyIndex( renderer, DevelVisual::Property::MIX_COLOR );
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

  blendModeValue = renderer.GetProperty( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( blendModeValue.Get<int>(), (int)BlendMode::ON, TEST_LOCATION );

  animation.AnimateTo( Property(actor, Actor::Property::COLOR), Color::WHITE );
  animation.Play();

  application.SendNotification();
  application.Render(0); // Ensure animation starts
  application.Render(2000u); // Halfway point
  Vector4 testColor(1.0f, 0.0f, 0.5f, 0.75f );

  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>("uColor", Vector4(0.5f, 0.5f, 0.5f, 1.0f )), true, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector3>("mixColor", Vector3(testColor)), true, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<float>("opacity", testColor.a), true, TEST_LOCATION );

  application.Render(2000u); // Halfway point between blue and white

  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::WHITE, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector4>("uColor", Color::WHITE ), true, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<Vector3>("mixColor", Vector3(TARGET_MIX_COLOR)), true, TEST_LOCATION );
  DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<float>("opacity", TARGET_MIX_COLOR.a), true, TEST_LOCATION );

  TestMixColor( visual, DevelVisual::Property::MIX_COLOR, TARGET_MIX_COLOR );

  blendModeValue = renderer.GetProperty( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( blendModeValue.Get<int>(), (int)BlendMode::ON, TEST_LOCATION );

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

  actor.SetSize(2000, 2000);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetColor(Color::BLACK);
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0);
  tet_infoline("Test that the renderer has the opacity property");
  Property::Index index = DevelHandle::GetPropertyIndex( renderer, DevelVisual::Property::OPACITY );
  DALI_TEST_CHECK( index != Property::INVALID_INDEX );


  Property::Value blendModeValue = renderer.GetProperty( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( blendModeValue.Get<int>(), (int)BlendMode::ON, TEST_LOCATION );

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

    application.SendNotification();
    application.Render(0);     // Ensure animation starts
    application.Render(2000u); // Halfway point through animation
    application.SendNotification(); // Handle any signals

    DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<float>("opacity", 0.75f), true, TEST_LOCATION );

    application.Render(2001u); // end
    application.SendNotification(); // ensure animation finished signal is sent

    DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<float>("opacity", 1.0f), true, TEST_LOCATION );

    blendModeValue = renderer.GetProperty( Renderer::Property::BLEND_MODE );
    DALI_TEST_EQUALS( blendModeValue.Get<int>(), (int)BlendMode::AUTO, TEST_LOCATION );
  }


  {
    tet_infoline( "Test that the opacity can be reduced via animation, and that the blend mode is set appropriately at the start and end of the animation." );

    Property::Map map;
    map["target"] = "testVisual";
    map["property"] = DevelVisual::Property::OPACITY;
    map["targetValue"] = 0.1f;
    map["animator"] = Property::Map()
      .Add("alphaFunction", "LINEAR")
      .Add("timePeriod", Property::Map()
           .Add("delay", 0.0f)
           .Add("duration", 4.0f));

    Dali::Toolkit::TransitionData transition = TransitionData::New( map );
    Animation animation = dummyImpl.CreateTransition( transition );
    animation.Play();

    blendModeValue = renderer.GetProperty( Renderer::Property::BLEND_MODE );
    DALI_TEST_EQUALS( blendModeValue.Get<int>(), (int)BlendMode::ON, TEST_LOCATION );

    application.SendNotification();
    application.Render(0);     // Ensure animation starts
    application.Render(2000u); // Halfway point
    application.SendNotification();

    DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<float>("opacity", 0.55f), true, TEST_LOCATION );

    application.Render(2016u); // end
    application.SendNotification();

    DALI_TEST_EQUALS( application.GetGlAbstraction().CheckUniformValue<float>("opacity", 0.1f), true, TEST_LOCATION );

    blendModeValue = renderer.GetProperty( Renderer::Property::BLEND_MODE );
    DALI_TEST_EQUALS( blendModeValue.Get<int>(), (int)BlendMode::ON, TEST_LOCATION );
  }


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

  actor.SetSize(2000, 2000);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  actor.SetColor(Color::BLACK);
  Stage::GetCurrent().Add(actor);

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION);

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index index = DevelHandle::GetPropertyIndex( renderer, DevelVisual::Property::MIX_COLOR );

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
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_REMOTE_IMAGE_FILE_NAME );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  Actor actor = CreateActorWithImageVisual( propertyMap );
  Stage::GetCurrent().Add( actor );
  application.SendNotification();

  Stage::GetCurrent().Remove( actor );
  application.SendNotification();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), false, TEST_LOCATION );
  DALI_TEST_EQUALS( drawTrace.FindMethod("DrawArrays"), false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageVisualSetInvalidAsyncImage(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Request image visual with invalid images - should draw broken.png" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_INVALID_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetSize( 200.f, 200.f );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
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
  propertyMap.Insert( Visual::Property::TYPE, Visual::IMAGE );
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

  actor.SetSize( 200.f, 200.f );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
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
  propertyMap.Insert( Visual::Property::TYPE, Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL, TEST_REMOTE_INVALID_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetSize( 200.f, 200.f );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
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
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_LARGE_IMAGE_FILE_NAME );
  propertyMap.Insert( DevelImageVisual::Property::ALPHA_MASK_URL, TEST_MASK_IMAGE_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  Property::Map testMap;
  visual.CreatePropertyMap(testMap);
  DALI_TEST_EQUALS(*testMap.Find(DevelImageVisual::Property::ALPHA_MASK_URL),Property::Value(TEST_MASK_IMAGE_FILE_NAME), TEST_LOCATION );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetSize( 200.f, 200.f );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( DevelControl::IsResourceReady( actor ), false, TEST_LOCATION );

  Stage::GetCurrent().Add( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( DevelControl::IsResourceReady( actor ), true, TEST_LOCATION );

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
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_IMAGE_FILE_NAME );
  propertyMap.Insert( "alphaMaskUrl", MASK_IMAGE );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  Property::Map testMap;
  visual.CreatePropertyMap(testMap);
  DALI_TEST_EQUALS(*testMap.Find(DevelImageVisual::Property::ALPHA_MASK_URL),Property::Value(MASK_IMAGE), TEST_LOCATION );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  DALI_TEST_EQUALS( DevelControl::IsResourceReady( actor ), false, TEST_LOCATION );

  actor.SetSize( 200.f, 200.f );
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );

  Stage::GetCurrent().Add( actor );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );
  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  DALI_TEST_EQUALS( DevelControl::IsResourceReady( actor ), true, TEST_LOCATION );

  END_TEST;
}
