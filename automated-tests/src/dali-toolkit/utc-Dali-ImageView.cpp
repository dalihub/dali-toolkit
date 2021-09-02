/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>
#include <toolkit-event-thread-callback.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-toolkit/devel-api/visuals/image-visual-actions-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/image-loader/image.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>

#include <test-native-image.h>
#include <test-encoded-image-buffer.h>
#include <sstream>
#include <unistd.h>


#include "dummy-control.h"

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_image_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_image_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const char* TEST_IMAGE_FILE_NAME =  "gallery_image_01.jpg";
const char* TEST_IMAGE_FILE_NAME2 =  "gallery_image_02.jpg";

const char* TEST_IMAGE_1 = TEST_RESOURCE_DIR "/TB-gloss.png";
const char* TEST_IMAGE_2 = TEST_RESOURCE_DIR "/tb-norm.png";

// resolution: 34*34, pixel format: RGBA8888
static const char* gImage_34_RGBA = TEST_RESOURCE_DIR "/icon-edit.png";
// resolution: 600*600, pixel format: RGB888
static const char* gImage_600_RGB = TEST_RESOURCE_DIR "/test-image-600.jpg";

// resolution: 50*50, frame count: 4, frame delay: 0.2 second for each frame
const char* TEST_GIF_FILE_NAME = TEST_RESOURCE_DIR "/anim.gif";

const char* TEST_VECTOR_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR  "/insta_camera.json";

void TestUrl( ImageView imageView, const std::string url )
{
  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  std::string urlActual;
  DALI_TEST_CHECK( value.Get( urlActual ) );
  DALI_TEST_EQUALS( urlActual, url, TEST_LOCATION );
}

} // namespace

int UtcDaliImageViewNewP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  DALI_TEST_CHECK( imageView );

  END_TEST;
}

int UtcDaliImageViewNewUrlP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New( TEST_IMAGE_FILE_NAME );
  DALI_TEST_CHECK( imageView );

  TestUrl( imageView, TEST_IMAGE_FILE_NAME );

  END_TEST;
}

int UtcDaliImageViewConstructorP(void)
{
  ToolkitTestApplication application;

  ImageView imageView;

  DALI_TEST_CHECK( !imageView );

  END_TEST;
}

int UtcDaliImageViewCopyConstructorP(void)
{
  ToolkitTestApplication application;

  // Initialize an object, ref count == 1
  ImageView imageView = ImageView::New();

  ImageView copy( imageView );
  DALI_TEST_CHECK( copy );

  END_TEST;
}

int UtcDaliImageViewMoveConstructor(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  DALI_TEST_EQUALS( 1, imageView.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  imageView.SetProperty( Actor::Property::SENSITIVE, false );
  DALI_TEST_CHECK( false == imageView.GetProperty< bool >( Actor::Property::SENSITIVE ) );

  ImageView moved = std::move( imageView );
  DALI_TEST_CHECK( moved );
  DALI_TEST_EQUALS( 1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  DALI_TEST_CHECK( false == moved.GetProperty< bool >( Actor::Property::SENSITIVE ) );
  DALI_TEST_CHECK( !imageView );

  END_TEST;
}

int UtcDaliImageViewAssignmentOperatorP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  ImageView copy( imageView );
  DALI_TEST_CHECK( copy );
  DALI_TEST_EQUALS( imageView, copy, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewMoveAssignment(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  DALI_TEST_EQUALS( 1, imageView.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  imageView.SetProperty( Actor::Property::SENSITIVE, false );
  DALI_TEST_CHECK( false == imageView.GetProperty< bool >( Actor::Property::SENSITIVE ) );

  ImageView moved;
  moved = std::move( imageView );
  DALI_TEST_CHECK( moved );
  DALI_TEST_EQUALS( 1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION );
  DALI_TEST_CHECK( false == moved.GetProperty< bool >( Actor::Property::SENSITIVE ) );
  DALI_TEST_CHECK( !imageView );

  END_TEST;
}

int UtcDaliImageViewDownCastP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  BaseHandle object(imageView);

  ImageView imageView2 = ImageView::DownCast( object );
  DALI_TEST_CHECK(imageView2);

  ImageView imageView3 = DownCast< ImageView >( object );
  DALI_TEST_CHECK(imageView3);

  END_TEST;
}

int UtcDaliImageViewDownCastN(void)
{
  ToolkitTestApplication application;

  BaseHandle unInitializedObject;

  ImageView imageView1 = ImageView::DownCast( unInitializedObject );
  DALI_TEST_CHECK( !imageView1 );

  ImageView imageView2 = DownCast< ImageView >( unInitializedObject );
  DALI_TEST_CHECK( !imageView2 );

  END_TEST;
}

int UtcDaliImageViewTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK( typeRegistry );

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "ImageView" );
  DALI_TEST_CHECK( typeInfo );

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );

  ImageView imageView = ImageView::DownCast( handle );
  DALI_TEST_CHECK( imageView );

  END_TEST;
}

int UtcDaliImageViewSetGetProperty01(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Property::Index idx = imageView.GetPropertyIndex( "image" );
  DALI_TEST_EQUALS( idx, (Property::Index)ImageView::Property::IMAGE, TEST_LOCATION );

  imageView.SetProperty( idx, TEST_IMAGE_FILE_NAME );
  TestUrl( imageView, TEST_IMAGE_FILE_NAME );

  END_TEST;
}

int UtcDaliImageViewPreMultipliedAlphaPng(void)
{
  ToolkitTestApplication application;

  // Set up trace debug
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable( true );

  Property::Map imageMap;
  imageMap[ ImageVisual::Property::URL ] = gImage_34_RGBA;
  imageMap[ ImageVisual::Property::RELEASE_POLICY] = ImageVisual::ReleasePolicy::NEVER;   // To keep the texture cache

  ImageView imageView1 = ImageView::New();
  imageView1.SetProperty( ImageView::Property::IMAGE, imageMap );

  application.GetScene().Add( imageView1 );

  Property::Value value = imageView1.GetProperty( ImageView::Property::PRE_MULTIPLIED_ALPHA );
  bool enable;
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( enable );    // Default value is true

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  value = imageView1.GetProperty( ImageView::Property::PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( enable );    // Keep true

  // conventional alpha blending
  Renderer renderer1 = imageView1.GetRendererAt( 0 );
  value = renderer1.GetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( enable );

  int srcFactorRgb    = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  int destFactorRgb   = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  int srcFactorAlpha  = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  int destFactorAlpha = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );
  DALI_TEST_CHECK( srcFactorRgb == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA );
  DALI_TEST_CHECK( srcFactorAlpha == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA );

  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );  // A new texture should be generated.
  textureTrace.Reset();

  // Disable pre-multiplied alpha blending
  imageView1.SetProperty( ImageView::Property::PRE_MULTIPLIED_ALPHA, false );

  // Reload the image
  Property::Map attributes;
  DevelControl::DoAction( imageView1, ImageView::Property::IMAGE, DevelImageVisual::Action::RELOAD, attributes );

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  value = imageView1.GetProperty( ImageView::Property::PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( !enable );

  // conventional alpha blending
  value = renderer1.GetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( !enable );

  srcFactorRgb    = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  destFactorRgb   = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  srcFactorAlpha  = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  destFactorAlpha = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );
  DALI_TEST_CHECK( srcFactorRgb == BlendFactor::SRC_ALPHA );
  DALI_TEST_CHECK( destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA );
  DALI_TEST_CHECK( srcFactorAlpha == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA );

  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );  // A new texture should be generated.
  textureTrace.Reset();

  // Make a new ImageView using the same image
  ImageView imageView2 = ImageView::New();
  imageView2.SetProperty( ImageView::Property::IMAGE, imageMap );

  application.GetScene().Add( imageView2 );

  application.SendNotification();
  application.Render();

  Renderer renderer2 = imageView2.GetRendererAt( 0 );
  value = renderer2.GetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( enable );

  srcFactorRgb    = renderer2.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  destFactorRgb   = renderer2.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  srcFactorAlpha  = renderer2.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  destFactorAlpha = renderer2.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );
  DALI_TEST_CHECK( srcFactorRgb == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA );
  DALI_TEST_CHECK( srcFactorAlpha == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA );

  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION ); // The cached texture should be used.

  END_TEST;
}

