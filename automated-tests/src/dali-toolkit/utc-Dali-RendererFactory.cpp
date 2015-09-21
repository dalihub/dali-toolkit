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

void AddStretchRegionsToImage( Integration::Bitmap* image, unsigned int imageWidth, unsigned int imageHeight, const Vector4& requiredStretchBorder, Pixel::Format pixelFormat )
{
  PixelBuffer* pixbuffer = image->GetBuffer();
  unsigned int bytesPerPixel = GetBytesPerPixel( pixelFormat );

  for( unsigned int column = requiredStretchBorder.x; column < imageWidth - requiredStretchBorder.z; ++column )
  {
    unsigned int pixelOffset = column * bytesPerPixel;
    pixbuffer[ pixelOffset ] = 0x00;
    pixbuffer[ pixelOffset + 1 ] = 0x00;
    pixbuffer[ pixelOffset + 2 ] = 0x00;
    pixbuffer[ pixelOffset + 3 ] = 0xFF;
  }

  for( unsigned int row = requiredStretchBorder.y; row < imageHeight - requiredStretchBorder.w; ++row )
  {
    unsigned int pixelOffset = row * imageWidth * bytesPerPixel;
    pixbuffer[ pixelOffset ] = 0x00;
    pixbuffer[ pixelOffset + 1 ] = 0x00;
    pixbuffer[ pixelOffset + 2 ] = 0x00;
    pixbuffer[ pixelOffset + 3 ] = 0xFF;
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
                                                 const Vector4& requiredStretchBorder,
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
  AddStretchRegionsToImage( bitmap, ninePatchImageWidth, ninePatchImageHeight, requiredStretchBorder, pixelFormat );

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
  propertyMap.Insert("renderer-type", "color-renderer");
  propertyMap.Insert("blend-color", testColor);

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

  Vector4 actualValue(Vector4::ZERO);
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
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize(Vector2(200.f, 200.f));
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();

  application.SendNotification();
  application.Render(0);

  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBlendColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, testColor, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetLinearGradientRenderer(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRendererFactoryGetRadialGradientRenderer");

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "gradient-renderer");

  Vector2 start(-1.f, -1.f);
  Vector2 end(1.f, 1.f);
  propertyMap.Insert("gradient-start-position", start);
  propertyMap.Insert("gradient-end-position", end);
  propertyMap.Insert("gradient-spread-method", "repeat");

  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.2f );
  stopOffsets.PushBack( 0.8f );
  propertyMap.Insert("gradient-stop-offset", stopOffsets);

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("gradient-stop-color", stopColors);

  ControlRenderer controlRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  Vector2 size(200.f, 200.f);
  actor.SetSize(size);
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetOnStage( actor );
  controlRenderer.SetSize(size);

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  // A lookup texture is generated and pass to shader as sampler
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  application.SendNotification();
  application.Render(0);

  END_TEST;
}

