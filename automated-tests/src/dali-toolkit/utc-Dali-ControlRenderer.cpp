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

void dali_control_renderer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_control_renderer_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliControlRendererCopyAndAssignment(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererCopyAndAssignment" );

  RendererFactory factory = RendererFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "color-renderer");
  propertyMap.Insert("blend-color", Color::BLUE);
  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );

  ControlRenderer controlRendererCopy( controlRenderer );
  DALI_TEST_CHECK(controlRenderer == controlRendererCopy);

  ControlRenderer emptyControlRenderer;
  ControlRenderer emptyControlRendererCopy( emptyControlRenderer );
  DALI_TEST_CHECK(emptyControlRenderer == emptyControlRendererCopy);

  ControlRenderer controlRendererEquals;
  controlRendererEquals = controlRenderer;
  DALI_TEST_CHECK(controlRenderer == controlRendererEquals);

  ControlRenderer emptyControlRendererEquals;
  emptyControlRendererEquals = emptyControlRenderer;
  DALI_TEST_CHECK( emptyControlRenderer == emptyControlRendererEquals );

  //self assignment
  controlRenderer = controlRenderer;
  DALI_TEST_CHECK( controlRenderer = controlRendererCopy );

  END_TEST;
}

int UtcDaliControlRendererSetDepthIndex(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererSetDepthIndex" );

  RendererFactory factory = RendererFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "color-renderer");
  propertyMap.Insert("blend-color", Color::BLUE);
  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );

  controlRenderer.SetDepthIndex( 1.f );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );
  controlRenderer.SetOnStage( actor );

  DALI_TEST_EQUALS( actor.GetRendererAt(0u).GetDepthIndex(), 1.f, TEST_LOCATION );

  controlRenderer.SetDepthIndex( -1.f );
  DALI_TEST_EQUALS( actor.GetRendererAt(0u).GetDepthIndex(), -1.f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliControlRendererSetOnStage(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliControlRendererSetDepthIndex" );

  RendererFactory factory = RendererFactory::Get();
  Property::Map propertyMap;
  propertyMap.Insert("renderer-type", "color-renderer");
  propertyMap.Insert("blend-color", Color::BLUE);
  ControlRenderer controlRenderer = factory.GetControlRenderer( propertyMap );

  Actor actor = Actor::New();
  actor.SetSize(200.f, 200.f);
  Stage::GetCurrent().Add( actor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 0u );

  controlRenderer.SetOnStage( actor );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_CHECK( actor.GetRendererCount() == 1u );

  END_TEST;
}
