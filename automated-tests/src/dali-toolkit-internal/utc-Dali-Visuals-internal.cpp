/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <toolkit-text-utils.h>
#include <toolkit-event-thread-callback.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/npatch-loader.h>
#include <dummy-visual.h>
#include <../dali-toolkit/dali-toolkit-test-utils/dummy-control.h>
#include <dali-toolkit/devel-api/visuals/arc-visual-properties-devel.h>

using namespace Dali;
using namespace Toolkit;

namespace
{

const char* TEST_VECTOR_IMAGE_FILE_NAME =  TEST_RESOURCE_DIR  "/insta_camera.json";

}

int UtcDaliVisualAction(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Register an ImageVisual and and perform an Action on Visual directly" );
  Vector2 controlSize( 20.f, 30.f );

  //Created DummyVisual
  Property::Map settings;
  Toolkit::Internal::DummyVisualPtr dummyVisualPtr = Toolkit::Internal::DummyVisual::New( settings );

  DummyControl dummyControl = DummyControl::New( true );
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  tet_infoline( "Register visual and stage control" );

  Toolkit::Visual::Base visualBaseHandle = Toolkit::Visual::Base( dummyVisualPtr.Get() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visualBaseHandle );
  dummyControl.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  Stage::GetCurrent().Add( dummyControl );

  application.SendNotification();
  application.Render();

  tet_infoline( "Check action counter is 0 before DoAction" );
  DALI_TEST_EQUALS( dummyVisualPtr->GetActionCounter() , 0, TEST_LOCATION );

  tet_infoline( "Perform TEST_ACTION action on Visual. Should increase the action counter" );

  Property::Map attributes;
  Toolkit::Internal::Visual::Base& internalVisualBase =  GetImplementation( visualBaseHandle );
  internalVisualBase.DoAction( Dali::Toolkit::Internal::DummyVisual::TEST_ACTION, attributes );
  application.SendNotification();
  DALI_TEST_EQUALS( dummyVisualPtr->GetActionCounter() , 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualActionNotImplemented(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Register an ImageVisual and and perform an Action on a Visual which does not support any Actions" );
  Vector2 controlSize( 20.f, 30.f );

  //Created DummyVisual
  Property::Map settings;
  Toolkit::Internal::DummyVisualPtr dummyVisualPtr = Toolkit::Internal::DummyVisual::New( settings );

  DummyControl dummyControl = DummyControl::New( true );
  Impl::DummyControl& dummyImpl = static_cast<Impl::DummyControl&>(dummyControl.GetImplementation());

  tet_infoline( "Register visual and stage control" );

  VisualFactory factory = VisualFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap.Insert(ColorVisual::Property::MIX_COLOR,  Color::BLUE);
  Visual::Base visual = factory.CreateVisual( propertyMap );

  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
  dummyControl.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  Stage::GetCurrent().Add( dummyControl );

  application.SendNotification();
  application.Render();

  tet_infoline( "Check action counter is 0 before DoAction" );
  DALI_TEST_EQUALS( dummyVisualPtr->GetActionCounter() , 0, TEST_LOCATION );

  tet_infoline( "Perform TEST_ACTION action on Color Visual which does not support it.. Should not increment the action counter" );
  Property::Map attributes;
  GetImplementation( visual ).DoAction( Dali::Toolkit::Internal::DummyVisual::TEST_ACTION, attributes );
  application.SendNotification();
  DALI_TEST_EQUALS( dummyVisualPtr->GetActionCounter() , 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVisualSetProperties(void)
{
  ToolkitTestApplication application;
  tet_infoline( "Register a visual and SetProperties" );

  Toolkit::Internal::VisualFactoryCache* factoryCache = new Toolkit::Internal::VisualFactoryCache(false);

  //Created ColorVisual
  Property::Map propertyMap1;
  propertyMap1.Insert(Visual::Property::TYPE,  Visual::COLOR);
  propertyMap1.Insert(ColorVisual::Property::MIX_COLOR,  Color::RED);
  Toolkit::Internal::ColorVisualPtr colorVisualPtr = Toolkit::Internal::ColorVisual::New( *factoryCache, propertyMap1 );

  DummyControl dummyControl = DummyControl::New( true );
  Impl::DummyControl& dummyImpl = static_cast< Impl::DummyControl& >( dummyControl.GetImplementation() );

  Toolkit::Visual::Base visualBaseHandle = Toolkit::Visual::Base( colorVisualPtr.Get() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visualBaseHandle );
  dummyControl.SetProperty( Actor::Property::SIZE, Vector2( 200.f, 200.f ) );
  Stage::GetCurrent().Add( dummyControl );

  application.SendNotification();
  application.Render();

  Property::Map propertyMap;
  propertyMap.Insert( Visual::Property::MIX_COLOR,  Color::BLUE );

  colorVisualPtr->SetProperties( propertyMap );
  application.SendNotification();
  application.Render();

  Property::Map resultMap;
  visualBaseHandle.CreatePropertyMap( resultMap );

  Property::Value* colorValue = resultMap.Find( Visual::Property::MIX_COLOR,  Property::VECTOR4 );
  DALI_TEST_CHECK( colorValue );
  DALI_TEST_CHECK( colorValue->Get< Vector4 >() == Color::BLUE );

  delete factoryCache;

  END_TEST;
}

int UtcDaliNPatchBufferGetRedOffsetAndMask(void)
{
  TestApplication application;

  tet_infoline("UtcDaliNPatchBufferGetRedOffsetAndMask");

  int byteOffset = 0;
  int bitMask = 0;

  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::A8, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::L8, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::LA88, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );

  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::RGB888, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0xff );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::RGB8888, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0xff );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::RGBA8888, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0xff );

  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::BGR8888, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 2 && bitMask == 0xff );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::BGRA8888, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 2 && bitMask == 0xff );

  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::RGB565, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0xf8 );

  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::BGR565, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 1 && bitMask == 0x1f );

  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::RGBA4444, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0xf0 );

  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::BGRA4444, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 1 && bitMask == 0xf0 );

  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::RGBA5551, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0xf8 );

  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::BGRA5551, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 1 && bitMask == 0x1e );

  // Compressed formats are not supported
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::INVALID, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_R11_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SIGNED_R11_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RG11_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SIGNED_RG11_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGB8_ETC2, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ETC2, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGB8_ETC1, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGB_PVRTC_4BPPV1, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA8_ETC2_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );

  // Not supported
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::RGB16F, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Toolkit::Internal::NPatchBuffer::GetRedOffsetAndMask( Pixel::RGB32F, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualCreateInstancePropertyMap(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualCreateInstancePropertyMap" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE,  DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME  );

  // request AnimatedVectorImageVisual with a property map
  VisualFactory factory = VisualFactory::Get();
  Visual::Base visual = factory.CreateVisual( propertyMap );
  Toolkit::Internal::Visual::Base& visualImpl = GetImplementation( visual );

  Property::Map resultMap;
  visualImpl.CreateInstancePropertyMap( resultMap );

  // check the property values from the returned map from a visual
  DALI_TEST_CHECK( resultMap.Empty() );   // Now the map is empty

  END_TEST;
}

int UtcDaliAnimatedVectorImageVisualSetProperties(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliAnimatedVectorImageVisualSetProperties" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ANIMATED_VECTOR_IMAGE )
             .Add( ImageVisual::Property::URL, TEST_VECTOR_IMAGE_FILE_NAME  )
             .Add( DevelImageVisual::Property::LOOP_COUNT, 3  )
             .Add( DevelImageVisual::Property::PLAY_RANGE, Vector2( 0.2f, 0.8f )  );

  Visual::Base visual = VisualFactory::Get().CreateVisual( propertyMap );
  Toolkit::Internal::Visual::Base& visualImpl = GetImplementation( visual );
  DALI_TEST_CHECK( visual );

  DummyControl actor = DummyControl::New( true );
  DummyControlImpl& dummyImpl = static_cast< DummyControlImpl& >( actor.GetImplementation() );
  dummyImpl.RegisterVisual( DummyControl::Property::TEST_VISUAL, visual );
  actor.SetProperty( Actor::Property::SIZE, Vector2( 200.0f, 200.0f ) );
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render();

  // renderer is added to actor
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  Renderer renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  Property::Map propertyMap1;
  propertyMap1.Add( DevelImageVisual::Property::LOOP_COUNT, 1  )
              .Add( DevelImageVisual::Property::PLAY_RANGE, Vector2( 0.4f, 0.6f )  );

  visualImpl.SetProperties( propertyMap1 );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );
  renderer = actor.GetRendererAt( 0u );
  DALI_TEST_CHECK( renderer );

  actor.Unparent( );
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  END_TEST;
}

int UtcDaliArcVisualCreateInstancePropertyMap(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliArcVisualCreateInstancePropertyMap" );

  Property::Map propertyMap;
  propertyMap.Add( Toolkit::Visual::Property::TYPE, DevelVisual::ARC )
             .Add( DevelArcVisual::Property::THICKNESS, 20.0f );

  // request ArcVisual with a property map
  VisualFactory factory = VisualFactory::Get();
  Visual::Base visual = factory.CreateVisual( propertyMap );
  Toolkit::Internal::Visual::Base& visualImpl = GetImplementation( visual );

  Property::Map resultMap;
  visualImpl.CreateInstancePropertyMap( resultMap );

  // check the property values from the returned map from a visual
  DALI_TEST_CHECK( resultMap.Empty() );   // Now the map is empty

  END_TEST;
}
