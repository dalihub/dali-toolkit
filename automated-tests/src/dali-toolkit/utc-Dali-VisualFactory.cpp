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
#include <toolkit-bitmap-loader.h>
#include <toolkit-event-thread-callback.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/devel-api/images/nine-patch-image.h>
#include <dali-toolkit/devel-api/align-enums.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/dali-toolkit.h>
#include "dummy-control.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
typedef NinePatchImage::StretchRanges StretchRanges;

const char* TEST_IMAGE_FILE_NAME =  "gallery_image_01.jpg";
const char* TEST_NPATCH_FILE_NAME =  "gallery_image_01.9.png";
const char* TEST_SVG_FILE_NAME = TEST_RESOURCE_DIR "/svg1.svg";
const char* TEST_OBJ_FILE_NAME = TEST_RESOURCE_DIR "/Cube.obj";
const char* TEST_MTL_FILE_NAME = TEST_RESOURCE_DIR "/ToyRobot-Metal.mtl";
const char* TEST_SIMPLE_OBJ_FILE_NAME = TEST_RESOURCE_DIR "/Cube-Points-Only.obj";
const char* TEST_SIMPLE_MTL_FILE_NAME = TEST_RESOURCE_DIR "/ToyRobot-Metal-Simple.mtl";

// resolution: 50*50, frame count: 4, frame delay: 0.2 second for each frame
const char* TEST_GIF_FILE_NAME = TEST_RESOURCE_DIR "/anim.gif";

// resolution: 34*34, pixel format: RGBA8888
static const char* gImage_34_RGBA = TEST_RESOURCE_DIR "/icon-edit.png";
// resolution: 600*600, pixel format: RGB888
static const char* gImage_600_RGB = TEST_RESOURCE_DIR "/test-image-600.jpg";

Property::Map DefaultTransform()
{
  Property::Map transformMap;
  transformMap
    .Add( Toolkit::DevelVisual::Transform::Property::OFFSET, Vector2(0.0f, 0.0f) )
    .Add( Toolkit::DevelVisual::Transform::Property::SIZE, Vector2(1.0f, 1.0f) )
    .Add( Toolkit::DevelVisual::Transform::Property::ORIGIN, Toolkit::Align::CENTER )
    .Add( Toolkit::DevelVisual::Transform::Property::ANCHOR_POINT, Toolkit::Align::CENTER )
    .Add( Toolkit::DevelVisual::Transform::Property::OFFSET_POLICY, Vector2( Toolkit::DevelVisual::Transform::Policy::RELATIVE, Toolkit::DevelVisual::Transform::Policy::RELATIVE ) )
    .Add( Toolkit::DevelVisual::Transform::Property::SIZE_POLICY, Vector2( Toolkit::DevelVisual::Transform::Policy::RELATIVE, Toolkit::DevelVisual::Transform::Policy::RELATIVE ) );
  return transformMap;
}

Integration::Bitmap* CreateBitmap( unsigned int imageWidth, unsigned int imageHeight, unsigned int initialColor, Pixel::Format pixelFormat )
{
  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_RETAIN );
  Integration::PixelBuffer* pixbuffer = bitmap->GetPackedPixelsProfile()->ReserveBuffer( pixelFormat, imageWidth, imageHeight, imageWidth, imageHeight );
  unsigned int bytesPerPixel = GetBytesPerPixel( pixelFormat );

  memset( pixbuffer, initialColor, imageHeight * imageWidth * bytesPerPixel );

  return bitmap;
}

void InitialiseRegionsToZeroAlpha( Integration::Bitmap* image, unsigned int imageWidth, unsigned int imageHeight, Pixel::Format pixelFormat )
{
  PixelBuffer* pixbuffer = image->GetBuffer();
  unsigned int bytesPerPixel = GetBytesPerPixel( pixelFormat );

  for( unsigned int row = 0; row < imageWidth; ++row )
  {
    unsigned int pixelOffset = row * bytesPerPixel;
    pixbuffer[ pixelOffset + 3 ] = 0x00;
    pixelOffset += ( imageHeight - 1 ) * imageWidth * bytesPerPixel;
    pixbuffer[ pixelOffset + 3 ] = 0x00;
  }

  for ( unsigned int column = 0; column < imageHeight; ++column )
  {
    unsigned int pixelOffset = column * imageWidth * bytesPerPixel;
    pixbuffer[ pixelOffset + 3 ] = 0x00;
    pixelOffset += ( imageWidth -1 ) * bytesPerPixel;
    pixbuffer[ pixelOffset + 3 ] = 0x00;
  }
}

void AddStretchRegionsToImage( Integration::Bitmap* image, unsigned int imageWidth, unsigned int imageHeight, const StretchRanges& stretchRangesX, const StretchRanges& stretchRangesY, Pixel::Format pixelFormat )
{
  PixelBuffer* pixbuffer = image->GetBuffer();
  unsigned int bytesPerPixel = GetBytesPerPixel( pixelFormat );

  for(StretchRanges::ConstIterator it = stretchRangesX.Begin(); it != stretchRangesX.End(); ++it)
  {
    const Uint16Pair& range = *it;
    //since the stretch range is in the cropped image space, we need to offset by 1 to get it to the uncropped image space
    for( unsigned int column = range.GetX() + 1u; column < range.GetY() + 1u; ++column )
    {
      unsigned int pixelOffset = column * bytesPerPixel;
      pixbuffer[ pixelOffset ] = 0x00;
      pixbuffer[ pixelOffset + 1 ] = 0x00;
      pixbuffer[ pixelOffset + 2 ] = 0x00;
      pixbuffer[ pixelOffset + 3 ] = 0xFF;
    }
  }


  for(StretchRanges::ConstIterator it = stretchRangesY.Begin(); it != stretchRangesY.End(); ++it)
  {
    const Uint16Pair& range = *it;
    //since the stretch range is in the cropped image space, we need to offset by 1 to get it to the uncropped image space
    for( unsigned int row = range.GetX() + 1u; row < range.GetY() + 1u; ++row )
    {
      unsigned int pixelOffset = row * imageWidth * bytesPerPixel;
      pixbuffer[ pixelOffset ] = 0x00;
      pixbuffer[ pixelOffset + 1 ] = 0x00;
      pixbuffer[ pixelOffset + 2 ] = 0x00;
      pixbuffer[ pixelOffset + 3 ] = 0xFF;
    }
  }
}

