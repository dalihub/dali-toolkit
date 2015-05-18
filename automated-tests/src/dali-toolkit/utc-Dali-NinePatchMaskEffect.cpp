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

// Need to override adaptor classes for toolkit test harness, so include
// test harness headers before dali headers.
#include <dali-toolkit-test-suite-utils.h>

#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/shader-effects/nine-patch-mask-effect.h>

using namespace Dali;

void utc_dali_toolkit_nine_patch_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_nine_patch_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliNinePatchMaskEffectApply(void)
{
  ToolkitTestApplication application;

  BufferImage image = CreateBufferImage();
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
  END_TEST;
}
