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

  view = EffectsView::New( EffectsView::DROP_SHADOW );
  DALI_TEST_CHECK( view );

  Stage::GetCurrent().Add( view );

  view.Reset();
  view = EffectsView::New( EffectsView::EMBOSS );
  DALI_TEST_CHECK( view );

  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliEffectsViewCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New( EffectsView::DROP_SHADOW );
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

  BaseHandle view = EffectsView::New( EffectsView::EMBOSS );
  DALI_TEST_CHECK( EffectsView::DownCast( view ) );

  BaseHandle empty;
  DALI_TEST_CHECK( ! EffectsView::DownCast( empty ) );

  BaseHandle another = Actor::New();
  DALI_TEST_CHECK( ! EffectsView::DownCast( another ) );

  END_TEST;
}

// Positive test case for a method
int UtcDaliEffectsViewAddRemove(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliGaussianBlurViewAddRemove");

  EffectsView view = EffectsView::New( EffectsView::DROP_SHADOW );
  DALI_TEST_CHECK( view );

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );


  view.SetParentOrigin(ParentOrigin::CENTER);
  view.SetSize(Stage::GetCurrent().GetSize());
  view.Add(actor);
  Stage::GetCurrent().Add(view);

  DALI_TEST_CHECK( actor.OnStage() );
  DALI_TEST_CHECK( actor.GetParent() );
  DALI_TEST_CHECK( actor.GetParent() != view );

  view.Remove(actor);

  DALI_TEST_CHECK( !actor.OnStage() );
  END_TEST;
}


int UtcDaliEffectsViewAddRemoveEmboss(void)
{
  ToolkitTestApplication application;
  tet_infoline("UtcDaliEffectsViewAddRemoveEmboss");

  tet_infoline("Checking number of render tasks = 1");
  application.SendNotification();
  application.Render();
  Stage stage = Stage::GetCurrent();
  DALI_TEST_EQUALS( stage.GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION );

  tet_infoline("Create effects view");

  EffectsView view = EffectsView::New( EffectsView::EMBOSS );
  Vector3 offsetSet( 2.f, 3.f, 4.f );
  Vector4 colorSet( 0.2f, 0.3f, 0.4f, 0.5f );
  view.SetProperty( EffectsView::Property::EFFECT_OFFSET, offsetSet);
  view.SetProperty( EffectsView::Property::EFFECT_COLOR, colorSet);
  Vector3 offsetAnimate( 4.f, 6.f, 8.f );
  float durationSeconds(0.05f);
  Animation animation = Animation::New( durationSeconds );
  animation.AnimateTo( Property(view,EffectsView::Property::EFFECT_OFFSET ), offsetAnimate );
  animation.Play();

  DALI_TEST_CHECK( view );

  Actor actor = Actor::New();
  actor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
  DALI_TEST_CHECK( !actor.OnStage() );

  view.SetParentOrigin(ParentOrigin::CENTER);

  view.Add(actor);
  view.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

  stage.Add(view);

  DALI_TEST_CHECK( actor.OnStage() );

  application.SendNotification();
  application.Render();

  tet_infoline("Removing view from stage disables view");
  stage.Remove(view);

  tet_infoline("Checking number of render tasks = 1");
  DALI_TEST_EQUALS( stage.GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION );

  tet_infoline("Adding view to stage again re-enables view");
  stage.Add(view);

  tet_infoline("Removing view from stage disables view");
  DALI_TEST_GREATER( stage.GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION );
  stage.Remove(view);
  view.Reset();

  tet_infoline("Checking number of render tasks = 1");
  DALI_TEST_EQUALS( stage.GetRenderTaskList().GetTaskCount(), 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliEffectsViewGetTypeP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New( EffectsView::DROP_SHADOW );
  DALI_TEST_CHECK( view.GetType() == EffectsView::DROP_SHADOW );

  view.Reset();
  view = EffectsView::New( EffectsView::EMBOSS );
  DALI_TEST_CHECK( view.GetType() == EffectsView::EMBOSS );

  END_TEST;
}

int UtcDaliEffectsViewOnStage(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New(EffectsView::EMBOSS);
  view.SetSize(100.f, 100.f);
  Stage stage = Stage::GetCurrent();
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTaskCount() == 1 );

  stage.Add( view );
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTaskCount() > 1 );

  END_TEST;
}

int UtcDaliEffectsViewOffStage(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New(EffectsView::DROP_SHADOW);
  view.SetSize(100.f, 100.f);
  Stage stage = Stage::GetCurrent();
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTaskCount() == 1 );

  stage.Add( view );
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTaskCount() > 1 );

  stage.Remove( view );
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK( stage.GetRenderTaskList().GetTaskCount() == 1 );

  END_TEST;
}