void AddChildRegionsToImage( Integration::Bitmap* image, unsigned int imageWidth, unsigned int imageHeight, const Vector4& requiredChildRegion, Pixel::Format pixelFormat )
{
  PixelBuffer* pixbuffer = image->GetBuffer();
  unsigned int bytesPerPixel = GetBytesPerPixel( pixelFormat );

  Integration::Bitmap::PackedPixelsProfile* srcProfile = image->GetPackedPixelsProfile();
  unsigned int bufferStride = srcProfile->GetBufferStride();

  // Add bottom child region
  for( unsigned int column = requiredChildRegion.x; column < imageWidth - requiredChildRegion.z; ++column )
  {
    unsigned int pixelOffset = column * bytesPerPixel;
    pixelOffset += ( imageHeight - 1 ) * bufferStride;
    pixbuffer[ pixelOffset ] = 0x00;
    pixbuffer[ pixelOffset + 1 ] = 0x00;
    pixbuffer[ pixelOffset + 2 ] = 0x00;
    pixbuffer[ pixelOffset + 3 ] = 0xFF;
  }

  // Add right child region
  for ( unsigned int row = requiredChildRegion.y; row < imageHeight - requiredChildRegion.w; ++row )
  {
    unsigned int pixelOffset = row * bufferStride + ( imageWidth - 1 ) * bytesPerPixel;
    pixbuffer[ pixelOffset ] = 0x00;
    pixbuffer[ pixelOffset + 1 ] = 0x00;
    pixbuffer[ pixelOffset + 2 ] = 0x00;
    pixbuffer[ pixelOffset + 3 ] = 0xFF;
  }
}

Integration::ResourcePointer CustomizeNinePatch( TestApplication& application,
                                                 unsigned int ninePatchImageWidth,
                                                 unsigned int ninePatchImageHeight,
                                                 const StretchRanges& stretchRangesX,
                                                 const StretchRanges& stretchRangesY,
                                                 bool addChildRegion = false,
                                                 Vector4 requiredChildRegion = Vector4::ZERO )
{
  TestPlatformAbstraction& platform = application.GetPlatform();

  Pixel::Format pixelFormat = Pixel::RGBA8888;

  tet_infoline("Create Bitmap");
  platform.SetClosestImageSize(Vector2( ninePatchImageWidth, ninePatchImageHeight));
  Integration::Bitmap* bitmap = CreateBitmap( ninePatchImageWidth, ninePatchImageHeight, 0xFF, pixelFormat );

  tet_infoline("Clear border regions");
  InitialiseRegionsToZeroAlpha( bitmap, ninePatchImageWidth, ninePatchImageHeight, pixelFormat );

  tet_infoline("Add Stretch regions to Bitmap");
  AddStretchRegionsToImage( bitmap, ninePatchImageWidth, ninePatchImageHeight, stretchRangesX, stretchRangesY, pixelFormat );

  if( addChildRegion )
  {
    tet_infoline("Add Child regions to Bitmap");
    AddChildRegionsToImage( bitmap, ninePatchImageWidth, ninePatchImageHeight, requiredChildRegion, pixelFormat );
  }

  tet_infoline("Getting resource");
  Integration::ResourcePointer resourcePtr(bitmap);
  //platform.SetResourceLoaded( 0, Dali::Integration::ResourceBitmap, resourcePtr );
  platform.SetSynchronouslyLoadedResource( resourcePtr);

  return resourcePtr;
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

  application.SendNotification();
  application.Render();

  application.Render();
  application.SendNotification();

  if( resourcePtr )
  {
    DALI_TEST_EQUALS( application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceSynchronouslyFunc ), true, TEST_LOCATION);
  }

  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );

}

} // namespace


void dali_visual_factory_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_visual_factory_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliVisualFactoryGet(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactory" );

  //Register type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "VisualFactory" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  VisualFactory factory;
  factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  VisualFactory newFactory = VisualFactory::Get();
  DALI_TEST_CHECK( newFactory );

  // Check that visual factory is a singleton
  DALI_TEST_CHECK(factory == newFactory);

  END_TEST;
}

int UtcDaliVisualFactoryCopyAndAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryCopyAndAssignment" );
  VisualFactory factory = VisualFactory::Get();

  VisualFactory factoryCopy( factory );
  DALI_TEST_CHECK(factory == factoryCopy);

  VisualFactory emptyFactory;
  VisualFactory emptyFactoryCopy( emptyFactory );
  DALI_TEST_CHECK(emptyFactory == emptyFactoryCopy);

  VisualFactory factoryEquals;
  factoryEquals = factory;
  DALI_TEST_CHECK(factory == factoryEquals);

  VisualFactory emptyFactoryEquals;
  emptyFactoryEquals = emptyFactory;
  DALI_TEST_CHECK( emptyFactory == emptyFactoryEquals );

  //self assignment
  factory = factory;
  DALI_TEST_CHECK( factory = factoryCopy );

  END_TEST;
}

