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
TEST_FUNCTION( UtcDaliNinePatchMaskEffectApply, POSITIVE_TC_IDX );


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

static void UtcDaliNinePatchMaskEffectApply()
{
  ToolkitTestApplication application;

  BitmapImage image = CreateBitmapImage();
  ImageActor actor0 = ImageActor::New( image );
  Toolkit::NinePatchMaskEffect::Apply( actor0, "" );

  Stage::GetCurrent().Add( actor0 );

  application.SendNotification();   // Force usage of constraint
  application.Render();

  DALI_TEST_CHECK( actor0.GetStyle() == ImageActor::STYLE_NINE_PATCH );

  ImageActor actor1 = ImageActor::New( image );
  Vector4 border( 0, 0, 0, 0 );
  Toolkit::NinePatchMaskEffect::Apply( actor1, "", border );

  Stage::GetCurrent().Add( actor1 );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( actor1.GetStyle() == ImageActor::STYLE_NINE_PATCH );
}

