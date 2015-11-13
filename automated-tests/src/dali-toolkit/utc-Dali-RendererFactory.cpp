/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/renderer.h>
#include <dali/devel-api/rendering/material.h>
#include <dali/devel-api/rendering/shader.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
typedef NinePatchImage::StretchRanges StretchRanges;

const char* TEST_IMAGE_FILE_NAME =  "gallery_image_01.jpg";
const char* TEST_NPATCH_FILE_NAME =  "gallery_image_01.9.jpg";

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
  platform.SetResourceLoaded( 0, Dali::Integration::ResourceBitmap, resourcePtr );

  return resourcePtr;
}

void TestControlRendererRender( ToolkitTestApplication& application,
                                Actor& actor,
                                ControlRenderer& controlRenderer,
                                std::size_t expectedSamplers = 0,
                                ImageDimensions imageDimensions = ImageDimensions(),
                                Integration::ResourcePointer resourcePtr = Integration::ResourcePointer())
{
  if( resourcePtr )
  {
    // set the image size, for test case, this needs to be set before loading started
    application.GetPlatform().SetClosestImageSize(  Vector2(imageDimensions.GetWidth(), imageDimensions.GetHeight()) );
  }

  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize( Vector2(200.f, 200.f) );
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfTextures() == expectedSamplers );

  application.SendNotification();
  application.Render();

  if( resourcePtr )
  {
    Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
    if(request)
    {
      application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, resourcePtr );
    }
  }

  application.Render();
  application.SendNotification();

  if( resourcePtr )
  {
    DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc));
  }

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

}

} // namespace


void dali_renderer_factory_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_renderer_factory_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliRendererFactoryGet(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactory" );

  //Register type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "RendererFactory" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );

  RendererFactory factory;
  factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  RendererFactory newFactory = RendererFactory::Get();
  DALI_TEST_CHECK( newFactory );

  // Check that renderer factory is a singleton
  DALI_TEST_CHECK(factory == newFactory);

  END_TEST;
}

int UtcDaliRendererFactoryCopyAndAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryCopyAndAssignment" );
  RendererFactory factory = RendererFactory::Get();

  RendererFactory factoryCopy( factory );
  DALI_TEST_CHECK(factory == factoryCopy);

  RendererFactory emptyFactory;
  RendererFactory emptyFactoryCopy( emptyFactory );
  DALI_TEST_CHECK(emptyFactory == emptyFactoryCopy);

  RendererFactory factoryEquals;
  factoryEquals = factory;
  DALI_TEST_CHECK(factory == factoryEquals);

  RendererFactory emptyFactoryEquals;
  emptyFactoryEquals = emptyFactory;
  DALI_TEST_CHECK( emptyFactory == emptyFactoryEquals );

  //self assignment
  factory = factory;
  DALI_TEST_CHECK( factory = factoryCopy );

  END_TEST;
}

