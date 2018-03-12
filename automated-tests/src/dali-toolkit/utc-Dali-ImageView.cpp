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

#include <test-native-image.h>
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

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
  varying mediump vec2 vTexCoord;\n
  uniform sampler2D sTexture;\n
  uniform lowp vec4 uColor;\n
  \n
  void main()\n
  {\n
    gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n
  }\n
);

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

void TestImage( ImageView imageView, BufferImage image )
{
  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  Property::Map map;
  DALI_TEST_CHECK( value.Get( map ) );

  DALI_TEST_CHECK( map.Find( "width" ) );
  DALI_TEST_CHECK( map.Find( "height" ) );
  DALI_TEST_CHECK( map.Find( "type" ) );

  int width = 0;
  DALI_TEST_CHECK( map[ "width" ].Get( width ) );
  DALI_TEST_EQUALS( (unsigned int)width, image.GetWidth(), TEST_LOCATION );

  int height = 0;
  DALI_TEST_CHECK( map[ "height" ].Get( height ) );
  DALI_TEST_EQUALS( (unsigned int)height, image.GetHeight(), TEST_LOCATION );

  std::string type;
  DALI_TEST_CHECK( map[ "type" ].Get( type ) );
  DALI_TEST_EQUALS( type, "BufferImage", TEST_LOCATION );
}

void TestImage( ImageView imageView, ResourceImage image )
{
  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  Property::Map map;
  DALI_TEST_CHECK( value.Get( map ) );

  if( map.Find( "width" ) )
  {
    int width = 0;
    DALI_TEST_CHECK( map[ "width" ].Get( width ) );
    DALI_TEST_EQUALS( (unsigned int)width, image.GetWidth(), TEST_LOCATION );
  }

  if( map.Find( "height" ) )
  {
    int height = 0;
    DALI_TEST_CHECK( map[ "height" ].Get( height ) );
    DALI_TEST_EQUALS( (unsigned int)height, image.GetHeight(), TEST_LOCATION );
  }

  DALI_TEST_CHECK( map.Find( "type" ) );

  std::string type;
  DALI_TEST_CHECK( map[ "type" ].Get( type ) );
  DALI_TEST_EQUALS( type, "ResourceImage", TEST_LOCATION );

  std::string filename;
  DALI_TEST_CHECK( map[ "filename" ].Get( filename ) );
  DALI_TEST_EQUALS( filename, image.GetUrl(), TEST_LOCATION );
}

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
  TestApplication application;

  ImageView imageView = ImageView::New();

  DALI_TEST_CHECK( imageView );

  END_TEST;
}