int UtcDaliVisualFactoryGetColorVisual1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetColorVisual1:  Request color visual with a Property::Map" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  Vector4 testColor( 1.f, 0.5f, 0.3f, 0.2f );
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR,  testColor);

  Visual::Base visual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual );

  Vector3 actualValue(Vector4::ZERO);
  float opacity=0.0f;
  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.GetUniformValue<Vector3>( "mixColor", actualValue ) );
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "opacity", opacity ) );
  DALI_TEST_EQUALS( actualValue, Vector3(testColor), TEST_LOCATION );
  DALI_TEST_EQUALS( opacity, testColor.a, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualFactoryGetColorVisual2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetColorVisual2: Request color visual with a Vector4" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Vector4 testColor( 1.f, 0.5f, 0.3f, 0.2f );
  Dali::Property::Map map;
  map[ Visual::Property::TYPE ] = Visual::COLOR;
  map[ ColorVisual::Property::MIX_COLOR ] = testColor;
  Visual::Base visual = factory.CreateVisual( map );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual );

  Vector3 actualValue;
  float opacity;
  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.GetUniformValue<Vector3>( "mixColor", actualValue ) );
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "opacity", opacity ) );
  DALI_TEST_EQUALS( actualValue, Vector3(testColor), TEST_LOCATION );
  DALI_TEST_EQUALS( opacity, testColor.a, TEST_LOCATION );

  Stage::GetCurrent().Remove(actor);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliVisualFactoryGetBorderVisual1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetBorderVisual1:  Request border visual with a Property::Map" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  Vector4 testColor( 1.f, 0.5f, 0.3f, 0.2f );
  float testSize = 5.f;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::BORDER);
  propertyMap.Insert(BorderVisual::Property::COLOR,  testColor);
  propertyMap.Insert(BorderVisual::Property::SIZE,  testSize);

  Visual::Base visual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  visual.SetTransformAndSize(DefaultTransform(), Vector2(200.f, 200.f));

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  int blendMode = actor.GetRendererAt(0u).GetProperty<int>( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( static_cast<BlendMode::Type>(blendMode), BlendMode::ON, TEST_LOCATION );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualColor(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "borderColor", actualColor ) );
  DALI_TEST_EQUALS( actualColor, testColor, TEST_LOCATION );

  float actualSize = 0.f;
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "borderSize", actualSize ) );
  DALI_TEST_EQUALS( actualSize, testSize, TEST_LOCATION );

  actor.Unparent();
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliVisualFactoryGetBorderVisual2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetBorderVisual2:  Request border visual with a borderSize and a borderColor" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Vector4 testColor( 1.f, 0.5f, 0.3f, 1.f );
  float testSize = 5.f;

  Dali::Property::Map propertyMap;
  propertyMap[ Visual::Property::TYPE ] = Visual::BORDER;
  propertyMap[ BorderVisual::Property::COLOR  ] = testColor;
  propertyMap[ BorderVisual::Property::SIZE   ] = testSize;
  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  visual.SetTransformAndSize(DefaultTransform(), Vector2(200.f, 200.f));

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  int blendMode = actor.GetRendererAt(0u).GetProperty<int>( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( static_cast<BlendMode::Type>(blendMode), BlendMode::AUTO, TEST_LOCATION );

  Vector4 actualColor(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "borderColor", actualColor ) );
  DALI_TEST_EQUALS( actualColor, testColor, TEST_LOCATION );

  float actualSize = 0.f;
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "borderSize", actualSize ) );
  DALI_TEST_EQUALS( actualSize, testSize, TEST_LOCATION );

  actor.Unparent();

  // enable the anti-aliasing
  Dali::Property::Map map;
  map[ Visual::Property::TYPE ] = Visual::BORDER;
  map[ BorderVisual::Property::COLOR  ] = testColor;
  map[ BorderVisual::Property::SIZE   ] = testSize;
  map[ BorderVisual::Property::ANTI_ALIASING   ] = true;
  visual = factory.CreateVisual( map );

  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  Stage::GetCurrent().Add( actor );

  blendMode = actor.GetRendererAt(0u).GetProperty<int>( Renderer::Property::BLEND_MODE );
  DALI_TEST_EQUALS( static_cast<BlendMode::Type>(blendMode), BlendMode::ON, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualFactoryGetLinearGradientVisual(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualFactoryGetRadialGradientVisual");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::GRADIENT);

  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert(GradientVisual::Property::START_POSITION, start);
  propertyMap.Insert(GradientVisual::Property::END_POSITION, end);
  propertyMap.Insert(GradientVisual::Property::SPREAD_METHOD, GradientVisual::SpreadMethod::REPEAT);

  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.2f );
  stopOffsets.PushBack( 0.8f );
  propertyMap.Insert(GradientVisual::Property::STOP_OFFSET, stopOffsets);

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR, stopColors);

  Visual::Base visual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK( visual );

  // A lookup texture is generated and pass to shader as sampler
  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual, 1u);

  END_TEST;
}