int UtcDaliRendererFactoryGetColorRenderer1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetColorRenderer1:  Request color renderer with a Property::Map" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  Vector4 testColor( 1.f, 0.5f, 0.3f, 0.2f );
  propertyMap.Insert("rendererType",  "colorRenderer");
  propertyMap.Insert("blendColor",  testColor);

  ControlRenderer controlRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  TestControlRendererRender( application, actor, controlRenderer );

  Vector4 actualValue(Vector4::ZERO);
  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBlendColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, testColor, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetColorRenderer2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetColorRenderer2: Request color renderer with a Vector4" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Vector4 testColor( 1.f, 0.5f, 0.3f, 0.2f );
  ControlRenderer controlRenderer = factory.GetControlRenderer(testColor);
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  TestControlRendererRender( application, actor, controlRenderer );

  Vector4 actualValue(Vector4::ZERO);
  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBlendColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, testColor, TEST_LOCATION );

  controlRenderer.SetOffStage( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliRendererFactoryGetBorderRenderer1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetBorderRenderer1:  Request border renderer with a Property::Map" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  Vector4 testColor( 1.f, 0.5f, 0.3f, 0.2f );
  float testSize = 5.f;
  propertyMap.Insert("rendererType",  "borderRenderer");
  propertyMap.Insert("borderColor",  testColor);
  propertyMap.Insert("borderSize",  testSize);

  ControlRenderer controlRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize(Vector2(200.f, 200.f));
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualColor(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBorderColor", actualColor ) );
  DALI_TEST_EQUALS( actualColor, testColor, TEST_LOCATION );

  float actualSize = 0.f;
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uBorderSize", actualSize ) );
  DALI_TEST_EQUALS( actualSize, testSize, TEST_LOCATION );

  controlRenderer.SetOffStage( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliRendererFactoryGetBorderRenderer2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetBorderRenderer2:  Request border renderer with a borderSize and a borderColor" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Vector4 testColor( 1.f, 0.5f, 0.3f, 0.2f );
  float testSize = 5.f;

  ControlRenderer controlRenderer = factory.GetControlRenderer(testSize, testColor);
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize(Vector2(200.f, 200.f));
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualColor(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBorderColor", actualColor ) );
  DALI_TEST_EQUALS( actualColor, testColor, TEST_LOCATION );

  float actualSize = 0.f;
  DALI_TEST_CHECK( gl.GetUniformValue<float>( "uBorderSize", actualSize ) );
  DALI_TEST_EQUALS( actualSize, testSize, TEST_LOCATION );

  END_TEST;
}


int UtcDaliRendererFactoryGetLinearGradientRenderer(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRendererFactoryGetRadialGradientRenderer");

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "gradientRenderer");

  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert("gradientStartPosition",   start);
  propertyMap.Insert("gradientEndPosition",   end);
  propertyMap.Insert("gradientSpreadMethod",   "repeat");

  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.2f );
  stopOffsets.PushBack( 0.8f );
  propertyMap.Insert("gradientStopOffset",   stopOffsets);

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("gradientStopColor",   stopColors);

  ControlRenderer controlRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( controlRenderer );

  // A lookup texture is generated and pass to shader as sampler
  Actor actor = Actor::New();
  TestControlRendererRender( application, actor, controlRenderer, 1u );

  controlRenderer.SetOffStage( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliRendererFactoryGetRadialGradientRenderer(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRendererFactoryGetRadialGradientRenderer");

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert("rendererType",  "gradientRenderer");

  Vector2 center(100.f, 100.f);
  float radius = 100.f;
  propertyMap.Insert("gradientUnits",  "userSpace");
  propertyMap.Insert("gradientCenter",  center);
  propertyMap.Insert("gradientRadius",  radius);

  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.0f );
  stopOffsets.PushBack( 1.f );
  propertyMap.Insert("gradientStopOffset",   stopOffsets);

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("gradientStopColor",   stopColors);

  ControlRenderer controlRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( controlRenderer );

  // A lookup texture is generated and pass to shader as sampler
  Actor actor = Actor::New();
  TestControlRendererRender( application, actor, controlRenderer, 1u );

  Matrix3 alignMatrix( radius, 0.f, 0.f, 0.f, radius, 0.f, center.x, center.y, 1.f );
  alignMatrix.Invert();

  Matrix3 actualValue( Matrix3::IDENTITY );
  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.GetUniformValue<Matrix3>( "uAlignmentMatrix", actualValue ) );
  DALI_TEST_EQUALS( actualValue, alignMatrix, Math::MACHINE_EPSILON_100, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetImageRenderer1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetImageRenderer1: Request image renderer with a Property::Map" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( "rendererType",  "imageRenderer" );
  propertyMap.Insert( "imageUrl",  TEST_IMAGE_FILE_NAME );

  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.
  TestControlRendererRender( application, actor, controlRenderer, 1u,
                             ImageDimensions(512, 513),
                             Integration::ResourcePointer(Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD)));

  TestGlAbstraction& gl = application.GetGlAbstraction();
  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  controlRenderer.SetOffStage( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliRendererFactoryGetImageRenderer2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetImageRenderer2: Request image renderer with an image handle" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Image image = ResourceImage::New(TEST_IMAGE_FILE_NAME);
  ControlRenderer controlRenderer = factory.GetControlRenderer( image );

  Actor actor = Actor::New();
  // For tesing the LoadResourceFunc is called, a big image size should be set, so the atlasing is not applied.
  // Image with a size smaller than 512*512 will be uploaded as a part of the atlas.
  TestControlRendererRender( application, actor, controlRenderer, 1u,
                             ImageDimensions(512, 513),
                             Integration::ResourcePointer(Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD)) );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetNPatchRenderer1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetNPatchRenderer1: Request 9-patch renderer with a Property::Map" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  const unsigned int ninePatchImageHeight = 18;
  const unsigned int ninePatchImageWidth = 28;
  StretchRanges stretchRangesX;
  stretchRangesX.PushBack( Uint16Pair( 2, 3 ) );
  StretchRanges stretchRangesY;
  stretchRangesY.PushBack( Uint16Pair( 4, 5 ) );
  Integration::ResourcePointer ninePatchResource = CustomizeNinePatch( application, ninePatchImageWidth, ninePatchImageHeight, stretchRangesX, stretchRangesY );

  Property::Map propertyMap;
  propertyMap.Insert( "rendererType",  "nPatchRenderer" );
  propertyMap.Insert( "imageUrl",  TEST_NPATCH_FILE_NAME );
  {
    tet_infoline( "whole grid" );
    ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );
    DALI_TEST_CHECK( controlRenderer );

    Actor actor = Actor::New();
    TestControlRendererRender( application, actor, controlRenderer, 1u,
                               ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                               ninePatchResource );

    TestGlAbstraction& gl = application.GetGlAbstraction();
    int textureUnit = -1;
    DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
    DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );
  }

  propertyMap.Insert( "borderOnly",  true );
  {
    tet_infoline( "border only" );
    ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );
    DALI_TEST_CHECK( controlRenderer );

    Actor actor = Actor::New();
    TestControlRendererRender( application, actor, controlRenderer, 1u,
                               ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                               ninePatchResource );

    TestGlAbstraction& gl = application.GetGlAbstraction();
    int textureUnit = -1;
    DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
    DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliRendererFactoryGetNPatchRenderer2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetNPatchRenderer2: Request n-patch renderer with a Property::Map" );

  RendererFactory factory = RendererFactory::Get();
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
  propertyMap.Insert( "rendererType",  "nPatchRenderer" );
  propertyMap.Insert( "imageUrl",  TEST_NPATCH_FILE_NAME );
  {
    ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );
    DALI_TEST_CHECK( controlRenderer );

    Actor actor = Actor::New();
    TestControlRendererRender( application, actor, controlRenderer, 1u,
                               ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                               ninePatchResource );

    TestGlAbstraction& gl = application.GetGlAbstraction();
    int textureUnit = -1;
    DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
    DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

    controlRenderer.SetOffStage( actor );
    DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  }

  propertyMap.Insert( "borderOnly",  true );
  {
    tet_infoline( "border only" );
    ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );
    DALI_TEST_CHECK( controlRenderer );

    Actor actor = Actor::New();
    TestControlRendererRender( application, actor, controlRenderer, 1u,
                               ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                               ninePatchResource );

    TestGlAbstraction& gl = application.GetGlAbstraction();
    int textureUnit = -1;
    DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
    DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

    controlRenderer.SetOffStage( actor );
    DALI_TEST_CHECK( actor.GetRendererCount() == 0u );
  }

  END_TEST;
}

