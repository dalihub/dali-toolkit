/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <unistd.h>
#include <dali/dali.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

// Resolution: 50*50, pixel format: RGBA8888
static const char* gImage_50_RGBA = TEST_RESOURCE_DIR "/icon-delete.png";

// Resolution: 128*128, pixel format: RGB888
static const char* gImage_128_RGB = TEST_RESOURCE_DIR "/gallery-small-1.jpg";


void VerifyLoad( PixelData pixelData, uint32_t width, uint32_t height )
{
  DALI_TEST_CHECK( pixelData );
  DALI_TEST_EQUALS<unsigned int>( pixelData.GetWidth(), width, TEST_LOCATION );
  DALI_TEST_EQUALS<unsigned int>( pixelData.GetHeight(), height, TEST_LOCATION );
}

} // anonymous namespace


int UtcDaliSyncImageLoaderLoad(void)
{
  PixelData pixelData = Toolkit::SyncImageLoader::Load( gImage_50_RGBA );

  DALI_TEST_EQUALS<bool>( pixelData, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSyncImageLoaderLoadWithDimensions(void)
{
  PixelData pixelData = Toolkit::SyncImageLoader::Load( gImage_50_RGBA, ImageDimensions( 25, 25 ) );

  VerifyLoad( pixelData, 25u, 25u );

  END_TEST;
}

int UtcDaliSyncImageLoaderLoadWithAllOptions(void)
{
  PixelData pixelData = Toolkit::SyncImageLoader::Load( gImage_128_RGB, ImageDimensions( 100, 100 ), FittingMode::SCALE_TO_FILL, SamplingMode::BOX_THEN_LINEAR, true );

  VerifyLoad( pixelData, 100u, 100u );

  END_TEST;
}