int UtcDaliVisualFactoryGetRadialGradientVisual(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualFactoryGetRadialGradientVisual");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::GRADIENT);

  Vector2 center(100.f, 100.f);
  float radius = 100.f;
  propertyMap.Insert(GradientVisual::Property::UNITS,  GradientVisual::Units::USER_SPACE);
  propertyMap.Insert(GradientVisual::Property::CENTER,  center);
  propertyMap.Insert(GradientVisual::Property::RADIUS,  radius);

  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.0f );
  stopOffsets.PushBack( 1.f );
  propertyMap.Insert(GradientVisual::Property::STOP_OFFSET,   stopOffsets);

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR,   stopColors);

  Visual::Base visual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK( visual );

  // A lookup texture is generated and pass to shader as sampler
  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual, 1u );

  Matrix3 alignMatrix( radius, 0.f, 0.f, 0.f, radius, 0.f, center.x, center.y, 1.f );
  alignMatrix.Invert();

  Matrix3 actualValue( Matrix3::IDENTITY );
  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.GetUniformValue<Matrix3>( "uAlignmentMatrix", actualValue ) );
  DALI_TEST_EQUALS( actualValue, alignMatrix, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualFactoryDefaultOffsetsGradientVisual(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliVisualFactoryGetRadialGradientVisual");

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::GRADIENT);

  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert(GradientVisual::Property::START_POSITION, start);
  propertyMap.Insert(GradientVisual::Property::END_POSITION, end);
  propertyMap.Insert(GradientVisual::Property::SPREAD_METHOD, GradientVisual::SpreadMethod::REPEAT);

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert(GradientVisual::Property::STOP_COLOR, stopColors);

  Visual::Base visual = factory.CreateVisual(propertyMap);
  DALI_TEST_CHECK( visual );

  // A lookup texture is generated and pass to shader as sampler
  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual, 1u );

  Stage::GetCurrent().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliVisualFactoryGetImageVisual1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetImageVisual1: Request image visual with a Property::Map" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_IMAGE_FILE_NAME );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.

  const int width=512;
  const int height=513;
  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  Integration::Bitmap* bitmap = Integration::Bitmap::New( Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD );
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, width, height,width, height );

  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual, 1u,
                    ImageDimensions(width, height),
                    Integration::ResourcePointer( bitmap ) );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  Stage::GetCurrent().Remove( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliVisualFactoryGetImageVisual2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetImageVisual2: Request image visual with an image handle" );

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

int UtcDaliVisualFactoryGetImageVisual3(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetImageVisual3: Request image visual with a Property::Map, test custom wrap mode and pixel area with atlasing" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test wrap mode with atlasing. Image with a size smaller than 512*512 will be uploaded as a part of the atlas.
  const int width=34;
  const int height=34;
  const Vector4 pixelArea(-0.5f, -0.5f, 2.f, 2.f);

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  gImage_34_RGBA );
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
  application.Render();
  application.SendNotification();
  BitmapLoader loader = BitmapLoader::GetLatestCreated();
  DALI_TEST_CHECK( loader );
  loader.WaitForLoading();// waiting until the image to be loaded
  DALI_TEST_CHECK( loader.IsLoaded() );

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

int UtcDaliVisualFactoryGetImageVisual4(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetImageVisual4: Request image visual with a Property::Map, test custom wrap mode and pixel area without atlasing" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  // Test wrap mode without atlasing. Image with a size bigger than 512*512 will NOT be uploaded as a part of the atlas.
  const int width=600;
  const int height=600;
  const Vector4 pixelArea(-0.5f, -0.5f, 2.f, 2.f);

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  gImage_600_RGB );
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
  application.Render();
  application.SendNotification();
  BitmapLoader loader = BitmapLoader::GetLatestCreated();
  DALI_TEST_CHECK( loader );
  loader.WaitForLoading();// waiting until the image to be loaded
  DALI_TEST_CHECK( loader.IsLoaded() );

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

int UtcDaliVisualFactoryGetNPatchVisual1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetNPatchVisual1: Request 9-patch visual with a Property::Map" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  const unsigned int ninePatchImageHeight = 18;
  const unsigned int ninePatchImageWidth = 28;
  StretchRanges stretchRangesX;
  stretchRangesX.PushBack( Uint16Pair( 2, 3 ) );
  StretchRanges stretchRangesY;
  stretchRangesY.PushBack( Uint16Pair( 4, 5 ) );
  Integration::ResourcePointer ninePatchResource = CustomizeNinePatch( application, ninePatchImageWidth, ninePatchImageHeight, stretchRangesX, stretchRangesY );

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_NPATCH_FILE_NAME );
  {
    tet_infoline( "whole grid" );
    Visual::Base visual = factory.CreateVisual( propertyMap );
    DALI_TEST_CHECK( visual );


    TestGlAbstraction& gl = application.GetGlAbstraction();
    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    DummyControl actor = DummyControl::New();
    TestVisualRender( application, actor, visual, 1u,
                      ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                      ninePatchResource );

    DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  }

  propertyMap.Insert( ImageVisual::Property::BORDER_ONLY,  true );
  {
    tet_infoline( "border only" );
    Visual::Base visual = factory.CreateVisual( propertyMap );
    DALI_TEST_CHECK( visual );

    TestGlAbstraction& gl = application.GetGlAbstraction();
    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    DummyControl actor = DummyControl::New();
    TestVisualRender( application, actor, visual, 1u,
                      ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                      ninePatchResource );

    DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliVisualFactoryGetNPatchVisual2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetNPatchVisual2: Request n-patch visual with a Property::Map" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  const unsigned int ninePatchImageWidth = 18;
  const unsigned int ninePatchImageHeight = 28;
  StretchRanges stretchRangesX;
  stretchRangesX.PushBack( Uint16Pair( 2, 3 ) );
  stretchRangesX.PushBack( Uint16Pair( 5, 7 ) );
  stretchRangesX.PushBack( Uint16Pair( 12, 15 ) );
  StretchRanges stretchRangesY;
  stretchRangesY.PushBack( Uint16Pair( 4, 5 ) );
  stretchRangesY.PushBack( Uint16Pair( 8, 12 ) );
  stretchRangesY.PushBack( Uint16Pair( 15, 16 ) );
  stretchRangesY.PushBack( Uint16Pair( 25, 27 ) );
  Integration::ResourcePointer ninePatchResource = CustomizeNinePatch( application, ninePatchImageWidth, ninePatchImageHeight, stretchRangesX, stretchRangesY );

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  TEST_NPATCH_FILE_NAME );
  {
    Visual::Base visual = factory.CreateVisual( propertyMap );
    DALI_TEST_CHECK( visual );

    TestGlAbstraction& gl = application.GetGlAbstraction();
    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);

    DummyControl actor = DummyControl::New();
    TestVisualRender( application, actor, visual, 1u,
                      ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                      ninePatchResource );

    DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

    Stage::GetCurrent().Remove( actor );
    DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  }

  propertyMap.Insert( ImageVisual::Property::BORDER_ONLY,  true );
  {
    tet_infoline( "border only" );
    Visual::Base visual = factory.CreateVisual( propertyMap );
    DALI_TEST_CHECK( visual );

    TestGlAbstraction& gl = application.GetGlAbstraction();
    TraceCallStack& textureTrace = gl.GetTextureTrace();
    textureTrace.Enable(true);
    DummyControl actor = DummyControl::New();
    TestVisualRender( application, actor, visual, 1u,
                      ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                      ninePatchResource );


    DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

    Stage::GetCurrent().Remove( actor );
    DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  }

  END_TEST;
}