int UtcDaliImageViewNewImageP(void)
{
  TestApplication application;

  BufferImage image = CreateBufferImage( 100, 200, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  ImageView imageView = ImageView::New( image );

  DALI_TEST_CHECK( imageView );
  TestImage( imageView, image );

  END_TEST;
}

int UtcDaliImageViewNewUrlP(void)
{
  TestApplication application;

  ImageView imageView = ImageView::New( TEST_IMAGE_FILE_NAME );
  DALI_TEST_CHECK( imageView );

  TestUrl( imageView, TEST_IMAGE_FILE_NAME );

  END_TEST;
}

int UtcDaliImageViewConstructorP(void)
{
  TestApplication application;

  ImageView imageView;

  DALI_TEST_CHECK( !imageView );

  END_TEST;
}

int UtcDaliImageViewCopyConstructorP(void)
{
  TestApplication application;

  // Initialize an object, ref count == 1
  ImageView imageView = ImageView::New();

  ImageView copy( imageView );
  DALI_TEST_CHECK( copy );

  END_TEST;
}

int UtcDaliImageViewAssignmentOperatorP(void)
{
  TestApplication application;

  ImageView imageView = ImageView::New();

  ImageView copy( imageView );
  DALI_TEST_CHECK( copy );
  DALI_TEST_EQUALS( imageView, copy, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewDownCastP(void)
{
  TestApplication application;

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
  TestApplication application;

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

int UtcDaliImageViewSetGetProperty02(void)
{
  ToolkitTestApplication application;

  Image image = CreateBufferImage( 10, 10, Color::WHITE );
  ImageView imageView = ImageView::New(image);
  Vector4 fullImageRect( 0.f, 0.f, 1.f, 1.f );

  Stage::GetCurrent().Add( imageView );

  application.SendNotification();
  application.Render();
  TestGlAbstraction& gl = application.GetGlAbstraction();

  Vector4 pixelAreaUniform;
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "pixelArea", pixelAreaUniform ) );
  DALI_TEST_EQUALS( pixelAreaUniform, fullImageRect, TEST_LOCATION );

  Property::Value value = imageView.GetProperty( ImageView::Property::PIXEL_AREA );
  Vector4 pixelAreaValue;
  DALI_TEST_CHECK( value.Get(pixelAreaValue) );
  DALI_TEST_EQUALS( pixelAreaValue, fullImageRect, TEST_LOCATION );

  Vector4 pixelAreaSet( 0.2f, 0.2f, 0.3f, 0.3f );
  imageView.SetProperty( ImageView::Property::PIXEL_AREA, pixelAreaSet);

  application.SendNotification();
  application.Render();

  value = imageView.GetProperty( ImageView::Property::PIXEL_AREA );
  value.Get(pixelAreaValue);
  DALI_TEST_EQUALS( pixelAreaValue, pixelAreaSet, TEST_LOCATION );

  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "pixelArea", pixelAreaUniform ) );
  DALI_TEST_EQUALS( pixelAreaUniform, pixelAreaSet, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetGetProperty03(void)
{
  ToolkitTestApplication application;

  Image image = CreateBufferImage( 10, 10, Color::WHITE );
  ImageView imageView = ImageView::New(image);
  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  // conventional alpha blending
  Renderer renderer = imageView.GetRendererAt( 0 );
  Property::Value value = renderer.GetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );
  bool enable;
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( !enable );

  // pre-multiplied alpha blending
  imageView.SetProperty( Toolkit::ImageView::Property::PRE_MULTIPLIED_ALPHA, true );
  application.SendNotification();
  application.Render();

  int srcFactorRgb    = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_RGB );
  int destFactorRgb   = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_RGB );
  int srcFactorAlpha  = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_SRC_ALPHA );
  int destFactorAlpha = renderer.GetProperty<int>( Renderer::Property::BLEND_FACTOR_DEST_ALPHA );
  DALI_TEST_CHECK( srcFactorRgb == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorRgb == BlendFactor::ONE_MINUS_SRC_ALPHA );
  DALI_TEST_CHECK( srcFactorAlpha == BlendFactor::ONE );
  DALI_TEST_CHECK( destFactorAlpha == BlendFactor::ONE_MINUS_SRC_ALPHA );

  value = renderer.GetProperty( Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA );
  DALI_TEST_CHECK( value.Get( enable ) );
  DALI_TEST_CHECK( enable );

  END_TEST;
}