int UtcDaliEffectsViewRefreshP(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New( EffectsView::DROP_SHADOW );
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

  EffectsView view = EffectsView::New( EffectsView::DROP_SHADOW );
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

int UtcDaliEffectsViewSizeProperty(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New( EffectsView::DROP_SHADOW );

  Property::Index idx = view.GetPropertyIndex( "effectSize" );
  DALI_TEST_EQUALS( idx, (Property::Index)EffectsView::Property::EFFECT_SIZE, TEST_LOCATION );

  view.SetProperty( idx, 5 );
  Property::Value value = view.GetProperty( EffectsView::Property::EFFECT_SIZE );
  int size;
  DALI_TEST_CHECK( value.Get(size) );
  DALI_TEST_CHECK( size == 5 );

  END_TEST;
}

int UtcDaliEffectsViewOffsetProperty(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New( EffectsView::EMBOSS );
  Stage::GetCurrent().Add( view );

  Property::Value value = view.GetProperty( EffectsView::Property::EFFECT_OFFSET );
  Vector3 offsetValue;
  DALI_TEST_CHECK( value.Get(offsetValue) );
  DALI_TEST_EQUALS( offsetValue, Vector3::ZERO, TEST_LOCATION );

  Vector3 offsetSet( 2.f, 3.f, 4.f );
  view.SetProperty( EffectsView::Property::EFFECT_OFFSET, offsetSet);
  application.SendNotification();
  application.Render(0);
  value = view.GetProperty( EffectsView::Property::EFFECT_OFFSET );
  value.Get(offsetValue);
  DALI_TEST_EQUALS( offsetValue, offsetSet, TEST_LOCATION );

  Vector3 offsetAnimate( 4.f, 6.f, 8.f );
  float durationSeconds(0.05f);
  Animation animation = Animation::New( durationSeconds );
  animation.AnimateTo( Property(view,EffectsView::Property::EFFECT_OFFSET ), offsetAnimate );
  animation.Play();
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) + 1u/*just beyond the animation duration*/);

  value = view.GetProperty( EffectsView::Property::EFFECT_OFFSET );
  value.Get(offsetValue);
  DALI_TEST_EQUALS( offsetValue, offsetAnimate, TEST_LOCATION );

  END_TEST;
}

int UtcDaliEffectsViewColorProperty(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New( EffectsView::DROP_SHADOW );
  Stage::GetCurrent().Add( view );

  Property::Value value = view.GetProperty( EffectsView::Property::EFFECT_COLOR );
  Vector4 colorValue;
  DALI_TEST_CHECK( value.Get(colorValue) );
  DALI_TEST_EQUALS( colorValue, Color::WHITE, TEST_LOCATION );

  Vector4 colorSet( 0.2f, 0.3f, 0.4f, 0.5f );
  view.SetProperty( EffectsView::Property::EFFECT_COLOR, colorSet);
  application.SendNotification();
  application.Render(0);
  value = view.GetProperty( EffectsView::Property::EFFECT_COLOR );
  value.Get(colorValue);
  DALI_TEST_EQUALS( colorValue, colorSet, TEST_LOCATION );

  Vector4 colorAnimate( 0.5f, 0.6f, 0.8f, 1.f );
  float durationSeconds(0.05f);
  Animation animation = Animation::New( durationSeconds );
  animation.AnimateTo( Property(view,EffectsView::Property::EFFECT_COLOR ), colorAnimate );
  animation.Play();
  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*1000.0f) + 1u/*just beyond the animation duration*/);

  value = view.GetProperty( EffectsView::Property::EFFECT_COLOR );
  value.Get(colorValue);
  DALI_TEST_EQUALS( colorValue, colorAnimate, TEST_LOCATION );

  END_TEST;
}

int UtcDaliEffectsViewGetSetBackgroundColor(void)
{
  ToolkitTestApplication application;

  EffectsView view = EffectsView::New(EffectsView::DROP_SHADOW);
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

  EffectsView view = EffectsView::New(EffectsView::DROP_SHADOW);
  view.SetSize(100.f, 100.f);

  Stage stage = Stage::GetCurrent();
  stage.Add( view );
  application.SendNotification();
  application.Render();

  RenderTaskList renderTaskList = stage.GetRenderTaskList();
  DALI_TEST_CHECK( renderTaskList.GetTask( 1 ).GetRefreshRate() == RenderTask::REFRESH_ALWAYS );

  view.SetRefreshOnDemand( true );
  DALI_TEST_CHECK( renderTaskList.GetTask( 1 ).GetRefreshRate() == RenderTask::REFRESH_ONCE );

  view.SetRefreshOnDemand( false );
  DALI_TEST_CHECK( renderTaskList.GetTask( 1 ).GetRefreshRate() == RenderTask::REFRESH_ALWAYS );

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
    EffectsView view = EffectsView::New(EffectsView::DROP_SHADOW);
    view.SetSize( 200.0f, 200.0f, 0.0f );
    stage.Add( view );
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS( view.GetCurrentSize(), Vector3( 200.0f, 200.0f, 0.0f ), TEST_LOCATION );
  }

  {
    EffectsView view = EffectsView::New(EffectsView::EMBOSS);
    view.SetSize( 200.0f, 200.0f, 0.0f );
    stage.Add( view );
    application.SendNotification();
    application.Render();

    DALI_TEST_EQUALS( view.GetCurrentSize(), Vector3( 200.0f, 200.0f, 0.0f ), TEST_LOCATION );
  }

  {
    EffectsView view = EffectsView::New(EffectsView::DROP_SHADOW);
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
