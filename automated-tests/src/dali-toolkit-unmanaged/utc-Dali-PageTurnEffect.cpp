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
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>


using namespace Dali;

void page_turn_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void page_turn_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliPageTurnEffectApply(void)
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
  END_TEST;
}

int UtcDaliPageTurnEffectConstruct(void)
{
  ToolkitTestApplication application;

  Toolkit::PageTurnEffect* effect = new Toolkit::PageTurnEffect();
  delete effect;

  DALI_TEST_CHECK( true );
  END_TEST;
}