int UtcDaliImageViewPreMultipliedAlphaJpg(void)
{
  ToolkitTestApplication application;

  // Set up trace debug
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable( true );

  Property::Map imageMap;
  imageMap[ ImageVisual::Property::URL ] = gImage_600_RGB;
  imageMap[ ImageVisual::Property::RELEASE_POLICY] = ImageVisual::ReleasePolicy::NEVER;   // To keep the texture cache

  ImageView imageView1 = ImageView::New();
  imageView1.SetProperty( ImageView::Property::IMAGE, imageMap );

  application.GetScene().Add( imageView1 );

  Property::Value value = imageView1.GetProperty( ImageView::Property::PRE_MULTIPLIED_ALPHA );
  bool enable;
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( enable );    // Default value is true

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  value = imageView1.GetProperty( ImageView::Property::PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( !enable );    // Should be false after loading

  // conventional alpha blending
  Renderer renderer1 = imageView1.GetRendererAt( 0 );
  value = renderer1.GetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( !enable );

  int srcFactorRgb    = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  int destFactorRgb   = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  int srcFactorAlpha  = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  int destFactorAlpha = renderer1.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );
  DALI_TEST_CHECK( srcFactorRgb == BlendFactor::SRC_ALPHA );
  DALI_TEST_CHECK( destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA );
  DALI_TEST_CHECK( srcFactorAlpha == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA );

  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), true, TEST_LOCATION );  // A new texture should be generated.
  textureTrace.Reset();

  ImageView imageView2 = ImageView::New();
  imageView2.SetProperty( ImageView::Property::IMAGE, imageMap );

  // Disable pre-multiplied alpha blending
  imageView2.SetProperty( ImageView::Property::PRE_MULTIPLIED_ALPHA, false );

  application.GetScene().Add( imageView2 );

  application.SendNotification();
  application.Render();

  value = imageView2.GetProperty( ImageView::Property::PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( !enable );

  // conventional alpha blending
  Renderer renderer2 = imageView2.GetRendererAt( 0 );
  value = renderer2.GetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( !enable );

  srcFactorRgb    = renderer2.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  destFactorRgb   = renderer2.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  srcFactorAlpha  = renderer2.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  destFactorAlpha = renderer2.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );
  DALI_TEST_CHECK( srcFactorRgb == BlendFactor::SRC_ALPHA );
  DALI_TEST_CHECK( destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA );
  DALI_TEST_CHECK( srcFactorAlpha == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA );

  DALI_TEST_EQUALS( textureTrace.FindMethod("GenTextures"), false, TEST_LOCATION ); // The cached texture should be used.

  END_TEST;
}

int UtcDaliImageViewPixelArea(void)
{
  // Test pixel area property
  ToolkitTestApplication application;

  static std::vector<UniformData> customUniforms =
  {
    UniformData("pixelArea", Property::Type::VECTOR4),
  };

  TestGraphicsController& graphics = application.GetGraphicsController();
  graphics.AddCustomUniforms(customUniforms);

  // Gif image, use AnimatedImageVisual internally
  // Atlasing is applied to pack multiple frames, use custom wrap mode
  ImageView gifView = ImageView::New();
  const Vector4 pixelAreaVisual( 0.f, 0.f, 2.f, 2.f );
  gifView.SetProperty( ImageView::Property::IMAGE,
                       Property::Map().Add( ImageVisual::Property::URL, TEST_GIF_FILE_NAME )
                                      .Add( ImageVisual::Property::PIXEL_AREA, pixelAreaVisual ) );

  // Add to stage
  Integration::Scene stage = application.GetScene();
  stage.Add( gifView );

  // loading started
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( gifView.GetRendererCount() == 1u );

  const Vector4 fullTextureRect( 0.f, 0.f, 1.f, 1.f );
  // test that the pixel area value defined in the visual property map is registered on renderer
  Renderer renderer = gifView.GetRendererAt(0);
  Property::Value pixelAreaValue = renderer.GetProperty( renderer.GetPropertyIndex( "pixelArea" ) );
  DALI_TEST_EQUALS( pixelAreaValue.Get<Vector4>(), pixelAreaVisual, TEST_LOCATION );

  // test that the shader has the default pixel area value registered.
  Shader shader = renderer.GetShader();
  pixelAreaValue = shader.GetProperty( shader.GetPropertyIndex( "pixelArea" ) );
  DALI_TEST_EQUALS( pixelAreaValue.Get<Vector4>(), fullTextureRect, TEST_LOCATION );

  // test that the uniform uses the pixelArea property on the renderer.
  TestGlAbstraction& gl = application.GetGlAbstraction();
  Vector4 pixelAreaUniform;
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "pixelArea", pixelAreaUniform ) );
  DALI_TEST_EQUALS( pixelAreaVisual, pixelAreaUniform, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  // set the pixelArea property on the control
  const Vector4 pixelAreaControl( -1.f, -1.f, 3.f, 3.f );
  gifView.SetProperty( ImageView::Property::PIXEL_AREA, pixelAreaControl );
  application.SendNotification();
  application.Render(16);

  // check the pixelArea property on the control
  pixelAreaValue = gifView.GetProperty( gifView.GetPropertyIndex( "pixelArea" ) );
  DALI_TEST_EQUALS( pixelAreaValue.Get<Vector4>(), pixelAreaControl, TEST_LOCATION );
  // test that the uniform uses the pixelArea property on the control.
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "pixelArea", pixelAreaUniform ) );
  DALI_TEST_EQUALS( pixelAreaControl, pixelAreaUniform, Math::MACHINE_EPSILON_100, TEST_LOCATION );


  END_TEST;
}

int UtcDaliImageViewAsyncLoadingWithoutAltasing(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();
  const std::vector<GLuint>& textures = gl.GetBoundTextures();
  size_t numTextures = textures.size();

  // Async loading, no atlasing for big size image
  ImageView imageView = ImageView::New( gImage_600_RGB );

  // By default, Aysnc loading is used
  application.GetScene().Add( imageView );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(100, 100) );
  imageView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  const std::vector<GLuint>& textures2 = gl.GetBoundTextures();
  DALI_TEST_GREATER( textures2.size(), numTextures, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewAsyncLoadingWithAtlasing(void)
{
  ToolkitTestApplication application;

  //Async loading, automatic atlasing for small size image
  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);

  Property::Map imageMap;

  imageMap[ ImageVisual::Property::URL ] = gImage_34_RGBA;
  imageMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 34;
  imageMap[ ImageVisual::Property::DESIRED_WIDTH ] = 34;
  imageMap[ ImageVisual::Property::ATLASING] = true;

  ImageView imageView = ImageView::New();
  imageView.SetProperty( ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Toolkit::Control::Property::PADDING, Extents( 10u, 10u, 10u, 10u ) );

  // By default, Aysnc loading is used
  // loading is not started if the actor is offScene

  application.GetScene().Add( imageView );
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  imageView.SetProperty( Dali::Actor::Property::LAYOUT_DIRECTION,  Dali::LayoutDirection::RIGHT_TO_LEFT );
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);

  callStack.Enable(false);

  TraceCallStack::NamedParams params;
  params["width"] << 34;
  params["height"] << 34;