int UtcDaliVisualFactoryGetNPatchVisual3(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetNPatchVisual3: Request 9-patch visual with an image url" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  const unsigned int ninePatchImageHeight = 18;
  const unsigned int ninePatchImageWidth = 28;
  StretchRanges stretchRangesX;
  stretchRangesX.PushBack( Uint16Pair( 2, 3 ) );
  StretchRanges stretchRangesY;
  stretchRangesY.PushBack( Uint16Pair( 4, 5 ) );
  Integration::ResourcePointer ninePatchResource = CustomizeNinePatch( application, ninePatchImageWidth, ninePatchImageHeight, stretchRangesX, stretchRangesY );

  Visual::Base visual = factory.CreateVisual( TEST_NPATCH_FILE_NAME, ImageDimensions() );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual, 1u,
                    ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                    ninePatchResource );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );


  ResourceImage image = ResourceImage::New(TEST_NPATCH_FILE_NAME);
  Visual::Base nPatchVisual = factory.CreateVisual( image );
  Vector2 controlSize( 20.f, 30.f ), naturalSize(0,0);
  nPatchVisual.SetTransformAndSize(DefaultTransform(), controlSize );
  nPatchVisual.GetNaturalSize( naturalSize );
  DALI_TEST_EQUALS( naturalSize, Vector2( ninePatchImageWidth-2, ninePatchImageHeight-2 ), TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualFactoryGetNPatchVisual4(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetNPatchVisual4: Request n-patch visual with an image url" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  const unsigned int ninePatchImageHeight = 18;
  const unsigned int ninePatchImageWidth = 28;
  StretchRanges stretchRangesX;
  stretchRangesX.PushBack( Uint16Pair( 2, 3 ) );
  stretchRangesX.PushBack( Uint16Pair( 5, 7 ) );
  stretchRangesX.PushBack( Uint16Pair( 12, 15 ) );
  StretchRanges stretchRangesY;
  stretchRangesY.PushBack( Uint16Pair( 4, 5 ) );
  stretchRangesY.PushBack( Uint16Pair( 8, 12 ) );
  stretchRangesY.PushBack( Uint16Pair( 15, 16 ) );
  stretchRangesY.PushBack( Uint16Pair( 25, 27 ) );
  Integration::ResourcePointer ninePatchResource = CustomizeNinePatch( application, ninePatchImageWidth, ninePatchImageHeight, stretchRangesX, stretchRangesY );

  Visual::Base visual = factory.CreateVisual( TEST_NPATCH_FILE_NAME, ImageDimensions() );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual, 1u,
                    ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                    ninePatchResource );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualFactoryGetNPatchVisualN1(void)
{
  //This should still load but display an error image

  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetNPatchVisualN: Request n-patch visual with an invalid image url" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Visual::Base visual = factory.CreateVisual( "ERROR.9.jpg", ImageDimensions() );
  DALI_TEST_CHECK( visual );

  //The testkit still has to load a bitmap for the broken renderer image
  Integration::Bitmap* bitmap = Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 100, 100, 100, 100 );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual, 1u,
                    ImageDimensions(),
                    Integration::ResourcePointer(bitmap) );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualFactoryGetNPatchVisualN2(void)
{
  //This should still load but display an error image

  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetNPatchVisualN: Request n-patch visual with an invalid URL" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::IMAGE );
  propertyMap.Insert( ImageVisual::Property::URL,  "ERROR.9.jpg" );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  //The testkit still has to load a bitmap for the broken renderer image
  Integration::Bitmap* bitmap = Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD);
  bitmap->GetPackedPixelsProfile()->ReserveBuffer( Pixel::RGBA8888, 100, 100, 100, 100 );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& drawTrace = gl.GetDrawTrace();
  drawTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  TestVisualRender( application, actor, visual, 1u,
                    ImageDimensions(),
                    Integration::ResourcePointer(bitmap) );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualFactoryGetNPatchVisualN3(void)
{
  // Passing in an invalid visual type so we should not get a visual

  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetNPatchVisualN: Request n-patch visual with an invalid visual type" );

  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  111 );
  propertyMap.Insert( ImageVisual::Property::URL,  "ERROR.9.jpg" );

  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( !visual );

  END_TEST;
}

int UtcDaliVisualFactoryGetSvgVisual(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetSvgVisual: Request svg visual with a svg url" );

  VisualFactory factory = VisualFactory::Get();
  Visual::Base visual = factory.CreateVisual( TEST_SVG_FILE_NAME, ImageDimensions() );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  visual.SetTransformAndSize(DefaultTransform(), Vector2(200.f, 200.f) );

  application.SendNotification();
  application.Render();

  // renderer is not added to actor until the rasterization is completed.
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  // waiting for the resource uploading
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualFactoryGetSvgVisualLarge(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetSvgVisual: Request svg visual with a svg url" );

  VisualFactory factory = VisualFactory::Get();
  Visual::Base visual = factory.CreateVisual( TEST_SVG_FILE_NAME, ImageDimensions( 2000, 2000 ) );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New(true);
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  actor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS ); // Only rasterizes when it knows control size.
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // renderer is not added to actor until the rasterization is completed.
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  DALI_TEST_EQUALS( Test::WaitForEventThreadTrigger( 1 ), true, TEST_LOCATION );

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  // waiting for the resource uploading
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  END_TEST;
}

//Creates a mesh visual from the given propertyMap and tries to load it on stage in the given application.
//This is expected to succeed, which will then pass the test.
void MeshVisualLoadsCorrectlyTest( Property::Map& propertyMap, ToolkitTestApplication& application )
{
  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  //Create a mesh visual.
  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  //Create an actor on stage to house the visual.
  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  visual.SetTransformAndSize(DefaultTransform(), Vector2( 200.f, 200.f ) );

  //Ensure set on stage.
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );

  //Attempt to render to queue resource load requests.
  application.SendNotification();
  application.Render( 0 );

  //Render again to upload the now-loaded textures.
  application.SendNotification();
  application.Render( 0 );

  Matrix testScaleMatrix;
  testScaleMatrix.SetIdentityAndScale( Vector3( 1.0, -1.0, 1.0 ) );
  Matrix actualScaleMatrix;

  //Test to see if the object has been successfully loaded.
  DALI_TEST_CHECK( application.GetGlAbstraction().GetUniformValue<Matrix>( "uObjectMatrix", actualScaleMatrix ) );
  DALI_TEST_EQUALS( actualScaleMatrix, testScaleMatrix, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  //Finish by setting off stage, and ensuring this was successful.
  actor.Unparent();
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
}

//Creates a mesh visual from the given propertyMap and tries to load it on stage in the given application.
//This is expected to fail, which will then pass the test.
void MeshVisualDoesNotLoadCorrectlyTest( Property::Map& propertyMap, ToolkitTestApplication& application )
{
  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  //Create a mesh visual.
  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  //Create an actor on stage to house the visual.
  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  visual.SetTransformAndSize(DefaultTransform(),  Vector2( 200.f, 200.f ) );

  //Ensure set on stage.
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );

  //Attempt to render to queue resource load requests.
  application.SendNotification();
  application.Render( 0 );

  //Render again to upload the now-loaded textures.
  application.SendNotification();
  application.Render( 0 );

  //Test to see if the object has not been loaded, as expected.
  Matrix scaleMatrix;
  DALI_TEST_CHECK( !application.GetGlAbstraction().GetUniformValue<Matrix>( "uObjectMatrix", scaleMatrix ) );

  //Finish by setting off stage, and ensuring this was successful.
  actor.Unparent();
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
}