int UtcDaliRendererFactoryGetNPatchRenderer3(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetNPatchRenderer3: Request 9-patch renderer with an image url" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  const unsigned int ninePatchImageHeight = 18;
  const unsigned int ninePatchImageWidth = 28;
  StretchRanges stretchRangesX;
  stretchRangesX.PushBack( Uint16Pair( 2, 3 ) );
  StretchRanges stretchRangesY;
  stretchRangesY.PushBack( Uint16Pair( 4, 5 ) );
  Integration::ResourcePointer ninePatchResource = CustomizeNinePatch( application, ninePatchImageWidth, ninePatchImageHeight, stretchRangesX, stretchRangesY );

  ControlRenderer controlRenderer = factory.GetControlRenderer( TEST_NPATCH_FILE_NAME );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  TestControlRendererRender( application, actor, controlRenderer, 1u,
                             ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                             ninePatchResource );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetNPatchRenderer4(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetNPatchRenderer4: Request n-patch renderer with an image url" );

  RendererFactory factory = RendererFactory::Get();
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

  ControlRenderer controlRenderer = factory.GetControlRenderer( TEST_NPATCH_FILE_NAME );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  TestControlRendererRender( application, actor, controlRenderer, 1u,
                             ImageDimensions(ninePatchImageWidth, ninePatchImageHeight),
                             ninePatchResource );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetNPatchRendererN1(void)
{
  //This should still load but display an error image

  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetNPatchRendererN: Request n-patch renderer with an invalid image url" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  ControlRenderer controlRenderer = factory.GetControlRenderer( "ERROR.9.jpg" );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  //The testkit still has to load a bitmap for the broken renderer image
  TestControlRendererRender( application, actor, controlRenderer, 1u,
                             ImageDimensions(),
                             Integration::ResourcePointer(Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD)) );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetNPatchRendererN2(void)
{
  //This should still load but display an error image

  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetNPatchRendererN: Request n-patch renderer with an invalid Property::Map" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert( "rendererType",  "nPatchRenderer" );
  propertyMap.Insert( "imageUrl",  111 );

  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  //The testkit still has to load a bitmap for the broken renderer image
  TestControlRendererRender( application, actor, controlRenderer, 1u,
                             ImageDimensions(),
                             Integration::ResourcePointer(Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD)) );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryResetRenderer1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryResetRenderer1" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  ControlRenderer controlRenderer = factory.GetControlRenderer( Color::RED );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  TestControlRendererRender( application, actor, controlRenderer );

  Vector4 actualValue(Vector4::ZERO);
  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBlendColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::RED, TEST_LOCATION );

  factory.ResetRenderer( controlRenderer, actor, Color::GREEN );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBlendColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  Image bufferImage = CreateBufferImage( 100, 200, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  factory.ResetRenderer( controlRenderer, actor, bufferImage );

  Actor actor2 = Actor::New();
  actor2.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor2 );
  controlRenderer.SetSize(Vector2(200.f, 200.f));
  controlRenderer.SetOnStage( actor2 );
  application.SendNotification();
  application.Render(0);

  END_TEST;
}

int UtcDaliRendererFactoryResetRenderer2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryResetRenderer2" );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Image resourceImage = ResourceImage::New(TEST_IMAGE_FILE_NAME);
  ControlRenderer controlRenderer = factory.GetControlRenderer( resourceImage );
  DALI_TEST_CHECK( controlRenderer );
  controlRenderer.SetSize(Vector2(200.f, 200.f));
  controlRenderer.SetOnStage( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  application.SendNotification();
  application.Render(0);

  Image bufferImage = CreateBufferImage( 100, 200, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  factory.ResetRenderer( controlRenderer, actor, bufferImage );
  application.SendNotification();
  application.Render(0);

  factory.ResetRenderer( controlRenderer, actor, Color::RED );

  Actor actor2 = Actor::New();
  actor2.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor2 );
  controlRenderer.SetSize(Vector2(200.f, 200.f));
  controlRenderer.SetOnStage( actor2 );
  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render(0);
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBlendColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::RED, TEST_LOCATION );

  END_TEST;
}