int UtcDaliImageViewPixelArea(void)
{
  // Test pixel area property
  ToolkitTestApplication application;

  // Gif image, use AnimatedImageVisual internally
  // Atlasing is applied to pack multiple frames, use custom wrap mode
  ImageView gifView = ImageView::New();
  const Vector4 pixelAreaVisual( 0.f, 0.f, 2.f, 2.f );
  gifView.SetProperty( ImageView::Property::IMAGE,
                       Property::Map().Add( ImageVisual::Property::URL, TEST_GIF_FILE_NAME )
                                      .Add( ImageVisual::Property::PIXEL_AREA, pixelAreaVisual ) );

  // Add to stage
  Stage stage = Stage::GetCurrent();
  stage.Add( gifView );

  // loading started
  application.SendNotification();
  application.Render(16);
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
  Stage::GetCurrent().Add( imageView );
  imageView.SetSize(100, 100);
  imageView.SetParentOrigin( ParentOrigin::CENTER );

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
  // loading is not started if the actor is offStage

  Stage::GetCurrent().Add( imageView );
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
  params["width"] = ToString(34);
  params["height"] = ToString(34);
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

  Stage::GetCurrent().Add( imageView );
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
  params["width"] = ToString(34);
  params["height"] = ToString(34);
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

    Stage::GetCurrent().Add( imageView );
    application.SendNotification();
    application.Render(16);

    TraceCallStack::NamedParams params;
    params["width"] = ToString(34);
    params["height"] = ToString(34);
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

    Stage::GetCurrent().Add( imageView );
    application.SendNotification();
    application.Render(16);

    TraceCallStack::NamedParams params;
    params["width"] = ToString(34);
    params["height"] = ToString(34);
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

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliImageViewSizeWithBackground(void)
{
  ToolkitTestApplication application;

  int width = 100;
  int height = 200;
  Image image = CreateBufferImage( width, height, Vector4(1.f, 1.f, 1.f, 1.f) );
  ImageView imageView = ImageView::New();
  imageView.SetBackgroundImage( image );

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetCurrentSize().width, (float)width, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetCurrentSize().height, (float)height, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSizeWithBackgroundAndImage(void)
{
  ToolkitTestApplication application;

  int widthBackground = 100;
  int heightBackground = 200;
  int width = 300;
  int height = 400;
  Image imageBackground = CreateBufferImage( widthBackground, heightBackground, Vector4(1.f, 1.f, 1.f, 1.f) );
  Image image = CreateBufferImage( width, height, Vector4(1.f, 1.f, 1.f, 1.f) );

  ImageView imageView = ImageView::New();
  imageView.SetBackgroundImage( imageBackground );
  imageView.SetImage( image );

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetCurrentSize().width, (float)width, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetCurrentSize().height, (float)height, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewHeightForWidthBackground(void)
{
  ToolkitTestApplication application;

  int widthBackground = 100;
  int heightBackground = 200;
  Image imageBackground = CreateBufferImage( widthBackground, heightBackground, Vector4(1.f, 1.f, 1.f, 1.f) );

  ImageView imageView = ImageView::New();
  imageView.SetBackgroundImage( imageBackground );

  Stage::GetCurrent().Add( imageView );
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
  int height = 400;
  Image imageBackground = CreateBufferImage( widthBackground, heightBackground, Vector4(1.f, 1.f, 1.f, 1.f) );
  Image image = CreateBufferImage( width, height, Vector4(1.f, 1.f, 1.f, 1.f) );

  ImageView imageView = ImageView::New();
  imageView.SetBackgroundImage( imageBackground );
  imageView.SetImage( image );

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( imageView.GetHeightForWidth( width ), (float)height, TEST_LOCATION );
  DALI_TEST_EQUALS( imageView.GetWidthForHeight( height ), (float)width, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetBufferImage(void)
{
  ToolkitTestApplication application;

  int width1 = 300;
  int height1 = 400;
  BufferImage image1 = CreateBufferImage( width1, height1, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  ImageView imageView = ImageView::New();
  imageView.SetImage( image1 );

  TestImage( imageView, image1 );

  int width2 = 600;
  int height2 = 500;
  BufferImage image2 = CreateBufferImage( width2, height2, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  imageView.SetImage( image2 );

  TestImage( imageView, image2 );

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

int UtcDaliImageViewSetImageOnstageP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );
  TestImage( imageView, image1 );

  int width = 300;
  int height = 400;
  BufferImage image2 = CreateBufferImage( width, height, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  imageView.SetImage( image2 );
  TestImage( imageView, image2 );

  END_TEST;
}

int UtcDaliImageViewSetImageOnstageN(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();

  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );
  TestImage( imageView, image1 );

  Image image2;
  imageView.SetImage( image2 );

  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  //the value should be empty
  std::string url;
  DALI_TEST_CHECK( !value.Get( url ) );

  Property::Map map;
  DALI_TEST_CHECK( !value.Get( map ) );

  END_TEST;
}

int UtcDaliImageViewSetImageOffstageP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();
  Stage::GetCurrent().Remove( imageView );

  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );
  TestImage( imageView, image1 );

  int width = 300;
  int height = 400;
  BufferImage image2 = CreateBufferImage( width, height, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  imageView.SetImage( image2 );
  TestImage( imageView, image2 );

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


  int width = 100;
  int height = 200;
  Image image = CreateBufferImage( width, height, Vector4(1.f, 1.f, 1.f, 1.f) );

  // Check ImageView with background and main image, to ensure both visuals are marked as loaded
  ImageView imageView = ImageView::New( TEST_GIF_FILE_NAME );

  imageView.SetBackgroundImage( image );

  DALI_TEST_EQUALS( imageView.IsResourceReady(), false, TEST_LOCATION );

  imageView.ResourceReadySignal().Connect( &ResourceReadySignal);

  Stage::GetCurrent().Add( imageView );

  application.SendNotification();
  application.Render(16);


  DALI_TEST_EQUALS( imageView.IsResourceReady(), true, TEST_LOCATION );

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliImageViewSetImageOffstageN(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();

  Stage::GetCurrent().Add( imageView );
  application.SendNotification();
  application.Render();
  Stage::GetCurrent().Remove( imageView );

  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );
  TestImage( imageView, image1 );

  Image image2;
  imageView.SetImage( image2 );

  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  //the value should be empty
  std::string url;
  DALI_TEST_CHECK( !value.Get( url ) );

  Property::Map map;
  DALI_TEST_CHECK( !value.Get( map ) );

  END_TEST;
}

int UtcDaliImageViewSetImageN(void)
{
  ToolkitTestApplication application;

  Image image1;
  ImageView imageView = ImageView::New();
  imageView.SetImage( image1 );

  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );

  //the value should be empty
  std::string url;
  DALI_TEST_CHECK( !value.Get( url ) );

  Property::Map map;
  DALI_TEST_CHECK( !value.Get( map ) );

  std::string resource_url;
  Property::Value val = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  DALI_TEST_CHECK( !val.Get( resource_url ) );

  END_TEST;
}

int UtcDaliImageViewSetImageTypeChangesP(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  Toolkit::Internal::Control& controlImpl = Toolkit::Internal::GetImplementation( imageView );

  Stage::GetCurrent().Add( imageView );

  std::string url;
  Property::Map map;
  Toolkit::Visual::Base visual;

  Property::Value value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  visual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );

  application.SendNotification();
  application.Render( 16 );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( ! value.Get( map ) ); // Value should be empty
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

  // Set an empty Image
  imageView.SetImage( Image() );

  application.SendNotification();
  application.Render( 16 );

  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  visual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( ! value.Get( map ) ); // Value should be empty
  DALI_TEST_CHECK( ! visual );           // Visual should be invalid

  // Set an Image
  ResourceImage image1 = ResourceImage::New( TEST_IMAGE_FILE_NAME );
  imageView.SetImage( image1 );

  application.SendNotification();
  application.Render( 16 );

  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  visual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( value.Get( map ) );   // Value should NOT be empty
  DALI_TEST_CHECK( visual );             // Visual should be valid

  // Set an empty URL
  imageView.SetImage( "" );

  application.SendNotification();
  application.Render( 16 );

  value = imageView.GetProperty( imageView.GetPropertyIndex( "image" ) );
  visual = DevelControl::GetVisual( controlImpl, ImageView::Property::IMAGE );

  DALI_TEST_CHECK( ! value.Get( url ) ); // Value should be empty
  DALI_TEST_CHECK( ! value.Get( map ) ); // Value should be empty
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
  DALI_TEST_CHECK( imageView.GetProperty( ImageView::Property::RESOURCE_URL ).Get< std::string >().empty() );

  imageView.SetProperty( ImageView::Property::RESOURCE_URL, "TestString" );
  DALI_TEST_EQUALS( imageView.GetProperty( ImageView::Property::RESOURCE_URL ).Get< std::string >(), "TestString", TEST_LOCATION );

  END_TEST;
}

// Scenarios 1: ImageView from regular image
int UtcDaliImageViewSetImageBufferImage(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  Stage::GetCurrent().Add( imageView );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace( true );

  std::vector< GLuint > ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  int width = 300;
  int height = 400;
  BufferImage image = CreateBufferImage( width, height, Color::WHITE );

  imageView.SetImage( image );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethod("BindTexture") );

  std::stringstream params;
  params << GL_TEXTURE_2D << ", " << 23;
  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethodAndParams("BindTexture", params.str()) );

  END_TEST;
}

// Scenarios 2: ImageView from Native image
int UtcDaliImageViewSetImageNativeImage(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  Stage::GetCurrent().Add( imageView );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace( true );

  std::vector< GLuint > ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  int width = 200;
  int height = 500;
  TestNativeImagePointer nativeImageInterface = TestNativeImage::New( width, height );
  NativeImage nativeImage = NativeImage::New( *(nativeImageInterface.Get()) );

  imageView.SetImage( nativeImage );
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethod("BindTexture") );

  std::stringstream params;
  params << GL_TEXTURE_EXTERNAL_OES << ", " << 23;
  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethodAndParams("BindTexture", params.str()) );

  END_TEST;
}

