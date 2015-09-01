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
#include <sstream>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/effects-view/effects-view.h>

using namespace Dali;
using namespace Toolkit;

void dali_effectsview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_effectsview_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliEffectsViewNew(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  DALI_TEST_CHECK( !view );

  view = EffectsView::New();
  DALI_TEST_CHECK( view );

  Stage::GetCurrent().Add( view );

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliEffectsViewCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  DALI_TEST_CHECK( view );

  EffectsView copy( view );
  DALI_TEST_CHECK( copy == view );

  EffectsView assign;
  DALI_TEST_CHECK( !assign );
  assign = view;
  DALI_TEST_CHECK( assign == view );

  // Self assignment
  assign = assign;
  DALI_TEST_CHECK( assign );
  DALI_TEST_CHECK( assign == view );

  END_TEST;
}

int UtcDaliEffectsViewDownCast(void)
{
  ToolkitTestApplication application;

  BaseHandle view = EffectsView::New();
  DALI_TEST_CHECK( EffectsView::DownCast( view ) );

  BaseHandle empty;
  DALI_TEST_CHECK( ! EffectsView::DownCast( empty ) );

  BaseHandle another = Actor::New();
  DALI_TEST_CHECK( ! EffectsView::DownCast( another ) );

  END_TEST;
}

int UtcDaliEffectsViewSetGetTypeP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  DALI_TEST_CHECK( view.GetType() == EffectsView::INVALID_TYPE );

  view.SetType( EffectsView::DROP_SHADOW );
  DALI_TEST_CHECK( view.GetType() == EffectsView::DROP_SHADOW );

  view.SetType( EffectsView::EMBOSS );
  DALI_TEST_CHECK( view.GetType() == EffectsView::EMBOSS );

  END_TEST;
}

int UtcDaliEffectsViewSetTypeN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    view.SetType( EffectsView::DROP_SHADOW );
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewGetTypeN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    EffectsView::EffectType type = view.GetType();
    (void) type;
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewEnableP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  Stage stage = Stage::GetCurrent();
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTaskCount() == 1 );

  view.Enable();
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTaskCount() > 1 );

  END_TEST;
}

int UtcDaliEffectsViewEnableN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    view.Enable();
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewDisableP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  Stage stage = Stage::GetCurrent();
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTaskCount() == 1 );

  view.Enable();
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTaskCount() > 1 );

  view.Disable();
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTaskCount() == 1 );

  END_TEST;
}

int UtcDaliEffectsViewDisableN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    view.Disable();
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewRefreshP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  try
  {
    view.Refresh();
    DALI_TEST_CHECK( true );
  }
  catch( ... )
  {
    DALI_TEST_CHECK( false ); // Should not get here!
  }

  END_TEST;
}

int UtcDaliEffectsViewRefreshN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    view.Refresh();
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewSetPixelFormatP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  try
  {
    view.SetPixelFormat( Pixel::RGBA8888 );
    DALI_TEST_CHECK( true );
  }
  catch( ... )
  {
    DALI_TEST_CHECK( false ); // Should not get here!
  }

  END_TEST;
}

int UtcDaliEffectsViewSetPixelFormatN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    view.SetPixelFormat( Pixel::RGBA8888 );
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewSetGetOutputImage(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  FrameBufferImage image = FrameBufferImage::New();
  DALI_TEST_CHECK( image );

  view.SetOutputImage( image );
  DALI_TEST_CHECK( view.GetOutputImage() == image );

  // Replace with another image
  FrameBufferImage image2 = FrameBufferImage::New();
  DALI_TEST_CHECK( image2 );
  view.SetOutputImage( image2 );
  DALI_TEST_CHECK( view.GetOutputImage() == image2 );

  // Remove output image
  view.SetOutputImage( FrameBufferImage() );
  DALI_TEST_CHECK( ! view.GetOutputImage() );

  END_TEST;
}

int UtcDaliEffectsViewSetOutputImageN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    view.SetOutputImage( FrameBufferImage::New() );
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewGetOutputImageN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    FrameBufferImage image = view.GetOutputImage();
    (void)image;
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewGetEffectSizePropertyIndexP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  DALI_TEST_CHECK( Property::INVALID_INDEX != view.GetEffectSizePropertyIndex() );

  END_TEST;
}

int UtcDaliEffectsViewGetEffectSizePropertyIndexN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    Property::Index index = view.GetEffectSizePropertyIndex();
    (void)index;
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewGetEffectStrengthPropertyIndexP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  DALI_TEST_CHECK( Property::INVALID_INDEX != view.GetEffectStrengthPropertyIndex() );

  END_TEST;
}