DALI_TEST_EQUALS( callStack.FindMethodAndParams( "TexSubImage2D", params ), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewAsyncLoadingWithAtlasing02(void)
{
  ToolkitTestApplication application;

  //Async loading, automatic atlasing for small size image
  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);

  Property::Map asyncLoadingMap;
  asyncLoadingMap[ "url" ] = gImage_34_RGBA;
  asyncLoadingMap[ "desiredHeight" ] = 34;
  asyncLoadingMap[ "desiredWidth" ] = 34;
  asyncLoadingMap[ "synchronousLoading" ] = false;
  asyncLoadingMap[ "atlasing" ] = true;

  ImageView imageView = ImageView::New();
  imageView.SetProperty( ImageView::Property::IMAGE, asyncLoadingMap );

  application.GetScene().Add( imageView );
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);

  callStack.Enable(false);

  TraceCallStack::NamedParams params;
  params["width"] << 34;
  params["height"] << 34;
  DALI_TEST_EQUALS( callStack.FindMethodAndParams( "TexSubImage2D", params ), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSyncLoading(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing sync loading and size using index key property map");

  Property::Map syncLoadingMap;
  syncLoadingMap[ ImageVisual::Property::SYNCHRONOUS_LOADING ] = true;
  syncLoadingMap[ ImageVisual::Property::ATLASING ] = true;

  // Sync loading, no atlasing for big size image
  {
    ImageView imageView = ImageView::New();

    // Sync loading is used
    syncLoadingMap[ ImageVisual::Property::URL ] = gImage_600_RGB;
    imageView.SetProperty( ImageView::Property::IMAGE, syncLoadingMap );
  }

  // Sync loading, automatic atlasing for small size image
  {
    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
    callStack.Reset();
    callStack.Enable(true);

    ImageView imageView = ImageView::New( );

    // Sync loading is used
    syncLoadingMap[ ImageVisual::Property::URL ] = gImage_34_RGBA;
    syncLoadingMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 34;
    syncLoadingMap[ ImageVisual::Property::DESIRED_WIDTH ] = 34;
    imageView.SetProperty( ImageView::Property::IMAGE, syncLoadingMap );

    application.GetScene().Add( imageView );
    application.SendNotification();
    application.Render(16);

    TraceCallStack::NamedParams params;
    params["width"] << 34;
    params["height"] << 34;
    DALI_TEST_EQUALS( callStack.FindMethodAndParams( "TexSubImage2D", params ),
                      true, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliImageViewSyncLoading02(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing sync loading and size using string key property map");

  // Sync loading, automatic atlasing for small size image
  {
    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
    callStack.Reset();
    callStack.Enable(true);

    ImageView imageView = ImageView::New( );

    // Sync loading is used
    Property::Map syncLoadingMap;
    syncLoadingMap[ "url" ] = gImage_34_RGBA;
    syncLoadingMap[ "desiredHeight" ] = 34;
    syncLoadingMap[ "desiredWidth" ] = 34;
    syncLoadingMap[ "synchronousLoading" ] = true;
    syncLoadingMap[ "atlasing" ] = true;
    imageView.SetProperty( ImageView::Property::IMAGE, syncLoadingMap );

    application.GetScene().Add( imageView );
    application.SendNotification();
    application.Render(16);

    TraceCallStack::NamedParams params;
    params["width"] << 34;
    params["height"] << 34;
    DALI_TEST_EQUALS( callStack.FindMethodAndParams( "TexSubImage2D", params ),
                      true, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliImageViewAsyncLoadingEncodedBuffer(void)
{
  ToolkitTestApplication application;
  TestGlAbstraction& gl = application.GetGlAbstraction();
  const std::vector<GLuint>& textures = gl.GetBoundTextures();
  size_t numTextures = textures.size();

  // Get encoded raw-buffer image and generate url
  EncodedImageBuffer buffer = ConvertFileToEncodedImageBuffer(gImage_600_RGB);
  ImageUrl           url    = Toolkit::Image::GenerateUrl(buffer);

  // Async loading, no atlasing for big size image
  ImageView imageView = ImageView::New(url.GetUrl());

  // By default, Aysnc loading is used
  application.GetScene().Add(imageView);
  imageView.SetProperty( Actor::Property::SIZE, Vector2(100, 100) );
  imageView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);
  application.SendNotification();

  const std::vector<GLuint>& textures2 = gl.GetBoundTextures();
  DALI_TEST_GREATER( textures2.size(), numTextures, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewAsyncLoadingEncodedBufferWithAtlasing(void)
{
  ToolkitTestApplication application;

  // Get encoded raw-buffer image and generate url
  EncodedImageBuffer buffer = ConvertFileToEncodedImageBuffer(gImage_600_RGB);
  ImageUrl           url    = Toolkit::Image::GenerateUrl(buffer);
  ImageUrl           url2   = Toolkit::Image::GenerateUrl(buffer);

  // Generate url is not equal to url2
  // NOTE : This behavior may changed when ImageUrl compare operator changed.
  DALI_TEST_CHECK(url != url2);
  // Generate url's string is equal to url2's string
  DALI_TEST_CHECK(url.GetUrl() == url2.GetUrl());

  EncodedImageBuffer buffer2 = ConvertFileToEncodedImageBuffer(gImage_600_RGB);
  url2 = Toolkit::Image::GenerateUrl(buffer2);

  // Check whethere two url are not equal
  DALI_TEST_CHECK(url.GetUrl() != url2.GetUrl());

  // Async loading, automatic atlasing for small size image
  TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
  callStack.Reset();
  callStack.Enable(true);

  Property::Map imageMap;

  imageMap[ ImageVisual::Property::URL ] = url.GetUrl();
  imageMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 34;
  imageMap[ ImageVisual::Property::DESIRED_WIDTH ] = 34;
  imageMap[ ImageVisual::Property::ATLASING] = true;

  ImageView imageView = ImageView::New();
  imageView.SetProperty( ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Toolkit::Control::Property::PADDING, Extents( 10u, 10u, 10u, 10u ) );

  // By default, Aysnc loading is used
  // loading is not started if the actor is offScene

  application.GetScene().Add( imageView );
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  // Change url to url2
  imageMap[ ImageVisual::Property::URL ] = url2.GetUrl();
  imageView.SetProperty( ImageView::Property::IMAGE, imageMap );

  imageView.SetProperty( Dali::Actor::Property::LAYOUT_DIRECTION,  Dali::LayoutDirection::RIGHT_TO_LEFT );
  application.SendNotification();
  application.Render(16);
  application.Render(16);
  application.SendNotification();

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);

  callStack.Enable(false);

  TraceCallStack::NamedParams params;
  params["width"] << 34;
  params["height"] << 34;
  DALI_TEST_EQUALS( callStack.FindMethodAndParams( "TexSubImage2D", params ), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSyncLoadingEncodedBuffer(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing sync loading from EncodedImageBuffer");

  // Get encoded raw-buffer image and generate url
  EncodedImageBuffer buffer = ConvertFileToEncodedImageBuffer(gImage_34_RGBA);
  ImageUrl           url    = Toolkit::Image::GenerateUrl(buffer);

  // Sync loading, automatic atlasing for small size image
  {
    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
    callStack.Reset();
    callStack.Enable(true);

    ImageView imageView = ImageView::New( );

    // Sync loading is used
    Property::Map syncLoadingMap;
    syncLoadingMap[ "url" ] = url.GetUrl();
    syncLoadingMap[ "desiredHeight" ] = 34;
    syncLoadingMap[ "desiredWidth" ] = 34;
    syncLoadingMap[ "synchronousLoading" ] = true;
    syncLoadingMap[ "atlasing" ] = true;
    imageView.SetProperty( ImageView::Property::IMAGE, syncLoadingMap );

    application.GetScene().Add( imageView );
    application.SendNotification();
    application.Render(16);

    TraceCallStack::NamedParams params;
    params["width"] << 34;
    params["height"] << 34;
    DALI_TEST_EQUALS( callStack.FindMethodAndParams( "TexSubImage2D", params ),
                      true, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliImageViewAddedTexture(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing image view with texture provided manager url");

  ImageView imageView = ImageView::New();

  // empty texture is ok, though pointless from app point of view
  TextureSet  empty;
  std::string url = TextureManager::AddTexture(empty);
  DALI_TEST_CHECK(url.size() > 0u);

  Property::Map propertyMap;
  propertyMap[ImageVisual::Property::URL] = url;
  imageView.SetProperty(ImageView::Property::IMAGE, propertyMap);

  application.GetScene().Add( imageView );
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliImageViewSizeWithBackground(void)
{
  ToolkitTestApplication application;

  int width = 100;
  int height = 200;
  ImageView imageView = ImageView::New();

  imageView.SetProperty( Control::Property::BACKGROUND,
                         {
                           { Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE },
                           { Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg" },
                           { ImageVisual::Property::DESIRED_WIDTH, width },
                           { ImageVisual::Property::DESIRED_HEIGHT, height },
                         }
                       );

  application.GetScene().Add( imageView );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ).width, (float)width, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ).height, (float)height, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSizeWithBackgroundAndImage(void)
{
  ToolkitTestApplication application;

  int widthBackground = 100;
  int heightBackground = 200;
  int width = 600;
  int height = 600;

  ImageView imageView = ImageView::New();

  imageView.SetProperty( Control::Property::BACKGROUND,
                         {
                           { Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE },
                           { Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg" },
                           { ImageVisual::Property::DESIRED_WIDTH, widthBackground },
                           { ImageVisual::Property::DESIRED_HEIGHT, heightBackground },
                          }
                       );

  imageView.SetImage( gImage_600_RGB ); // 1 to 1 ratio, 600x600 pixels

  application.GetScene().Add( imageView );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ).width, (float)width, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetCurrentProperty< Vector3 >( Actor::Property::SIZE ).height, (float)height, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewHeightForWidthBackground(void)
{
  ToolkitTestApplication application;

  int widthBackground = 100;
  int heightBackground = 200;

  ImageView imageView = ImageView::New();

  imageView.SetProperty( Control::Property::BACKGROUND,
                         {
                           { Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE },
                           { Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg" },
                           { ImageVisual::Property::DESIRED_WIDTH, widthBackground },
                           { ImageVisual::Property::DESIRED_HEIGHT, heightBackground }
                         }
                       );

  application.GetScene().Add( imageView );
  application.SendNotification();
  application.Render();

  Control control = Control::DownCast( imageView );
  DALI_TEST_CHECK( control );
  DALI_TEST_EQUALS( imageView.GetHeightForWidth( 123.f ), control.GetHeightForWidth( 123.f ), TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetWidthForHeight( 321.f ), control.GetWidthForHeight( 321.f ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewHeightForWidthBackgroundAndImage(void)
{
  ToolkitTestApplication application;

  int widthBackground = 100;
  int heightBackground = 200;
  int width = 300;
  int height = 300;

  ImageView imageView = ImageView::New();

  imageView.SetProperty( Control::Property::BACKGROUND,
                         {
                           { Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE },
                           { Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg" },
                           { ImageVisual::Property::DESIRED_WIDTH, widthBackground },
                           { ImageVisual::Property::DESIRED_HEIGHT, heightBackground }
                         }
                       ); // 1 to 2 ratio

  imageView.SetImage( gImage_600_RGB ); // 1 to 1 ratio

  application.GetScene().Add( imageView );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetHeightForWidth( width ), (float)height, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetWidthForHeight( height ), (float)width, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetImageUrl(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  imageView.SetImage( TEST_IMAGE_FILE_NAME );
  TestUrl( imageView, TEST_IMAGE_FILE_NAME );


  imageView.SetImage( TEST_IMAGE_FILE_NAME2 );
  TestUrl( imageView, TEST_IMAGE_FILE_NAME2 );

  END_TEST;
}

bool gResourceReadySignalFired = false;
Vector3 gNaturalSize;

void ResourceReadySignal( Control control )
{
  gResourceReadySignalFired = true;
}

int UtcDaliImageViewCheckResourceReady(void)
{
  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  // Check ImageView with background and main image, to ensure both visuals are marked as loaded
  ImageView imageView = ImageView::New( TEST_GIF_FILE_NAME );

  imageView.SetProperty( Control::Property::BACKGROUND,
                         {
                           { Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE },
                           { Toolkit::ImageVisual::Property::URL, TEST_RESOURCE_DIR "/gallery-small-1.jpg" },
                           { ImageVisual::Property::DESIRED_WIDTH, 100 },
                           { ImageVisual::Property::DESIRED_HEIGHT, 200 }
                          }
                       );

  DALI_TEST_EQUALS( imageView.IsResourceReady(), false, TEST_LOCATION );

  imageView.ResourceReadySignal().Connect( &ResourceReadySignal);

  application.GetScene().Add( imageView );

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 2 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetImageTypeChangesP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( imageView );

  application.GetScene().Add( imageView );

  std::string url;
  Property::Map map;
  Toolkit::Visual::Base visual;

  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  visual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );

  application.SendNotification();
  application.Render( 16 );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  value.Get( map );
  DALI_TEST_CHECK( map.Empty() );        // Value should be empty
  DALI_TEST_CHECK( ! visual );           // Visual should be invalid

  // Set a URL
  imageView.SetImage( "TEST_URL" );

  application.SendNotification();
  application.Render( 16 );

  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  visual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );

  DALI_TEST_CHECK( value.Get( url ) );   // Value should NOT be empty
  DALI_TEST_CHECK( ! value.Get( map ) ); // Value should be empty
  DALI_TEST_CHECK( visual );             // Visual should be valid

  // Set an empty URL
  imageView.SetImage( "" );

  application.SendNotification();
  application.Render( 16 );

  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  visual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  value.Get( map );
  DALI_TEST_CHECK( map.Empty() );        // Value should be empty
  DALI_TEST_CHECK( ! visual );           // Visual should be invalid

  // Set a URL in property map
  Property::Map propertyMap;
  propertyMap[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;
  imageView.SetProperty( ImageView::Property::IMAGE, propertyMap );

  application.SendNotification();
  application.Render( 16 );

  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  visual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( value.Get( map ) );   // Value should NOT be empty
  DALI_TEST_CHECK( visual );             // Visual should be valid

  // Set a URL in property map again
  propertyMap[ImageVisual::Property::URL] = gImage_34_RGBA;
  imageView.SetProperty( ImageView::Property::IMAGE, propertyMap );

  application.SendNotification();
  application.Render( 16 );

  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  visual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( value.Get( map ) );   // Value should NOT be empty
  DALI_TEST_CHECK( visual );             // Visual should be valid

  // Set an empty URL in property map
  propertyMap[ImageVisual::Property::URL] = std::string();
  imageView.SetProperty( ImageView::Property::IMAGE, propertyMap );

  application.SendNotification();
  application.Render( 16 );

  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  visual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( value.Get( map ) );   // Value should NOT be empty
  DALI_TEST_CHECK( ! visual );           // Visual should be invalid

  END_TEST;
}

int UtcDaliImageViewResourceUrlP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  DALI_TEST_CHECK( imageView.GetProperty( ImageView::Property::IMAGE ).Get< std::string >().empty() );

  imageView.SetProperty( ImageView::Property::IMAGE, "TestString" );
  DALI_TEST_EQUALS( imageView.GetProperty( ImageView::Property::IMAGE ).Get< std::string >(), "TestString", TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewReplaceImage(void)
{
  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  // Check ImageView with background and main image, to ensure both visuals are marked as loaded
  ImageView imageView = ImageView::New( TEST_IMAGE_1 );

  DALI_TEST_EQUALS( imageView.IsResourceReady(), false, TEST_LOCATION );

  imageView.ResourceReadySignal().Connect( &ResourceReadySignal);

  application.GetScene().Add( imageView );

  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  DALI_TEST_EQUALS( imageView.GetRendererCount(), 1u, TEST_LOCATION );

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  gResourceReadySignalFired = false;

  imageView.SetImage(TEST_IMAGE_2);

  application.SendNotification();
  application.Render(16);

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  DALI_TEST_EQUALS( imageView.GetRendererCount(), 1u, TEST_LOCATION );

  DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  END_TEST;
}

void OnRelayoutOverride( Size size )
{
  gNaturalSize = size; // Size Relayout is using
}

int UtcDaliImageViewReplaceImageAndGetNaturalSize(void)
{
  ToolkitTestApplication application;

  // Check ImageView with background and main image, to ensure both visuals are marked as loaded
  ImageView imageView = ImageView::New( TEST_IMAGE_1 );
  imageView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

  DummyControl dummyControl = DummyControl::New( true );
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());
  dummyControl.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::ALL_DIMENSIONS );

  dummyControl.Add( imageView );
  dummyImpl.SetRelayoutCallback( &OnRelayoutOverride );
  application.GetScene().Add( dummyControl );

  application.SendNotification();
  application.Render();

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  DALI_TEST_EQUALS( gNaturalSize.width, 1024.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( gNaturalSize.height, 1024.0f, TEST_LOCATION );

  gNaturalSize = Vector3::ZERO;

  imageView.SetImage(gImage_600_RGB);

  // Waiting for resourceReady so SendNotifcation not called here.

  // loading started, this waits for the loader thread for max 30 seconds
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( gNaturalSize.width, 600.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( gNaturalSize.height, 600.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewResourceReadySignalWithImmediateLoad(void)
{
  tet_infoline("Test Setting Image with IMMEDIATE load and receving ResourceReadySignal before staged.");

  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  Property::Map imageMap;

  imageMap[ ImageVisual::Property::URL ] = gImage_34_RGBA;
  imageMap[ ImageVisual::Property::LOAD_POLICY ] =  ImageVisual::LoadPolicy::IMMEDIATE;

  tet_infoline("Creating ImageView without URL so image does not start loading");
  ImageView imageView = ImageView::New();
  tet_infoline("Connect to image loaded signal before setting image");
  imageView.ResourceReadySignal().Connect( &ResourceReadySignal);
  tet_infoline("Setting Image with IMMEDIATE load, signal already connected so will be triggered.");
  imageView.SetProperty( ImageView::Property::IMAGE, imageMap );

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewResourceReadySignalWithReusedImage(void)
{
  tet_infoline("Test Setting Image that was already loaded by another ImageView and still getting ResourceReadySignal.");

  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  Property::Map imageMap;

  imageMap[ ImageVisual::Property::URL ] = gImage_34_RGBA;
  imageMap[ ImageVisual::Property::LOAD_POLICY ] =  ImageVisual::LoadPolicy::IMMEDIATE;

  ImageView imageView = ImageView::New();
  imageView.ResourceReadySignal().Connect( &ResourceReadySignal);
  imageView.SetProperty( ImageView::Property::IMAGE, imageMap );

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );
  gResourceReadySignalFired = false;

  ImageView imageViewWithExistingImage = ImageView::New();
  imageViewWithExistingImage.ResourceReadySignal().Connect( &ResourceReadySignal);
  imageViewWithExistingImage.SetProperty( ImageView::Property::IMAGE, imageMap );

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewResourceReadySignalWithReusedImage02(void)
{
  tet_infoline("Test Setting Image that was already loaded by another ImageView and still getting ResourceReadySignal when staged.");

  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  Property::Map imageImmediateLoadingMap;
  imageImmediateLoadingMap[ ImageVisual::Property::URL ] = gImage_34_RGBA;
  imageImmediateLoadingMap[ ImageVisual::Property::LOAD_POLICY ] =  ImageVisual::LoadPolicy::IMMEDIATE;

  tet_infoline("Immediate load an image");
  ImageView imageView = ImageView::New();
  imageView.ResourceReadySignal().Connect( &ResourceReadySignal);
  imageView.SetProperty( ImageView::Property::IMAGE, imageImmediateLoadingMap );

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);

  tet_infoline("Check image loaded");
  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );
  gResourceReadySignalFired = false;

  tet_infoline("Create another ImageView with the same URL");
  ImageView imageViewWithExistingImage = ImageView::New( gImage_34_RGBA );
  tet_infoline("Connect to ResourceReady signal for second ImageView, it should still fire as resource is ready");
  imageViewWithExistingImage.ResourceReadySignal().Connect( &ResourceReadySignal);

  application.GetScene().Add( imageViewWithExistingImage );

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewPaddingProperty(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imagePropertyMap[ Toolkit::ImageVisual::Property::URL ] = TEST_RESOURCE_DIR "/gallery-small-1.jpg" ;
  imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = 128;
  imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 128;
  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  imageView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  imageView.SetProperty( Control::Property::PADDING, Extents( 15, 10, 5, 10 ) );
  application.GetScene().Add( imageView );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetProperty<Extents>( Control::Property::PADDING ), Extents( 15, 10, 5, 10 ), TEST_LOCATION );

  ImageView childImage = ImageView::New();
  childImage.SetBackgroundColor( Color::BLACK );
  childImage.SetProperty( Actor::Property::SIZE, Vector2( 10.f, 10.f ) );
  imageView.Add( childImage );

  application.SendNotification();
  application.Render();

  // Child ImageView should be positioned dependinig on Parent ImageView's Padding value
  DALI_TEST_EQUALS( childImage.GetProperty<Vector3>( Dali::Actor::Property::POSITION ), Vector3( 15, 5, 0 ), TEST_LOCATION );

  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( imageView );
  Toolkit::Visual::Base imageVisual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );
  Property::Map resultMap;
  imageVisual.CreatePropertyMap( resultMap );

  Property::Value* transformValue = resultMap.Find( Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( transformValue );
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK( retMap );

  // Image Visual should be positioned depending on ImageView's padding
  DALI_TEST_EQUALS( retMap->Find( Visual::Transform::Property::OFFSET )->Get< Vector2 >(), Vector2( 15, 5 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewPaddingProperty02(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imagePropertyMap[ Toolkit::ImageVisual::Property::URL ] = TEST_RESOURCE_DIR "/Kid1.svg" ;
  imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = 128;
  imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 128;
  imagePropertyMap[ DevelVisual::Property::VISUAL_FITTING_MODE ] = Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO;
  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  imageView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  imageView.SetProperty( Control::Property::PADDING, Extents( 15, 10, 5, 10 ) );
  application.GetScene().Add( imageView );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetProperty<Extents>( Control::Property::PADDING ), Extents( 15, 10, 5, 10 ), TEST_LOCATION );

  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( imageView );
  Toolkit::Visual::Base imageVisual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );
  Property::Map resultMap;
  imageVisual.CreatePropertyMap( resultMap );

  Property::Value* transformValue = resultMap.Find( Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( transformValue );
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK( retMap );

  // Image Visual should be positioned depending on ImageView's padding
  DALI_TEST_EQUALS( retMap->Find( Visual::Transform::Property::OFFSET )->Get< Vector2 >(), Vector2( 15, 5 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewPaddingProperty03(void)
{
  tet_infoline("Test Setting Image Padding then removing it.");

  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imagePropertyMap[ Toolkit::ImageVisual::Property::URL ] = TEST_RESOURCE_DIR "/Kid1.svg" ;
  imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = 128;
  imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 128;
  imagePropertyMap[ DevelVisual::Property::VISUAL_FITTING_MODE ] = Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO;
  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  imageView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  imageView.SetProperty( Control::Property::PADDING, Extents( 15, 10, 5, 10 ) );
  application.GetScene().Add( imageView );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetProperty<Extents>( Control::Property::PADDING ), Extents( 15, 10, 5, 10 ), TEST_LOCATION );

  tet_infoline("Remove Padding and test Visual is position correctly");

  imageView.SetProperty( Control::Property::PADDING, Extents( 0, 0, 0, 0 ) );

  application.SendNotification();
  application.Render();

  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( imageView );
  Toolkit::Visual::Base imageVisual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );
  Property::Map resultMap;
  imageVisual.CreatePropertyMap( resultMap );

  Property::Value* transformValue = resultMap.Find( Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( transformValue );
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK( retMap );

  // Image Visual should be positioned depending on ImageView's padding
  DALI_TEST_EQUALS( retMap->Find( Visual::Transform::Property::OFFSET )->Get< Vector2 >(), Vector2( 0, 0 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewPaddingProperty04(void)
{
  tet_infoline("Test Setting Image Padding then removing it. Visual Fitting Mode as Fill");

  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imagePropertyMap[ Toolkit::ImageVisual::Property::URL ] = TEST_RESOURCE_DIR "/Kid1.svg" ;
  imagePropertyMap[ ImageVisual::Property::DESIRED_WIDTH ] = 128;
  imagePropertyMap[ ImageVisual::Property::DESIRED_HEIGHT ] = 128;
  imagePropertyMap[ DevelVisual::Property::VISUAL_FITTING_MODE ] = Toolkit::DevelVisual::FILL;
  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  imageView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  imageView.SetProperty( Control::Property::PADDING, Extents( 15, 10, 5, 10 ) );
  application.GetScene().Add( imageView );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetProperty<Extents>( Control::Property::PADDING ), Extents( 15, 10, 5, 10 ), TEST_LOCATION );

  tet_infoline("Remove Padding and test Visual is position correctly");

  imageView.SetProperty( Control::Property::PADDING, Extents( 0, 0, 0, 0 ) );

  application.SendNotification();
  application.Render();

  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( imageView );
  Toolkit::Visual::Base imageVisual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );
  Property::Map resultMap;
  imageVisual.CreatePropertyMap( resultMap );

  Property::Value* transformValue = resultMap.Find( Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( transformValue );
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK( retMap );

  // Image Visual should be positioned depending on ImageView's padding
  DALI_TEST_EQUALS( retMap->Find( Visual::Transform::Property::OFFSET )->Get< Vector2 >(), Vector2( 0, 0 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewTransformTest01(void)
{
  tet_infoline("Test Setting a offset transform on the ImageView");

  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  Property::Map imagePropertyMap;
  imagePropertyMap.Add( Toolkit::Visual::Property::TYPE,Toolkit::Visual::IMAGE )
                  .Add( Toolkit::ImageVisual::Property::URL,TEST_RESOURCE_DIR "/Kid1.svg" )
                  .Add( ImageVisual::Property::DESIRED_WIDTH,120 )
                  .Add( ImageVisual::Property::DESIRED_HEIGHT,120 )
                  .Add( DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FILL )
                  .Add( Visual::Property::TRANSFORM,
                        Property::Map().Add( Toolkit::Visual::Transform::Property::OFFSET_POLICY,
                                             Vector2( Visual::Transform::Policy::ABSOLUTE, Visual::Transform::Policy::ABSOLUTE ) )
                                       .Add( Toolkit::Visual::Transform::Property::OFFSET, Vector2( 8, 8 ) ) );

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE , imagePropertyMap );
  imageView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  imageView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  application.GetScene().Add( imageView );

  application.SendNotification();
  application.Render();

  // Check whether Image Visual transforms on ImageVieiw::OnRelayout()
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( imageView );
  Toolkit::Visual::Base imageVisual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );
  Property::Map resultMap;
  imageVisual.CreatePropertyMap( resultMap );

  Property::Value* transformValue = resultMap.Find( Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( transformValue );
  Property::Map* retMap = transformValue->GetMap();
  DALI_TEST_CHECK( retMap );

  // Image Visual should be positioned depending on ImageView's padding
  DALI_TEST_EQUALS( retMap->Find( Visual::Transform::Property::OFFSET )->Get< Vector2 >(), Vector2( 8, 8 ), TEST_LOCATION );
  DALI_TEST_EQUALS( retMap->Find( Visual::Transform::Property::OFFSET_POLICY )->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewUsingAtlasAndGetNaturalSize(void)
{
  ToolkitTestApplication application;

  // Check ImageView with background and main image, to ensure both visuals are marked as loaded
  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imageMap[ Toolkit::ImageVisual::Property::URL ] = gImage_34_RGBA;
  imageMap[ Toolkit::ImageVisual::Property::ATLASING ] = true;
  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Vector3 naturalSize = imageView.GetNaturalSize();

  DALI_TEST_EQUALS( naturalSize.width, 34.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( naturalSize.height, 34.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewFillMode(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual without padding and set the fill-mode to fill" );

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, gImage_600_RGB );
  imageMap.Add( DevelVisual::Property::VISUAL_FITTING_MODE, DevelVisual::FittingMode::FILL );

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2::ONE, TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == Toolkit::Visual::Transform::Policy::RELATIVE );

  END_TEST;
}

int UtcDaliImageViewFittingModeFitKeepAspectRatio(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual using FitKeepAspectRatio ( image: [600,600], view: [600,700] )" );
  tet_infoline( "  There should be need to change the transform, offset is adjusted to fit inside");

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, gImage_600_RGB ); // 600x600 image
  imageMap.Add( DevelVisual::Property::VISUAL_FITTING_MODE , Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO );

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(600,700) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  // If there's
  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 600, 600 ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 0, 50 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewFittingModesFill(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual using Fill ( image: [600,600], view: [600,700] )" );
  tet_infoline( "  There should be no need to change the transform, only size is changed to fit view");

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, gImage_600_RGB ); // 600x600 image
  imageMap.Add( DevelVisual::Property::VISUAL_FITTING_MODE , Toolkit::DevelVisual::FILL );

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(600,700) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2::ONE, TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == Toolkit::Visual::Transform::Policy::RELATIVE );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 0, 0 ), TEST_LOCATION ); // OFFSET is zero

  END_TEST;
}

int UtcDaliImageViewFittingModesOverfitKeepAspectRatio(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual using OverFitKeepAspectRatio ( image: [600,600], view: [600,500] )" );
  tet_infoline( "  offset or size is the same as view, but adjust internally using pixelArea ");

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, gImage_600_RGB ); // 600x600 image
  imageMap.Add( DevelVisual::Property::VISUAL_FITTING_MODE , Toolkit::DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO );

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(600,500) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  // If there's
  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 600, 500 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 0, 0 ), TEST_LOCATION ); // OFFSET is zero

  END_TEST;
}

int UtcDaliImageViewFittingModesCenter01(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual using Center ( image: [600,600], view: [700,700] )" );
  tet_infoline( "  There should be need to change the transform, offset is adjusted to fit inside");

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, gImage_600_RGB ); // 600x600 image
  imageMap.Add( DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::CENTER);

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(700,700) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 600, 600 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 50, 50 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewFittingModesCenter02(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual using Center ( image: [600,600], view: [500,500] )" );
  tet_infoline( "  There should be need to change the transform, offset is adjusted to fit inside");

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, gImage_600_RGB ); // 600x600 image
  imageMap.Add( DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::CENTER);

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(700,700) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 600, 600 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 50, 50 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewFittingModesFitHeight01(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual using FitHeight ( image: [600,600], view: [600,700] )" );

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, gImage_600_RGB ); // 600x600 image
  imageMap.Add( DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FIT_HEIGHT);

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(600,700) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 600, 700 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 0, 0 ), TEST_LOCATION ); // OFFSET is zero

  END_TEST;
}

int UtcDaliImageViewFittingModesFitHeight02(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual using FitHeight ( image: [600,600], view: [700,600] )" );

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, gImage_600_RGB ); // 249x169 image
  imageMap.Add( DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FIT_HEIGHT);

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(700,600) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 600, 600 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 50, 0 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewFittingModesFitWidth01(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual using FitWidth ( image: [600,600], view: [600,700] )" );

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, gImage_600_RGB ); // 600x600 image
  imageMap.Add( DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FIT_WIDTH);

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(600,700) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 600, 600 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 0, 50 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewFittingModesFitWidth02(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual using FitWidth ( image: [600,600], view:[700,600] )" );

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, gImage_600_RGB ); // 249x169 image
  imageMap.Add( DevelVisual::Property::VISUAL_FITTING_MODE, Toolkit::DevelVisual::FIT_WIDTH);

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(700,600) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 700, 600 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 0, 0 ), TEST_LOCATION ); // OFFSET is zero

  END_TEST;
}

int UtcDaliImageViewFittingModesChangeFittingMode01(void)
{
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliImageViewFittingModesChangeFittingMode, image: [600,600], view:[800,700]" );

  ImageView imageView = ImageView::New();

  // 1. Render using FittingMode::SHRINK_TO_FIT
  Property::Map imageMap;
  imageMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imageMap[ Toolkit::ImageVisual::Property::URL ] = gImage_600_RGB;
  imageMap[ DevelVisual::Property::VISUAL_FITTING_MODE ] =  Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO;

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(800,700) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 700, 700 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 50, 0 ), TEST_LOCATION );

  // 2. Render again using DevelVisaul::CENTER
  Property::Map imageMap2;
  imageMap2[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imageMap2[ Toolkit::ImageVisual::Property::URL ] = gImage_600_RGB;
  imageMap2[ DevelVisual::Property::VISUAL_FITTING_MODE ] = Toolkit::DevelVisual::CENTER;

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap2 );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(800,700) );

  application.GetScene().Add( imageView );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  returnedMap.Clear();
  visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );

  visual.CreatePropertyMap( returnedMap );

  value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 600, 600 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 100, 50 ), TEST_LOCATION );

  // 3. Render again using before fittingMode
  Property::Map imageMap3;
  imageMap3[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imageMap3[ Toolkit::ImageVisual::Property::URL ] = gImage_600_RGB;
  imageMap3[ DevelVisual::Property::VISUAL_FITTING_MODE ] =  Toolkit::DevelVisual::FIT_KEEP_ASPECT_RATIO;

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap3 );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(800,700) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  returnedMap.Clear();
  visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  visual.CreatePropertyMap( returnedMap );

  value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 700, 700 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 50, 0 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewFittingModesChangeFittingMode02(void)
{
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliImageViewFittingModesChangeFittingMode, image: [600,600], view:[800,700]" );

  ImageView imageView = ImageView::New();

  // 1. Render using FittingMode::OVER_FIT_KEEP_ASPECT_RATIO
  Property::Map imageMap;
  imageMap[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imageMap[ Toolkit::ImageVisual::Property::URL ] = gImage_600_RGB;
  imageMap[ DevelVisual::Property::VISUAL_FITTING_MODE ] =  Toolkit::DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO;

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(800,700) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 800, 700 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 0, 0 ), TEST_LOCATION );

  // 2. Render again using DevelVisaul::CENTER
  Property::Map imageMap2;
  imageMap2[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imageMap2[ Toolkit::ImageVisual::Property::URL ] = gImage_600_RGB;
  imageMap2[ DevelVisual::Property::VISUAL_FITTING_MODE ] = Toolkit::DevelVisual::CENTER;

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap2 );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(800,700) );

  application.GetScene().Add( imageView );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  returnedMap.Clear();
  visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );

  visual.CreatePropertyMap( returnedMap );

  value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 600, 600 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 100, 50 ), TEST_LOCATION );

  // 3. Render again using before fittingMode
  Property::Map imageMap3;
  imageMap3[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::IMAGE;
  imageMap3[ Toolkit::ImageVisual::Property::URL ] = gImage_600_RGB;
  imageMap3[ DevelVisual::Property::VISUAL_FITTING_MODE ] =  Toolkit::DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO;

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap3 );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(800,700) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  returnedMap.Clear();
  visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  visual.CreatePropertyMap( returnedMap );

  value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 800, 700 ), TEST_LOCATION ); // Change the internal size according to the image view size

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( Toolkit::Visual::Transform::Policy::ABSOLUTE, Toolkit::Visual::Transform::Policy::ABSOLUTE ), TEST_LOCATION );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 0, 0 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewFittingModesWithAnimatedVectorImageVisual(void)
{
  ToolkitTestApplication application;

  tet_infoline( "Create an ImageVisual using SCALE_TO_FILL and animated vector image ( image: [600,600], view:[600,600] )" );

  ImageView imageView = ImageView::New();
  Property::Map imageMap;
  imageMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE );
  imageMap.Add( Toolkit::ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME ); // 249x169 image

  imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, imageMap );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(600,600) );

  application.GetScene().Add( imageView );

  // Trigger a potential relayout
  application.SendNotification();
  application.Render();

  Toolkit::Visual::Base visual = DevelControl::GetVisual( Toolkit::Internal::GetImplementation( imageView ), Toolkit::ImageView::Property::IMAGE );
  Property::Map returnedMap;
  visual.CreatePropertyMap( returnedMap );

  Property::Value* value = returnedMap.Find( Toolkit::Visual::Property::TRANSFORM );
  DALI_TEST_CHECK( value );
  Property::Map* map = value->GetMap();
  DALI_TEST_CHECK( map );

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2::ONE, TEST_LOCATION ); // Relative size so will take up 100%

  value = map->Find( Toolkit::Visual::Transform::Property::SIZE_POLICY );
  DALI_TEST_CHECK( value );
  DALI_TEST_CHECK( value->Get< int >() == Toolkit::Visual::Transform::Policy::RELATIVE );

  value = map->Find( Toolkit::Visual::Transform::Property::OFFSET );
  DALI_TEST_CHECK( value );
  DALI_TEST_EQUALS( value->Get< Vector2 >(), Vector2( 0, 0 ), TEST_LOCATION ); // OFFSET is zero

  END_TEST;
}


int UtcDaliImageViewCustomShader(void)
{
  ToolkitTestApplication application;

  // Set a custom shader with an image url
  {
    Property::Map properties;
    Property::Map shader;
    const std::string vertexShader = "Foobar";
    const std::string fragmentShader = "Foobar";
    shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
    shader[Visual::Shader::Property::VERTEX_SHADER] = vertexShader;

    properties[Visual::Property::TYPE] = Visual::IMAGE;
    properties[Visual::Property::SHADER] = shader;
    properties[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;

    ImageView imageView = ImageView::New();
    imageView.SetProperty( ImageView::Property::IMAGE, properties );

    application.GetScene().Add( imageView );

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

    Renderer renderer = imageView.GetRendererAt( 0 );
    Shader shader2 = renderer.GetShader();
    Property::Value value = shader2.GetProperty( Shader::Property::PROGRAM );
    Property::Map* map = value.GetMap();
    DALI_TEST_CHECK( map );

    Property::Value* fragment = map->Find( "fragment" ); // fragment key name from shader-impl.cpp
    DALI_TEST_EQUALS( fragmentShader, fragment->Get< std::string >(), TEST_LOCATION );

    Property::Value* vertex = map->Find( "vertex" ); // vertex key name from shader-impl.cpp
    DALI_TEST_EQUALS( vertexShader, vertex->Get< std::string >(), TEST_LOCATION );
  }

  // Set a custom shader after setting an image url
  {
    Property::Map properties;
    Property::Map shader;
    const std::string vertexShader = "Foobar";
    const std::string fragmentShader = "Foobar";
    shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
    shader[Visual::Shader::Property::VERTEX_SHADER] = vertexShader;

    properties[Visual::Property::SHADER] = shader;

    ImageView imageView = ImageView::New( TEST_IMAGE_FILE_NAME );
    imageView.SetProperty( ImageView::Property::IMAGE, properties );

    application.GetScene().Add( imageView );

    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

    Renderer renderer = imageView.GetRendererAt( 0 );
    Shader shader2 = renderer.GetShader();
    Property::Value value = shader2.GetProperty( Shader::Property::PROGRAM );
    Property::Map* map = value.GetMap();
    DALI_TEST_CHECK( map );

    Property::Value* fragment = map->Find( "fragment" ); // fragment key name from shader-impl.cpp
    DALI_TEST_EQUALS( fragmentShader, fragment->Get< std::string >(), TEST_LOCATION );

    Property::Value* vertex = map->Find( "vertex" ); // vertex key name from shader-impl.cpp
    DALI_TEST_EQUALS( vertexShader, vertex->Get< std::string >(), TEST_LOCATION );
  }

  // Set a custom shader before setting an image url
  {
    Property::Map properties;
    Property::Map shader;
    const std::string vertexShader = "Foobar";
    const std::string fragmentShader = "Foobar";
    shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
    shader[Visual::Shader::Property::VERTEX_SHADER] = vertexShader;

    properties[Visual::Property::SHADER] = shader;

    ImageView imageView = ImageView::New();
    imageView.SetProperty( ImageView::Property::IMAGE, properties );
    imageView.SetProperty( ImageView::Property::IMAGE, TEST_IMAGE_FILE_NAME );

    application.GetScene().Add( imageView );

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

    Renderer renderer = imageView.GetRendererAt( 0 );
    Shader shader2 = renderer.GetShader();
    Property::Value value = shader2.GetProperty( Shader::Property::PROGRAM );
    Property::Map* map = value.GetMap();
    DALI_TEST_CHECK( map );

    Property::Value* fragment = map->Find( "fragment" ); // fragment key name from shader-impl.cpp
    DALI_TEST_EQUALS( fragmentShader, fragment->Get< std::string >(), TEST_LOCATION );

    Property::Value* vertex = map->Find( "vertex" ); // vertex key name from shader-impl.cpp
    DALI_TEST_EQUALS( vertexShader, vertex->Get< std::string >(), TEST_LOCATION );
  }

  // Set a custom shader after setting a property map
  {
    Property::Map properties;
    Property::Map shader;
    const std::string vertexShader = "Foobar";
    const std::string fragmentShader = "Foobar";
    shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
    shader[Visual::Shader::Property::VERTEX_SHADER] = vertexShader;

    properties[Visual::Property::SHADER] = shader;

    Property::Map properties1;
    properties1[Visual::Property::TYPE] = Visual::IMAGE;
    properties1[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;

    ImageView imageView = ImageView::New();
    imageView.SetProperty( ImageView::Property::IMAGE, properties1 );
    imageView.SetProperty( ImageView::Property::IMAGE, properties );

    application.GetScene().Add( imageView );

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

    Renderer renderer = imageView.GetRendererAt( 0 );
    Shader shader2 = renderer.GetShader();
    Property::Value value = shader2.GetProperty( Shader::Property::PROGRAM );
    Property::Map* map = value.GetMap();
    DALI_TEST_CHECK( map );

    Property::Value* fragment = map->Find( "fragment" ); // fragment key name from shader-impl.cpp
    DALI_TEST_EQUALS( fragmentShader, fragment->Get< std::string >(), TEST_LOCATION );

    Property::Value* vertex = map->Find( "vertex" ); // vertex key name from shader-impl.cpp
    DALI_TEST_EQUALS( vertexShader, vertex->Get< std::string >(), TEST_LOCATION );
  }

  // Set a custom shader before setting a property map
  {
    Property::Map properties;
    Property::Map shader;
    const std::string vertexShader = "Foobar";
    const std::string fragmentShader = "Foobar";
    shader[Visual::Shader::Property::FRAGMENT_SHADER] = fragmentShader;
    shader[Visual::Shader::Property::VERTEX_SHADER] = vertexShader;

    properties[Visual::Property::SHADER] = shader;

    Property::Map properties1;
    properties1[Visual::Property::TYPE] = Visual::IMAGE;
    properties1[ImageVisual::Property::URL] = TEST_IMAGE_FILE_NAME;

    ImageView imageView = ImageView::New();
    imageView.SetProperty( ImageView::Property::IMAGE, properties );
    imageView.SetProperty( ImageView::Property::IMAGE, properties1 );

    application.GetScene().Add( imageView );

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

    Renderer renderer = imageView.GetRendererAt( 0 );
    Shader shader2 = renderer.GetShader();
    Property::Value value = shader2.GetProperty( Shader::Property::PROGRAM );
    Property::Map* map = value.GetMap();
    DALI_TEST_CHECK( map );

    Property::Value* fragment = map->Find( "fragment" ); // fragment key name from shader-impl.cpp
    DALI_TEST_EQUALS( fragmentShader, fragment->Get< std::string >(), TEST_LOCATION );

    Property::Value* vertex = map->Find( "vertex" ); // vertex key name from shader-impl.cpp
    DALI_TEST_EQUALS( vertexShader, vertex->Get< std::string >(), TEST_LOCATION );
  }

  END_TEST;
}


namespace
{
static int gFailCounter = 0;
const int MAX_RETRIES(3);

void ReloadImage(ImageView imageView)
{
  Property::Map imageImmediateLoadingMap;
  imageImmediateLoadingMap[ ImageVisual::Property::URL ] = "Non-existant-image.jpg";
  imageImmediateLoadingMap[ ImageVisual::Property::LOAD_POLICY ] =  ImageVisual::LoadPolicy::IMMEDIATE;

  tet_infoline("Immediate load an image");
  imageView.SetProperty( ImageView::Property::IMAGE, imageImmediateLoadingMap );
}

void ResourceFailedReload( Control control )
{
  gFailCounter++;
  if( gFailCounter < MAX_RETRIES )
  {
    ReloadImage(ImageView::DownCast(control));
  }
}
}

int UtcDaliImageViewReloadFailedOnResourceReadySignal(void)
{
  tet_infoline("Test reloading failed image from within signal handler.");

  ToolkitTestApplication application;

  gFailCounter = 0;

  ImageView imageView = ImageView::New();
  imageView.ResourceReadySignal().Connect( &ResourceFailedReload );
  DALI_TEST_EQUALS( gFailCounter, 0, TEST_LOCATION );
  ReloadImage(imageView);

  // loading started, this waits for the loader thread to complete
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
  application.SendNotification();

  DALI_TEST_EQUALS( gFailCounter, 1, TEST_LOCATION );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
  application.SendNotification();

  DALI_TEST_EQUALS( gFailCounter, 2, TEST_LOCATION );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );
  application.SendNotification();
  DALI_TEST_EQUALS( gFailCounter, 3, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewLoadRemoteSVG(void)
{
  tet_infoline("Test load from a remote server.");

  ToolkitTestApplication application;
  Toolkit::ImageView imageView;
  imageView = Toolkit::ImageView::New();
  imageView.SetImage("https://dev.w3.org/SVG/tools/svgweb/samples/svg-files/check.svg");
  // Victor. Temporary (or permanent?) update as the url above seems not to work from time to time ...
//  imageView.SetImage("https://upload.wikimedia.org/wikipedia/commons/thumb/0/02/SVG_logo.svg/64px-SVG_logo.svg.png");
  imageView.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
  imageView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  imageView.SetProperty( Actor::Property::SIZE, Vector2(300, 300) );
  imageView.SetProperty( Actor::Property::POSITION, Vector3( 150.0f , 150.0f , 0.0f ) );

  application.GetScene().Add( imageView );

  DALI_TEST_CHECK( imageView );

  DALI_TEST_EQUALS( imageView.GetRendererCount(), 0u, TEST_LOCATION );

  application.SendNotification();

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetRendererCount(), 1u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSyncSVGLoading(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing SVG image sync loading");

  // Sync loading, automatic atlasing for small size image
  {
    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
    callStack.Reset();
    callStack.Enable(true);

    ImageView imageView = ImageView::New( );

    // Sync loading is used
    Property::Map syncLoadingMap;
    syncLoadingMap.Insert( Toolkit::Visual::Property::TYPE,  Toolkit::Visual::IMAGE );
    syncLoadingMap.Insert( Toolkit::ImageVisual::Property::URL,  TEST_RESOURCE_DIR "/svg1.svg"  );
    syncLoadingMap.Insert( Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING,  true);
    imageView.SetProperty( ImageView::Property::IMAGE, syncLoadingMap );

    application.GetScene().Add( imageView );
    DALI_TEST_CHECK( imageView );

    application.SendNotification();
    application.Render(16);
    Vector3 naturalSize = imageView.GetNaturalSize();

    DALI_TEST_EQUALS( naturalSize.width, 100.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( naturalSize.height, 100.0f, TEST_LOCATION );

  }
  END_TEST;
}

int UtcDaliImageViewAsyncSVGLoading(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing SVG image async loading");

  // Sync loading, automatic atlasing for small size image
  {
    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
    callStack.Reset();
    callStack.Enable(true);

    ImageView imageView = ImageView::New( );

    // Sync loading is used
    Property::Map syncLoadingMap;
    syncLoadingMap.Insert( Toolkit::Visual::Property::TYPE,  Toolkit::Visual::IMAGE );
    syncLoadingMap.Insert( Toolkit::ImageVisual::Property::URL,  TEST_RESOURCE_DIR "/svg1.svg"  );
    syncLoadingMap.Insert( Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING,  false);
    imageView.SetProperty( ImageView::Property::IMAGE, syncLoadingMap );

    application.GetScene().Add( imageView );
    DALI_TEST_CHECK( imageView );

    application.SendNotification();
    application.Render(16);
    Vector3 naturalSize = imageView.GetNaturalSize();

    DALI_TEST_EQUALS( naturalSize.width, 100.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( naturalSize.height, 100.0f, TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliImageViewSVGLoadingSyncSetInvalidValue(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing SVG image async loading");

  // Sync loading, automatic atlasing for small size image
  {
    TraceCallStack& callStack = application.GetGlAbstraction().GetTextureTrace();
    callStack.Reset();
    callStack.Enable(true);

    ImageView imageView = ImageView::New( );

    // Sync loading is used
    Property::Map syncLoadingMap;
    syncLoadingMap.Insert( Toolkit::Visual::Property::TYPE,  Toolkit::Visual::IMAGE );
    syncLoadingMap.Insert( Toolkit::ImageVisual::Property::URL,  TEST_RESOURCE_DIR "/svg1.svg"  );

    // Check to set invalid value
    // The SYNCHRONOUS_LOADING property must be set to the bool value.
    // Check if error log is outputted when setting other value like string.
    // Even if the wrong value is set, the image will be shown normally, and the synchronous value should be the default value(false).
    syncLoadingMap.Insert( Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING, std::to_string(5) );
    imageView.SetProperty( ImageView::Property::IMAGE, syncLoadingMap );

    application.GetScene().Add( imageView );
    DALI_TEST_CHECK( imageView );

    application.SendNotification();
    application.Render(16);
    Vector3 naturalSize = imageView.GetNaturalSize();
    DALI_TEST_EQUALS( naturalSize.width, 100.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( naturalSize.height, 100.0f, TEST_LOCATION );

    Property::Value value = imageView.GetProperty( ImageView::Property::IMAGE );
    Property::Map* map = value.GetMap();
    DALI_TEST_CHECK( map );

    Property::Value* sync = map->Find( Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING );
    DALI_TEST_CHECK( sync );
    DALI_TEST_EQUALS( false, sync->Get< bool >(), TEST_LOCATION );

  }
  END_TEST;
}

int UtcDaliImageViewSvgLoadingFailure(void)
{
  ToolkitTestApplication application;

  // Local svg file - invalid file path
  {
    gResourceReadySignalFired = false;

    ImageView imageView = ImageView::New( TEST_RESOURCE_DIR "/foo.svg" );
    imageView.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
    imageView.ResourceReadySignal().Connect( &ResourceReadySignal);

    DALI_TEST_EQUALS( imageView.IsResourceReady(), false, TEST_LOCATION );

    application.GetScene().Add( imageView );

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );
    DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( imageView.GetVisualResourceStatus( ImageView::Property::IMAGE ), Visual::ResourceStatus::FAILED, TEST_LOCATION );
  }

  // Local svg file - invalid file
  {
    gResourceReadySignalFired = false;

    ImageView imageView = ImageView::New( TEST_RESOURCE_DIR "/invalid.svg" );
    imageView.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
    imageView.ResourceReadySignal().Connect( &ResourceReadySignal);

    DALI_TEST_EQUALS( imageView.IsResourceReady(), false, TEST_LOCATION );

    application.GetScene().Add( imageView );

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );
    DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( imageView.GetVisualResourceStatus( ImageView::Property::IMAGE ), Visual::ResourceStatus::FAILED, TEST_LOCATION );
  }

  // Remote svg file
  {
    gResourceReadySignalFired = false;

    ImageView imageView = ImageView::New( "https://bar.org/foobar.svg" );
    imageView.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
    imageView.ResourceReadySignal().Connect( &ResourceReadySignal);

    DALI_TEST_EQUALS( imageView.IsResourceReady(), false, TEST_LOCATION );

    application.GetScene().Add( imageView );

    application.SendNotification();

    // loading started, this waits for the loader thread
    DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

    application.SendNotification();
    application.Render(16);

    DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );
    DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );
    DALI_TEST_EQUALS( imageView.GetVisualResourceStatus( ImageView::Property::IMAGE ), Visual::ResourceStatus::FAILED, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliImageViewSvgRasterizationFailure(void)
{
  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  ImageView imageView = ImageView::New( TEST_RESOURCE_DIR "/svg1.svg" );
  imageView.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  imageView.ResourceReadySignal().Connect( &ResourceReadySignal);

  DALI_TEST_EQUALS( imageView.IsResourceReady(), false, TEST_LOCATION );

  application.GetScene().Add( imageView );

  application.SendNotification();

  // loading started, this waits for the loader thread
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetVisualResourceStatus( ImageView::Property::IMAGE ), Visual::ResourceStatus::READY, TEST_LOCATION );

  // Reset flag
  gResourceReadySignalFired = false;

  // Change size
  imageView.SetProperty( Actor::Property::SIZE, Vector2( 0.f, 0.f ) );

  application.SendNotification();

  // rasterization started, this waits for the rasterize thread
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );
  // Fail to rasterize because the size is 0.
  DALI_TEST_EQUALS( imageView.GetVisualResourceStatus( ImageView::Property::IMAGE ), Visual::ResourceStatus::FAILED, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewTVGLoading(void)
{
  ToolkitTestApplication application;

  tet_infoline("ImageView Testing TVG image loading");

  {
    ImageView imageView = ImageView::New( );

    imageView.SetImage( TEST_RESOURCE_DIR "/test.tvg" );

    application.GetScene().Add( imageView );
    DALI_TEST_CHECK( imageView );
    Vector3 naturalSize = imageView.GetNaturalSize();

    DALI_TEST_EQUALS( naturalSize.width, 100.0f, TEST_LOCATION );
    DALI_TEST_EQUALS( naturalSize.height, 100.0f, TEST_LOCATION );
  }
  END_TEST;
}

namespace
{

static int gResourceReadySignalCounter = 0;

void OnResourceReadySignal01( Control control )
{
  gResourceReadySignalCounter++;

  if(control.GetVisualResourceStatus(ImageView::Property::IMAGE) == Visual::ResourceStatus::READY)
  {
    if( gResourceReadySignalCounter == 1 )
    {
      // Set image twice
      // It makes the first new visual be deleted immediately
      ImageView::DownCast( control ).SetImage( gImage_34_RGBA );
      ImageView::DownCast( control ).SetImage( gImage_34_RGBA );
    }
  }
  else if(control.GetVisualResourceStatus(ImageView::Property::IMAGE) == Visual::ResourceStatus::FAILED)
  {
    // Make the resource ready immediately
    control[ImageView::Property::IMAGE] = TEST_RESOURCE_DIR "/svg1.svg";
  }
}

void OnResourceReadySignal02( Control control )
{
  if(++gResourceReadySignalCounter == 1)
  {
    // It makes the first new visual be deleted immediately
    // The first image will not be loaded.
    control[ImageView::Property::IMAGE] = Property::Map().Add(ImageVisual::Property::URL, gImage_600_RGB)
                                                         .Add(ImageVisual::Property::RELEASE_POLICY, ImageVisual::ReleasePolicy::NEVER);
    control[ImageView::Property::IMAGE] = TEST_IMAGE_1;
  }
}

ImageView gImageView1;
ImageView gImageView2;
ImageView gImageView3;

void OnResourceReadySignal03( Control control )
{
  if(gResourceReadySignalCounter == 0)
  {
    // Queue loading
    // 1. Use cached image, then UploadComplete will be called right after OnResourceReadySignal03.
    gImageView2[ImageView::Property::IMAGE] = gImage_34_RGBA;

    // 2. Load a new image
    gImageView3[ImageView::Property::IMAGE] = TEST_IMAGE_1;

    // 3. Use the new image again
    gImageView1[ImageView::Property::IMAGE] = TEST_IMAGE_1;
    gImageView1.ResourceReadySignal().Connect(&OnResourceReadySignal03);
  }
  else if(gResourceReadySignalCounter == 1)
  {
    // This is called from TextureManager::ProcessQueuedTextures().
    gImageView1.Unparent();
    gImageView1.Reset();
  }
  gResourceReadySignalCounter++;
}

}

int UtcDaliImageViewSetImageOnResourceReadySignal01(void)
{
  tet_infoline("Test setting image from within signal handler.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  ImageView imageView = ImageView::New( gImage_34_RGBA );
  imageView.ResourceReadySignal().Connect( &OnResourceReadySignal01 );

  application.GetScene().Add( imageView );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( gResourceReadySignalCounter, 2, TEST_LOCATION );

  DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );

  // Reset count
  gResourceReadySignalCounter = 0;

  imageView[ImageView::Property::IMAGE] = "invalid.jpg";

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  // Run idle callback
  application.RunIdles();

  DALI_TEST_EQUALS( gResourceReadySignalCounter, 2, TEST_LOCATION );

  DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetImageOnResourceReadySignal02(void)
{
  tet_infoline("Test setting image from within signal handler.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  ImageView imageView = ImageView::New( gImage_34_RGBA );
  imageView.ResourceReadySignal().Connect( &OnResourceReadySignal02 );

  application.GetScene().Add( imageView );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  application.SendNotification();
  application.Render();

  // Wait for loading an image
  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  DALI_TEST_EQUALS( gResourceReadySignalCounter, 2, TEST_LOCATION );

  DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetImageOnResourceReadySignal03(void)
{
  tet_infoline("Test setting image from within signal handler.");

  ToolkitTestApplication application;

  gResourceReadySignalCounter = 0;

  gImageView1 = ImageView::New(gImage_34_RGBA);
  application.GetScene().Add(gImageView1);

  // Wait for loading
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1), true, TEST_LOCATION);

  gImageView2 = ImageView::New(gImage_600_RGB);
  gImageView2.ResourceReadySignal().Connect(&OnResourceReadySignal03);
  application.GetScene().Add(gImageView2);

  gImageView3 = ImageView::New();
  application.GetScene().Add(gImageView3);

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  application.SendNotification();
  application.Render();

  END_TEST;
}