// Scenarios 3: ImageView initially from regular image but then SetImage called with Native image
int UtcDaliImageViewSetImageBufferImageToNativeImage(void)
{
  ToolkitTestApplication application;

  int width = 300;
  int height = 400;
  BufferImage image = CreateBufferImage( width, height, Color::WHITE );

  ImageView imageView = ImageView::New( image );
  Stage::GetCurrent().Add( imageView );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace( true );

  std::vector< GLuint > ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethod("BindTexture") );

  std::stringstream params;
  params << GL_TEXTURE_2D << ", " << 23;
  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethodAndParams("BindTexture", params.str()) );

  width = 200;
  height = 500;
  TestNativeImagePointer nativeImageInterface = TestNativeImage::New( width, height );
  NativeImage nativeImage = NativeImage::New( *(nativeImageInterface.Get()) );
  imageView.SetImage( nativeImage );

  ids.clear();
  ids.push_back( 24 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethod("BindTexture") );

  std::stringstream nextTextureParams;
  nextTextureParams << GL_TEXTURE_EXTERNAL_OES << ", " << 24;
  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethodAndParams("BindTexture", nextTextureParams.str()) );

  END_TEST;
}

// Scenarios 4: ImageView initially from Native image but then SetImage called with regular image
int UtcDaliImageViewSetImageNativeImageToBufferImage(void)
{
  ToolkitTestApplication application;

  int width = 300;
  int height = 400;
  TestNativeImagePointer nativeImageInterface = TestNativeImage::New( width, height );
  NativeImage nativeImage = NativeImage::New( *(nativeImageInterface.Get()) );

  ImageView imageView = ImageView::New( nativeImage );
  Stage::GetCurrent().Add( imageView );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace( true );

  std::vector< GLuint > ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethod("BindTexture") );

  std::stringstream params;
  params << GL_TEXTURE_EXTERNAL_OES << ", " << 23;
  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethodAndParams("BindTexture", params.str()) );

  width = 200;
  height = 500;
  BufferImage image = CreateBufferImage( width, height, Color::WHITE );
  imageView.SetImage( image );

  ids.clear();
  ids.push_back( 24 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethod("BindTexture") );

  std::stringstream nextTextureParams;
  nextTextureParams << GL_TEXTURE_2D << ", " << 24;
  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethodAndParams("BindTexture", nextTextureParams.str()) );

  END_TEST;
}