int UtcDaliEffectsViewGetEffectStrengthPropertyIndexN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    Property::Index index = view.GetEffectStrengthPropertyIndex();
    (void)index;
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewGetEffectOffsetPropertyIndexP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  DALI_TEST_CHECK( Property::INVALID_INDEX != view.GetEffectOffsetPropertyIndex() );

  END_TEST;
}

int UtcDaliEffectsViewGetEffectOffsetPropertyIndexN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    Property::Index index = view.GetEffectOffsetPropertyIndex();
    (void)index;
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewGetEffectColorPropertyIndexP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  DALI_TEST_CHECK( Property::INVALID_INDEX != view.GetEffectColorPropertyIndex() );

  END_TEST;
}

int UtcDaliEffectsViewGetEffectColorPropertyIndexN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    Property::Index index = view.GetEffectColorPropertyIndex();
    (void)index;
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewGetSetBackgroundColor(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  view.SetBackgroundColor( Color::RED );
  DALI_TEST_CHECK( Color::RED == view.GetBackgroundColor() );

  view.SetBackgroundColor( Color::YELLOW );
  DALI_TEST_CHECK( Color::YELLOW == view.GetBackgroundColor() );

  END_TEST;
}

int UtcDaliEffectsViewSetBackgroundColorN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    view.SetBackgroundColor( Color::RED );
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewGetBackgroundColorN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    Vector4 color = view.GetBackgroundColor();
    (void)color;
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewSetRefreshOnDemandP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New();
  FrameBufferImage image = FrameBufferImage::New();
  view.SetOutputImage( image );
  view.Enable();

  Stage stage = Stage::GetCurrent();
  stage.Add( view );

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  DALI_TEST_CHECK( renderTaskList.GetTask( 1 ).GetRefreshRate() == RenderTask::REFRESH_ALWAYS );
  DALI_TEST_CHECK( renderTaskList.GetTask( 2 ).GetRefreshRate() == RenderTask::REFRESH_ALWAYS );

  view.SetRefreshOnDemand( true );
  DALI_TEST_CHECK( renderTaskList.GetTask( 1 ).GetRefreshRate() == RenderTask::REFRESH_ONCE );
  DALI_TEST_CHECK( renderTaskList.GetTask( 2 ).GetRefreshRate() == RenderTask::REFRESH_ONCE );

  view.SetRefreshOnDemand( false );
  DALI_TEST_CHECK( renderTaskList.GetTask( 1 ).GetRefreshRate() == RenderTask::REFRESH_ALWAYS );
  DALI_TEST_CHECK( renderTaskList.GetTask( 2 ).GetRefreshRate() == RenderTask::REFRESH_ALWAYS );

  END_TEST;
}

int UtcDaliEffectsViewSetRefreshOnDemandN(void)
{
  ToolkitTestApplication application;

  EffectsView view;
  try
  {
    view.SetRefreshOnDemand( false );
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch( ... )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliEffectsViewSizeSet(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  {
    EffectsView view = EffectsView::New();
    view.SetSize( 200.0f, 200.0f, 0.0f );
    stage.Add( view );
    view.Enable();
    application.SendNotification();
    application.Render();
    view.Disable();
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( view.GetCurrentSize(), Vector3( 200.0f, 200.0f, 0.0f ), TEST_LOCATION );
  }

  {
    EffectsView view = EffectsView::New();
    view.SetOutputImage( FrameBufferImage::New( 200, 200 ) );
    view.SetType( EffectsView::EMBOSS );
    view.SetSize( 200.0f, 200.0f, 0.0f );
    stage.Add( view );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( view.GetCurrentSize(), Vector3( 200.0f, 200.0f, 0.0f ), TEST_LOCATION );
  }

  {
    EffectsView view = EffectsView::New();
    view.SetType( EffectsView::DROP_SHADOW );
    view.SetSize( 200.0f, 200.0f, 0.0f );
    stage.Add( view );
    application.SendNotification();
    application.Render();

    stage.Remove( view );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( view.GetCurrentSize(), Vector3( 200.0f, 200.0f, 0.0f ), TEST_LOCATION );
  }

  END_TEST;
}

int UtcDaliEffectsViewTypeRegistry(void)
{
  ToolkitTestApplication application;

  TypeRegistry typeRegistry = TypeRegistry::Get();
  DALI_TEST_CHECK( typeRegistry );

  TypeInfo typeInfo = typeRegistry.GetTypeInfo( "EffectsView" );
  DALI_TEST_CHECK( typeInfo );

  BaseHandle handle = typeInfo.CreateInstance();
  DALI_TEST_CHECK( handle );

  EffectsView view = EffectsView::DownCast( handle );
  DALI_TEST_CHECK( view );

  END_TEST;
}
