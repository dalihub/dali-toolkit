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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/dali-toolkit.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}


enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};


#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliPageTurnEffectApply, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPageTurnEffectConstruct, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

// Create bitmap image
static BitmapImage CreateBitmapImage()
{
  BitmapImage image = BitmapImage::New(4,4,Pixel::RGBA8888);

  PixelBuffer* pixbuf = image.GetBuffer();

  // Using a 4x4 image gives a better blend with the GL implementation
  // than a 3x3 image
  for(size_t i=0; i<16; i++)
  {
    pixbuf[i*4+0] = 0xFF;
    pixbuf[i*4+1] = 0xFF;
    pixbuf[i*4+2] = 0xFF;
    pixbuf[i*4+3] = 0xFF;
  }

  return image;
}

static void UtcDaliPageTurnEffectApply()
{
  ToolkitTestApplication application;

  BitmapImage image = CreateBitmapImage();

  Toolkit::PageTurnEffect pageTurnEffect = Toolkit::PageTurnEffect::New();
  Toolkit::PageTurnEffect pageTurnEffect2 = Toolkit::PageTurnEffect::New(false);

  ImageActor pageActor = ImageActor::New( image );
  ImageActor backPageActor = ImageActor::New( image );
  pageActor.Add( backPageActor );

  pageTurnEffect.SetIsTurningBack( true );
  pageTurnEffect.SetShadowWidth( 0.0f );
  pageTurnEffect.SetSpineShadowParameter( Vector2( 0.0f, 0.0f ) );

  pageActor.SetShaderEffect( pageTurnEffect );
  Stage::GetCurrent().Add( pageActor );

  application.SendNotification();
  application.Render();

  const Vector2 pageSize( 0.0f, 0.0f );
  pageTurnEffect.SetPageSize( pageSize );

  const Vector2 originalCenter( 0.0f, 0.0f );
  pageTurnEffect.SetOriginalCenter( originalCenter );

  const Vector2 currentCenter( 0.0f, 0.0f );
  pageTurnEffect.SetCurrentCenter( currentCenter );

  application.SendNotification();
  application.Render();

  TestGlAbstraction& gl = application.GetGlAbstraction();
  DALI_TEST_CHECK( gl.CheckUniformValue( pageTurnEffect.GetPageSizePropertyName().c_str(), pageSize ) );
  DALI_TEST_CHECK( gl.CheckUniformValue( pageTurnEffect.GetOriginalCenterPropertyName().c_str(), originalCenter ) );
  DALI_TEST_CHECK( gl.CheckUniformValue( pageTurnEffect.GetCurrentCenterPropertyName().c_str(), currentCenter ) );
}

static void UtcDaliPageTurnEffectConstruct()
{
  ToolkitTestApplication application;

  Toolkit::PageTurnEffect* effect = new Toolkit::PageTurnEffect();
  delete effect;

  DALI_TEST_CHECK( true );
}