// Scenarios 5: ImageView from Native image with custom shader
int UtcDaliImageViewSetImageNativeImageWithCustomShader(void)
{
  ToolkitTestApplication application;

  int width = 300;
  int height = 400;

  Property::Map customShader;
  customShader.Insert( "vertexShader", VERTEX_SHADER );
  customShader.Insert( "fragmentShader", FRAGMENT_SHADER );

  Property::Array shaderHints;
  shaderHints.PushBack( "requiresSelfDepthTest" );
  shaderHints.PushBack( "outputIsTransparent" );
  shaderHints.PushBack( "outputIsOpaque" );
  shaderHints.PushBack( "modifiesGeometry" );

  customShader.Insert( "hints", shaderHints );

  Property::Map map;
  map.Insert( "shader", customShader );

  TestNativeImagePointer nativeImageInterface = TestNativeImage::New( width, height );
  NativeImage nativeImage = NativeImage::New( *(nativeImageInterface.Get()) );

  ImageView imageView = ImageView::New( nativeImage );
  imageView.SetProperty( ImageView::Property::IMAGE, map );
  Stage::GetCurrent().Add( imageView );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace( true );

  std::vector< GLuint > ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethod("BindTexture") );

  std::stringstream params;
  params << GL_TEXTURE_EXTERNAL_OES << ", " << 23;
  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethodAndParams("BindTexture", params.str()) );

  END_TEST;
}