//Test if mesh loads correctly when supplied with only the bare minimum requirements, an object file.
int UtcDaliVisualFactoryGetMeshVisual1(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetMeshVisual1:  Request mesh visual with a valid object file only" );


  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE,  Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );

  //Test to see if mesh loads correctly.
  MeshVisualLoadsCorrectlyTest( propertyMap, application );

  END_TEST;
}


//Test if mesh loads correctly when supplied with an object file as well as a blank material file and images directory.
int UtcDaliVisualFactoryGetMeshVisual2(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetMeshVisual2:  Request mesh visual with blank material file and images directory" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, "" );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, "" );

  //Test to see if mesh loads correctly.
  MeshVisualLoadsCorrectlyTest( propertyMap, application );

  END_TEST;
}

//Test if mesh loads correctly when supplied with all main parameters, an object file, a material file and a directory location, but duff optional parameters
int UtcDaliVisualFactoryGetMeshVisual3b(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetMeshVisual3:  Request mesh visual with all parameters correct" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, TEST_MTL_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::USE_MIPMAPPING, Color::GREEN ); // Test that wrong property types don't prevent the object load
  propertyMap.Insert( MeshVisual::Property::USE_SOFT_NORMALS, 1.0f );
  propertyMap.Insert( MeshVisual::Property::LIGHT_POSITION, 1.0f );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_DIR "/" );

  //Test to see if mesh loads correctly.
  MeshVisualLoadsCorrectlyTest( propertyMap, application );

  END_TEST;
}

//Test if mesh loads correctly when supplied with all main parameters, an object file, a material file and a directory location.
int UtcDaliVisualFactoryGetMeshVisual3(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetMeshVisual3:  Request mesh visual with all parameters correct" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, TEST_MTL_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::USE_MIPMAPPING, false );
  propertyMap.Insert( MeshVisual::Property::USE_SOFT_NORMALS, false );
  propertyMap.Insert( MeshVisual::Property::LIGHT_POSITION, Vector3::XAXIS );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_DIR "/" );

  //Test to see if mesh loads correctly.
  MeshVisualLoadsCorrectlyTest( propertyMap, application );

  END_TEST;
}

//Test if mesh visual can load a correctly supplied mesh without a normal map or gloss map in the material file.
int UtcDaliVisualFactoryGetMeshVisual4(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetMeshVisual4:  Request mesh visual with diffuse texture but not normal or gloss." );


  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, TEST_SIMPLE_MTL_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_DIR "/" );

  //Test to see if mesh loads correctly.
  MeshVisualLoadsCorrectlyTest( propertyMap, application );

  END_TEST;
}

//Test if mesh visual can load when made to use diffuse textures only.
int UtcDaliVisualFactoryGetMeshVisual5(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetMeshVisual5:  Request mesh visual and make it only use diffuse textures." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( "objectUrl", TEST_OBJ_FILE_NAME );
  propertyMap.Insert( "materialUrl", TEST_MTL_FILE_NAME );
  propertyMap.Insert( "texturesPath", TEST_RESOURCE_DIR "/" );
  propertyMap.Insert( "useMipmapping", false );
  propertyMap.Insert( "useSoftNormals", false );
  propertyMap.Insert( "lightPosition", Vector3::ZAXIS );
  propertyMap.Insert( "shadingMode", MeshVisual::ShadingMode::TEXTURED_WITH_SPECULAR_LIGHTING );

  //Test to see if mesh loads correctly.
  MeshVisualLoadsCorrectlyTest( propertyMap, application );


  END_TEST;
}

//Test if mesh visual can load when made to not use the supplied textures.
int UtcDaliVisualFactoryGetMeshVisual6(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetMeshVisual6:  Request mesh visual and make it not use any textures." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, TEST_MTL_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_DIR "/" );
  propertyMap.Insert( MeshVisual::Property::SHADING_MODE, MeshVisual::ShadingMode::TEXTURELESS_WITH_DIFFUSE_LIGHTING );

  //Test to see if mesh loads correctly.
  MeshVisualLoadsCorrectlyTest( propertyMap, application );

  END_TEST;
}
//Test if mesh visual loads correctly when light position is manually set.
int UtcDaliVisualFactoryGetMeshVisual7(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;


  tet_infoline( "UtcDaliVisualFactoryGetMeshVisual7:  Request mesh visual with custom light position." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, TEST_MTL_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_DIR "/" );
  propertyMap.Insert( MeshVisual::Property::LIGHT_POSITION, Vector3( 0.0, 1.0, 2.0 ) );

  //Test to see if mesh loads correctly.
  MeshVisualLoadsCorrectlyTest( propertyMap, application );

  END_TEST;
}

//Test if mesh visual loads correctly when supplied an object file without face normals or texture points.
//Note that this notably tests object loader functionality.
int UtcDaliVisualFactoryGetMeshVisual8(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetMeshVisual5:  Request mesh visual with normal-less object file." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_SIMPLE_OBJ_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, TEST_MTL_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_DIR "/" );

  //Test to see if mesh loads correctly.
  MeshVisualLoadsCorrectlyTest( propertyMap, application );

  END_TEST;
}

//Test if mesh visual handles the case of lacking an object file.
int UtcDaliVisualFactoryGetMeshVisualN1(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetMeshVisualN1:  Request mesh visual without object file" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, TEST_MTL_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_DIR "/" );

  //Test to see if mesh doesn't load with these properties, as expected.
  MeshVisualDoesNotLoadCorrectlyTest( propertyMap, application );


  END_TEST;
}

//Test if mesh visual handles the case of being passed invalid material and images urls.
int UtcDaliVisualFactoryGetMeshVisualN2(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetMeshVisualN2:  Request mesh visual with invalid material and images urls" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, TEST_OBJ_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, "invalid" );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, "also invalid" );

  //Test to see if mesh doesn't load with these properties, as expected.
  MeshVisualDoesNotLoadCorrectlyTest( propertyMap, application );


  END_TEST;
}