int UtcDaliRendererFactoryGetRadialGradientRenderer(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliRendererFactoryGetRadialGradientRenderer");

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "gradient-renderer");

  Vector2 center(100.f, 100.f);
  float radius = 100.f;
  propertyMap.Insert("gradient-units", "user-space");
  propertyMap.Insert("gradient-center", center);
  propertyMap.Insert("gradient-radius", radius);

  Property::Array stopOffsets;
  stopOffsets.PushBack( 0.0f );
  stopOffsets.PushBack( 1.f );
  propertyMap.Insert("gradient-stop-offset", stopOffsets);

  Property::Array stopColors;
  stopColors.PushBack( Color::RED );
  stopColors.PushBack( Color::GREEN );
  propertyMap.Insert("gradient-stop-color", stopColors);

  ControlRenderer controlRenderer = factory.GetControlRenderer(propertyMap);
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  Vector2 size(200.f, 200.f);
  actor.SetSize(size);
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize(size);
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  // A lookup texture is generated and pass to shader as sampler
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render(0);

  Matrix3 alignMatrix( radius, 0.f, 0.f, 0.f, radius, 0.f, center.x, center.y, 1.f );
  alignMatrix.Invert();

  Matrix3 actualValue( Matrix3::IDENTITY );
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
  propertyMap.Insert( "renderer-type", "image-renderer" );
  propertyMap.Insert( "image-url", TEST_IMAGE_FILE_NAME );

  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize( Vector2(200.f, 200.f) );
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render();

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD)));
  }

  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc));

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

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
  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize( Vector2(200.f, 200.f) );
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render();

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, Integration::ResourcePointer(Integration::Bitmap::New(Integration::Bitmap::BITMAP_2D_PACKED_PIXELS, ResourcePolicy::OWNED_DISCARD)));
  }

  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc));

  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetNPatchRenderer1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetNPatchRenderer1: Request n-patch renderer with a Property::Map" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  const unsigned int ninePatchImageHeight = 18;
  const unsigned int ninePatchImageWidth = 28;
  const Vector4 requiredStretchBorder( 3, 4, 5, 6 );
  Integration::ResourcePointer ninePatchResource = CustomizeNinePatch( application, ninePatchImageWidth, ninePatchImageHeight, requiredStretchBorder );

  Property::Map propertyMap;
  propertyMap.Insert( "renderer-type", "n-patch-renderer" );
  propertyMap.Insert( "image-url", TEST_NPATCH_FILE_NAME );

  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize( Vector2(200.f, 200.f) );
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render();

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, ninePatchResource );
  }

  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc));

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryGetNPatchRenderer2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryGetNPatchRenderer2: Request n-patch renderer with an image url" );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  const unsigned int ninePatchImageHeight = 18;
  const unsigned int ninePatchImageWidth = 28;
  const Vector4 requiredStretchBorder( 3, 4, 5, 6 );
  Integration::ResourcePointer ninePatchResource = CustomizeNinePatch( application, ninePatchImageWidth, ninePatchImageHeight, requiredStretchBorder );

  ControlRenderer controlRenderer = factory.GetControlRenderer( TEST_NPATCH_FILE_NAME );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize( Vector2(200.f, 200.f) );
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render();

  Integration::ResourceRequest* request = application.GetPlatform().GetRequest();
  if(request)
  {
    application.GetPlatform().SetResourceLoaded(request->GetId(), request->GetType()->id, ninePatchResource );
  }

  application.Render();
  application.SendNotification();

  DALI_TEST_CHECK(application.GetPlatform().WasCalled(TestPlatformAbstraction::LoadResourceFunc));

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
  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize( Vector2(200.f, 200.f) );
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render();

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
  propertyMap.Insert( "renderer-type", "n-patch-renderer" );
  propertyMap.Insert( "image-url", 111 );

  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );
  DALI_TEST_CHECK( controlRenderer );

  Actor actor = Actor::New();
  actor.SetSize( 200.f, 200.f );
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetSize( Vector2(200.f, 200.f) );
  controlRenderer.SetOnStage( actor );

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  DALI_TEST_CHECK( actor.GetRendererAt(0u).GetMaterial().GetNumberOfSamplers() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render();

  int textureUnit = -1;
  DALI_TEST_CHECK( gl.GetUniformValue< int >( "sTexture", textureUnit ) );
  DALI_TEST_EQUALS( textureUnit, 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRendererFactoryResetRenderer1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliRendererFactoryResetRenderer1" );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );

  RendererFactory factory = RendererFactory::Get();
  DALI_TEST_CHECK( factory );

  ControlRenderer controlRenderer = factory.GetControlRenderer( Color::RED );
  DALI_TEST_CHECK( controlRenderer );
  controlRenderer.SetSize(Vector2(200.f, 200.f));
  controlRenderer.SetOnStage( actor );
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  TestGlAbstraction& gl = application.GetGlAbstraction();
  application.SendNotification();
  application.Render(0);
  Vector4 actualValue(Vector4::ZERO);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBlendColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::RED, TEST_LOCATION );

  bool isNewRenderer = factory.ResetRenderer( controlRenderer, Color::GREEN );
  DALI_TEST_CHECK( !isNewRenderer );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( gl.GetUniformValue<Vector4>( "uBlendColor", actualValue ) );
  DALI_TEST_EQUALS( actualValue, Color::GREEN, TEST_LOCATION );

  Image bufferImage = CreateBufferImage( 100, 200, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  isNewRenderer = factory.ResetRenderer( controlRenderer, bufferImage );
  DALI_TEST_CHECK( isNewRenderer );

  Actor actor2 = Actor::New();
  actor2.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor2 );
  controlRenderer.SetSize(Vector2(200.f, 200.f));
  controlRenderer.SetOnStage( actor2 );
  application.SendNotification();
  application.Render(0);
  Image samplerImage = actor2.GetRendererAt(0u).GetMaterial().GetSamplerAt(0u).GetImage();
  DALI_TEST_CHECK( BufferImage::DownCast( samplerImage ) );

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
  Image samplerImage = actor.GetRendererAt(0u).GetMaterial().GetSamplerAt(0u).GetImage();
  DALI_TEST_CHECK( ResourceImage::DownCast( samplerImage ) );

  Image bufferImage = CreateBufferImage( 100, 200, Vector4( 1.f, 1.f, 1.f, 1.f ) );
  bool isNewRenderer = factory.ResetRenderer( controlRenderer, bufferImage );
  DALI_TEST_CHECK( !isNewRenderer );
  application.SendNotification();
  application.Render(0);
  samplerImage = actor.GetRendererAt(0u).GetMaterial().GetSamplerAt(0u).GetImage();
  DALI_TEST_CHECK( BufferImage::DownCast( samplerImage ) );

  isNewRenderer = factory.ResetRenderer( controlRenderer, Color::RED );
  DALI_TEST_CHECK( isNewRenderer );

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
