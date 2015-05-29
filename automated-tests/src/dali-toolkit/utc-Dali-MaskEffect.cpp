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
#include <dali-toolkit/devel-api/shader-effects/mask-effect.h>

using namespace Dali;

void utc_dali_toolkit_mask_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_mask_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliMaskEffectCreateEffect(void)
{
  ToolkitTestApplication application;

  BufferImage image = CreateBufferImage();

  ShaderEffect effect = Toolkit::MaskEffect::New( image );
  DALI_TEST_CHECK( effect );
  END_TEST;
}

int UtcDaliMaskEffectDestructor(void)
{
  ToolkitTestApplication application;

  Toolkit::MaskEffect* effect = new Toolkit::MaskEffect();
  delete effect;

  DALI_TEST_CHECK( true );
  END_TEST;
}