//Test if mesh visual handles the case of being passed an invalid object url
int UtcDaliVisualFactoryGetMeshVisualN3(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetMeshVisualN3:  Request mesh visual with invalid object url" );


  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::MESH );
  propertyMap.Insert( MeshVisual::Property::OBJECT_URL, "invalid" );
  propertyMap.Insert( MeshVisual::Property::MATERIAL_URL, TEST_MTL_FILE_NAME );
  propertyMap.Insert( MeshVisual::Property::TEXTURES_PATH, TEST_RESOURCE_DIR "/" );

  //Test to see if mesh doesn't load with these properties, as expected.
  MeshVisualDoesNotLoadCorrectlyTest( propertyMap, application );

  END_TEST;
}

//Creates a primitive visual with the given property map and tests to see if it correctly loads in the given application.
void TestPrimitiveVisualWithProperties( Property::Map& propertyMap, ToolkitTestApplication& application )
{
  VisualFactory factory = VisualFactory::Get();
  DALI_TEST_CHECK( factory );

  //Create a primitive visual.
  Visual::Base visual = factory.CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  //Create an actor on stage to house the visual.
  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );

  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  visual.SetTransformAndSize(DefaultTransform(),  Vector2( 200.f, 200.f ) );

  //Ensure set on stage.
  DALI_TEST_EQUALS( actor.GetRendererCount(), 1u, TEST_LOCATION );

  //Tell test application to load the visual.
  application.SendNotification();
  application.Render(0);

  Matrix testScaleMatrix;
  testScaleMatrix.SetIdentityAndScale( Vector3( 1.0, -1.0, 1.0 ) );
  Matrix actualScaleMatrix;

  //Test to see if the object has been successfully loaded.
  DALI_TEST_CHECK( application.GetGlAbstraction().GetUniformValue<Matrix>( "uObjectMatrix", actualScaleMatrix ) );
  DALI_TEST_EQUALS( actualScaleMatrix, testScaleMatrix, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  //Finish by setting off stage, and ensuring this was successful.
  actor.Unparent();
  DALI_TEST_EQUALS( actor.GetRendererCount(), 0u, TEST_LOCATION );
}