// Scenarios 6: ImageView initially from regular image with custom shader but then SetImage called with Native
int UtcDaliImageViewSetImageBufferImageWithCustomShaderToNativeImage(void)
{
  ToolkitTestApplication application;

  int width = 300;
  int height = 400;

  Property::Map customShader;
  customShader.Insert( "vertexShader", VERTEX_SHADER );
  customShader.Insert( "fragmentShader", FRAGMENT_SHADER );

  Property::Array shaderHints;
  shaderHints.PushBack( "requiresSelfDepthTest" );
  shaderHints.PushBack( "outputIsTransparent" );
  shaderHints.PushBack( "outputIsOpaque" );
  shaderHints.PushBack( "modifiesGeometry" );

  customShader.Insert( "hints", shaderHints );

  Property::Map map;
  map.Insert( "shader", customShader );

  BufferImage image = CreateBufferImage( width, height, Color::WHITE );

  ImageView imageView = ImageView::New( image );
  imageView.SetProperty( ImageView::Property::IMAGE, map );
  Stage::GetCurrent().Add( imageView );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace( true );

  std::vector< GLuint > ids;
  ids.push_back( 23 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethod("BindTexture") );

  std::stringstream params;
  params << GL_TEXTURE_2D << ", " << 23;
  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethodAndParams("BindTexture", params.str()) );

  TestNativeImagePointer nativeImageInterface = TestNativeImage::New( width, height );
  NativeImage nativeImage = NativeImage::New( *(nativeImageInterface.Get()) );
  imageView.SetImage( nativeImage );

  ids.clear();
  ids.push_back( 24 );
  application.GetGlAbstraction().SetNextTextureIds( ids );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethod("BindTexture") );

  std::stringstream nativeImageParams;
  nativeImageParams << GL_TEXTURE_EXTERNAL_OES << ", " << 24;
  DALI_TEST_CHECK( gl.GetTextureTrace().FindMethodAndParams("BindTexture", nativeImageParams.str()) );


  END_TEST;
}

int UtcDaliImageViewGetImageP1(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New();
  DALI_TEST_CHECK( ! imageView.GetImage() );

  Image image = CreateBufferImage();
  imageView.SetImage( image );
  DALI_TEST_CHECK( imageView.GetImage() == image );

  END_TEST;
}

int UtcDaliImageViewGetImageP2(void)
{
  ToolkitTestApplication application;

  BufferImage image = CreateBufferImage();
  ImageView imageView = ImageView::New( image );
  DALI_TEST_CHECK( imageView.GetImage() == image );

  END_TEST;
}

int UtcDaliImageViewGetImageN(void)
{
  ToolkitTestApplication application;

  ImageView imageView = ImageView::New( TEST_IMAGE_FILE_NAME );
  DALI_TEST_CHECK( ! imageView.GetImage() );

  Image image = CreateBufferImage();
  imageView.SetImage( image );
  DALI_TEST_CHECK( imageView.GetImage() == image );

  imageView.SetImage( TEST_IMAGE_FILE_NAME );
  DALI_TEST_CHECK( ! imageView.GetImage() );

  END_TEST;
}


int UtcDaliImageViewReplaceImage(void)
{
  ToolkitTestApplication application;

  gResourceReadySignalFired = false;

  int width = 100;
  int height = 200;
  Image image = CreateBufferImage( width, height, Vector4(1.f, 1.f, 1.f, 1.f) );

  // Check ImageView with background and main image, to ensure both visuals are marked as loaded
  ImageView imageView = ImageView::New( TEST_IMAGE_1 );

  DALI_TEST_EQUALS( imageView.IsResourceReady(), false, TEST_LOCATION );

  imageView.ResourceReadySignal().Connect( &ResourceReadySignal);

  Stage::GetCurrent().Add( imageView );

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
  Stage::GetCurrent().Add( dummyControl );

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

  Stage::GetCurrent().Add( imageViewWithExistingImage );

  DALI_TEST_EQUALS( gResourceReadySignalFired, true, TEST_LOCATION );

  END_TEST;
}