//Test if primitive shape loads correctly when supplied with only the bare minimum requirements, the shape to use.
int UtcDaliVisualFactoryGetPrimitiveVisual1(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual1:  Request primitive visual with a shape only" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::CUBE );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when supplied with all possible parameters
int UtcDaliVisualFactoryGetPrimitiveVisual2(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual2:  Request primitive visual with everything" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::CUBE );
  propertyMap.Insert( PrimitiveVisual::Property::MIX_COLOR, Vector4( 0.5, 0.5, 0.5, 1.0 ) );
  propertyMap.Insert( PrimitiveVisual::Property::SLICES, 10 );
  propertyMap.Insert( PrimitiveVisual::Property::STACKS, 20 );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_TOP_RADIUS, 30.0f );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, 40.0f );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_HEIGHT, 50.0f );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_RADIUS, 60.0f );
  propertyMap.Insert( PrimitiveVisual::Property::BEVEL_PERCENTAGE, 0.7f );
  propertyMap.Insert( PrimitiveVisual::Property::BEVEL_SMOOTHNESS, 0.8f );
  propertyMap.Insert( MeshVisual::Property::LIGHT_POSITION, Vector3( 0.9, 1.0, 1.1 ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads a sphere correctly.
int UtcDaliVisualFactoryGetPrimitiveVisual3(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual3:  Request primitive visual to display a sphere" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::MIX_COLOR, Vector4( 0.5, 0.5, 0.5, 1.0 ) );
  propertyMap.Insert( PrimitiveVisual::Property::SLICES, 10 );
  propertyMap.Insert( PrimitiveVisual::Property::STACKS, 20 );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads a conic section correctly.
int UtcDaliVisualFactoryGetPrimitiveVisual4(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual4:  Request primitive visual to display a conic section" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::CONICAL_FRUSTRUM );
  propertyMap.Insert( PrimitiveVisual::Property::MIX_COLOR, Vector4( 0.5, 0.5, 0.5, 1.0 ) );
  propertyMap.Insert( PrimitiveVisual::Property::SLICES, 10 );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_TOP_RADIUS, 30.0f );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS, 40.0f );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_HEIGHT, 50.0f );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads a bevelled cube correctly.
int UtcDaliVisualFactoryGetPrimitiveVisual5(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual5:  Request primitive visual to display a bevelled cube" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::BEVELLED_CUBE );
  propertyMap.Insert( PrimitiveVisual::Property::MIX_COLOR, Vector4( 0.5, 0.5, 0.5, 1.0 ) );
  propertyMap.Insert( PrimitiveVisual::Property::BEVEL_PERCENTAGE, 0.7f );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads an octahedron correctly.
int UtcDaliVisualFactoryGetPrimitiveVisual6(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual6:  Request primitive visual to display an octahedron" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::OCTAHEDRON );
  propertyMap.Insert( PrimitiveVisual::Property::MIX_COLOR, Vector4( 0.5, 0.5, 0.5, 1.0 ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads a cone correctly.
int UtcDaliVisualFactoryGetPrimitiveVisual7(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual7:  Request primitive visual to display a cone" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::CONE );
  propertyMap.Insert( PrimitiveVisual::Property::MIX_COLOR, Vector4( 0.5, 0.5, 0.5, 1.0 ) );
  propertyMap.Insert( PrimitiveVisual::Property::SLICES, 10 );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_TOP_RADIUS, 30.0f );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_HEIGHT, 50.0f );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when light position is manually set.
int UtcDaliVisualFactoryGetPrimitiveVisual8(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual8:  Request primitive visual with set light position" );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::MIX_COLOR, Vector4( 0.5, 0.5, 0.5, 1.0 ) );
  propertyMap.Insert( MeshVisual::Property::LIGHT_POSITION, Vector3( 0.0, 1.0, 2.0 ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when told to use too many slices.
int UtcDaliVisualFactoryGetPrimitiveVisual9(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual9:  Request primitive visual with above-cap slices." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::SLICES, Property::Value( 1000000 ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when told to use too few slices. (2 slices or less.)
int UtcDaliVisualFactoryGetPrimitiveVisual10(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual10:  Request primitive visual with too few slices." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::SLICES, Property::Value( 2 ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when told to use too many stacks.
int UtcDaliVisualFactoryGetPrimitiveVisual11(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual11:  Request primitive visual with too many stacks." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::STACKS, Property::Value( 1000000 ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when told to use too few stacks. (1 stack or less.)
int UtcDaliVisualFactoryGetPrimitiveVisual12(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual12:  Request primitive visual with too few stacks." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::STACKS, Property::Value( 1 ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when told to use invalid (zero or negative) dimensions.
int UtcDaliVisualFactoryGetPrimitiveVisual13(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual13:  Request primitive visual with invalid scale dimensions." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::SCALE_DIMENSIONS, Vector3::ZERO );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when told to use too low a bevel percentage.
int UtcDaliVisualFactoryGetPrimitiveVisual14(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual14:  Request primitive visual with too low a bevel percentage." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::BEVEL_PERCENTAGE, Property::Value( -1.0f ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when told to use too high a bevel percentage.
int UtcDaliVisualFactoryGetPrimitiveVisual15(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual15:  Request primitive visual with too high a bevel percentage." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::BEVEL_PERCENTAGE, Property::Value( 2.0f ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when told to use too low a bevel smoothness.
int UtcDaliVisualFactoryGetPrimitiveVisual16(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual16:  Request primitive visual with too low a bevel smoothness." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::BEVEL_SMOOTHNESS, Property::Value( -1.0f ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape loads correctly when told to use too high a bevel smoothness.
int UtcDaliVisualFactoryGetPrimitiveVisual17(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisual17:  Request primitive visual with too high a bevel smoothness." );

  //Set up visual properties.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );
  propertyMap.Insert( PrimitiveVisual::Property::SHAPE, PrimitiveVisual::Shape::SPHERE );
  propertyMap.Insert( PrimitiveVisual::Property::BEVEL_SMOOTHNESS, Property::Value( 2.0f ) );

  //Test to see if shape loads correctly.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

//Test if primitive shape visual handles the case of not being passed a specific shape to use.
int UtcDaliVisualFactoryGetPrimitiveVisualN1(void)
{
  //Set up test application first, so everything else can be handled.
  ToolkitTestApplication application;

  tet_infoline( "UtcDaliVisualFactoryGetPrimitiveVisualN1:  Request primitive visual without shape" );

  //Set up visual properties, without supplying shape.
  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::TYPE, Visual::PRIMITIVE );

  //Test to see if shape loads regardless of missing input.
  TestPrimitiveVisualWithProperties( propertyMap, application );

  END_TEST;
}

int UtcDaliVisualFactoryGetAnimatedImageVisual1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetAnimatedImageVisual1: Request animated image visual with a gif url" );

  VisualFactory factory = VisualFactory::Get();
  Visual::Base visual = factory.CreateVisual( TEST_GIF_FILE_NAME, ImageDimensions() );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetSize( 200.0f, 200.0f );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  // test the uniforms which used to handle the atlas rect
  // the four frames should be located inside atlas as follows: atlas size 100*100
  // -------------
  // |     |     |
  // |  0  |  1  |
  // -------------
  // |     |     |
  // |  2  |  3  |
  // -------------

  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  Property::Value atlasRectValue = renderer.GetProperty( renderer.GetPropertyIndex( "uAtlasRect" ) );
  // take into consideration the half pixel correction
  DALI_TEST_EQUALS( atlasRectValue.Get<Vector4>(), Vector4(0.5f, 0.5f, 49.5f, 49.5f)/100.f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  // waiting for the resource uploading
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  // Force the timer used by the animatedImageVisual to tick,
  Dali::Timer timer = Timer::New( 0 );
  timer.MockEmitSignal();
  application.SendNotification();
  application.Render();
  atlasRectValue = renderer.GetProperty( renderer.GetPropertyIndex( "uAtlasRect" ) );
  // take into consideration the half pixel correction
  DALI_TEST_EQUALS( atlasRectValue.Get<Vector4>(), Vector4(50.5f, 0.5f, 99.5f, 49.5f)/100.f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  // Force the timer used by the animatedImageVisual to tick,
  timer.MockEmitSignal();
  application.SendNotification();
  application.Render();
  atlasRectValue = renderer.GetProperty( renderer.GetPropertyIndex( "uAtlasRect" ) );
  // take into consideration the half pixel correction
  DALI_TEST_EQUALS( atlasRectValue.Get<Vector4>(), Vector4(0.5f, 50.5f, 49.5f, 99.5f)/100.f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  // Force the timer used by the animatedImageVisual to tick,
  timer.MockEmitSignal();
  application.SendNotification();
  application.Render();
  atlasRectValue = renderer.GetProperty( renderer.GetPropertyIndex( "uAtlasRect" ) );
  // take into consideration the half pixel correction
  DALI_TEST_EQUALS( atlasRectValue.Get<Vector4>(), Vector4(50.5f, 50.5f, 99.5f, 99.5f)/100.f, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  // Test SetOffStage().
  actor.Unparent();
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliVisualFactoryGetAnimatedImageVisual2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliVisualFactoryGetAnimatedImageVisual2: Request animated image visual with a Property::Map, test custom wrap mode and pixel area" );

  const Vector4 pixelArea(-0.5f, -0.5f, 2.f, 2.f);
  Property::Map propertyMap;
  propertyMap.Add( Visual::Property::TYPE,  Visual::IMAGE  )
             .Add( ImageVisual::Property::URL, TEST_GIF_FILE_NAME  )
             .Add( ImageVisual::Property::PIXEL_AREA, pixelArea )
             .Add( ImageVisual::Property::WRAP_MODE_U, WrapMode::MIRRORED_REPEAT )
             .Add( ImageVisual::Property::WRAP_MODE_V, WrapMode::REPEAT );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  DALI_TEST_CHECK( visual );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  TraceCallStack& textureTrace = gl.GetTextureTrace();
  textureTrace.Enable(true);
  TraceCallStack& texParameterTrace = gl.GetTexParameterTrace();
  texParameterTrace.Enable( true );

  DummyControl actor = DummyControl::New();
  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  dummyImpl.RegisterVisual( Control::CONTROL_PROPERTY_END_INDEX + 1, visual );
  actor.SetSize( 200.0f, 200.0f );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  DALI_TEST_EQUALS( textureTrace.FindMethod("BindTexture"), true, TEST_LOCATION );

  // For animated image visual, the wrapping is handled manually in shader, so the following gl function should not be called
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
