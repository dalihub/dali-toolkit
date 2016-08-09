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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/pan-gesture-event.h>

using namespace Dali;
using namespace Toolkit;

void utc_dali_toolkit_scroll_view_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_scroll_view_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const int RENDER_FRAME_INTERVAL = 16;                           ///< Duration of each frame in ms. (at approx 60FPS)

/*
 * Simulate time passed by.
 *
 * @note this will always process at least 1 frame (1/60 sec)
 *
 * @param application Test application instance
 * @param duration Time to pass in milliseconds.
 * @return The actual time passed in milliseconds
 */
int Wait(ToolkitTestApplication& application, int duration = 0)
{
  int time = 0;

  for(int i = 0; i <= ( duration / RENDER_FRAME_INTERVAL); i++)
  {
    application.SendNotification();
    application.Render(RENDER_FRAME_INTERVAL);
    time += RENDER_FRAME_INTERVAL;
  }

  return time;
}

/**
 * Creates a Ruler that snaps to a specified grid size.
 * If that grid size is 0.0 then this ruler does not
 * snap.
 *
 * @param[in] gridSize (optional) The grid size for the ruler,
 * (Default = 0.0 i.e. no snapping)
 * @return The ruler is returned.
 */
RulerPtr CreateRuler(float gridSize = 0.0f)
{
  if(gridSize <= Math::MACHINE_EPSILON_0)
  {
      return new DefaultRuler();
  }
  return new FixedRuler(gridSize);
}

// Callback probes.

static bool gOnScrollStartCalled;                       ///< Whether the OnScrollStart signal was invoked.
static bool gOnScrollUpdateCalled;                      ///< Whether the OnScrollUpdate signal was invoked.
static bool gOnScrollCompleteCalled;                    ///< Whether the OnScrollComplete signal was invoked.
static Vector3 gConstraintResult;                       ///< Result from constraint.

static std::vector< Actor > gPages;                                ///< Keeps track of all the pages for applying effects.
typedef std::vector< Actor >::iterator ActorIter;

static void ResetScrollCallbackResults()
{
  gOnScrollStartCalled = false;
  gOnScrollUpdateCalled = false;
  gOnScrollCompleteCalled = false;
}

/**
 * Invoked when scrolling starts.
 *
 * @param[in] position The current scroll position.
 */
static void OnScrollStart( const Vector2& position )
{
  gOnScrollStartCalled = true;
}

/**
 * Invoked when scrolling updates (via dragging)
 *
 * @param[in] position The current scroll position.
 */
static void OnScrollUpdate( const Vector2& position )
{
  gOnScrollUpdateCalled = true;
}

/**
 * Invoked when scrolling finishes
 *
 * @param[in] position The current scroll position.
 */
static void OnScrollComplete( const Vector2& position )
{
  gOnScrollCompleteCalled = true;
}

ScrollView SetupTestScrollView(int rows, int columns, Vector2 size)
{
  Constraint constraint;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetSize(size);
  scrollView.SetAnchorPoint(AnchorPoint::CENTER);
  scrollView.SetParentOrigin(ParentOrigin::CENTER);

  constraint = Constraint::New<Dali::Vector3>( scrollView, Dali::Actor::Property::SIZE, Dali::EqualToConstraint() );
  constraint.AddSource( Dali::ParentSource( Dali::Actor::Property::SIZE ) );
  constraint.Apply();

  scrollView.SetWrapMode(false);
  scrollView.ScrollStartedSignal().Connect( &OnScrollStart );
  scrollView.ScrollUpdatedSignal().Connect( &OnScrollUpdate );
  scrollView.ScrollCompletedSignal().Connect( &OnScrollComplete );
  Stage::GetCurrent().Add( scrollView );
  RulerPtr rulerX = CreateRuler(size.width);
  RulerPtr rulerY = CreateRuler(size.height);
  if(columns > 1)
  {
    rulerX->SetDomain(RulerDomain(0.0f, size.width * columns));
  }
  else
  {
    rulerX->Disable();
  }
  if(rows > 1)
  {
    rulerY->SetDomain(RulerDomain(0.0f, size.width * rows));
  }
  else
  {
    rulerY->Disable();
  }

  scrollView.SetRulerX( rulerX );
  scrollView.SetRulerY( rulerY );
  Stage::GetCurrent().Add( scrollView );

  Actor container = Actor::New();
  container.SetParentOrigin(ParentOrigin::CENTER);
  container.SetAnchorPoint(AnchorPoint::CENTER);
  container.SetSize( size );
  scrollView.Add( container );

  constraint = Constraint::New<Vector3>( container, Actor::Property::SIZE, EqualToConstraint() );
  constraint.AddSource( Dali::ParentSource( Dali::Actor::Property::SIZE ) );
  constraint.Apply();

  gPages.clear();
  for(int row = 0;row<rows;row++)
  {
    for(int column = 0;column<columns;column++)
    {
      Actor page = Actor::New();

      constraint = Constraint::New<Vector3>( page, Actor::Property::SIZE, EqualToConstraint() );
      constraint.AddSource( Dali::ParentSource( Dali::Actor::Property::SIZE ) );
      constraint.Apply();
      page.SetParentOrigin( ParentOrigin::CENTER );
      page.SetAnchorPoint( AnchorPoint::CENTER );
      page.SetPosition( column * size.x, row * size.y );
      container.Add(page);

      gPages.push_back(page);
    }
  }

  ResetScrollCallbackResults();
  return scrollView;
}

void CleanupTest()
{
  gPages.clear();
  ResetScrollCallbackResults();
}

Actor AddActorToPage(Actor page, float x, float y, float cols, float rows)
{
  Stage stage = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  const float margin = 10.0f;
  const Vector2 actorSize((stageSize.x / cols) - margin, (stageSize.y / rows) - margin);

  Actor actor = Actor::New();
  actor.SetParentOrigin( ParentOrigin::CENTER );
  actor.SetAnchorPoint( AnchorPoint::CENTER );

  Vector3 position( margin * 0.5f + (actorSize.x + margin) * x - stageSize.width * 0.5f,
                    margin * 0.5f + (actorSize.y + margin) * y - stageSize.height * 0.5f,
                    0.0f);
  Vector3 positionEnd( margin * 0.5f + (actorSize.x + margin) * (x + cols) - stageSize.width * 0.5f - margin,
                       margin * 0.5f + (actorSize.y + margin) * (y + rows) - stageSize.height * 0.5f - margin,
                       0.0f);
  Vector3 size(positionEnd - position);
  actor.SetPosition( position + size * 0.5f);
  actor.SetSize( positionEnd - position );
  page.Add(actor);
  return actor;
}

} // unnamed namespace

int UtcDaliScrollViewPagePathEffectSetup(void)
{
  tet_infoline(" UtcDaliScrollViewPagePathEffectSetup");

  ScrollViewPagePathEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewPagePathEffect::New(Dali::Path::New(), Vector3::ZERO,Toolkit::ScrollView::Property::SCROLL_FINAL_X, Vector3::ZERO,0);

  DALI_TEST_CHECK( handle );

  effect = ScrollViewPagePathEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
  END_TEST;
}

int UtcDaliScrollViewPagePathEffectTest(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScrollViewPagePathEffectTest");

  Vector2 size = Stage::GetCurrent().GetSize();

  ScrollView scrollView = SetupTestScrollView(1, 3, size);
  Actor testPage = gPages[2];
  Wait(application, 500);

  //Create path
  float xHalfSize( size.x * 0.5f);

  Dali::Path path = Dali::Path::New();
  Dali::Property::Array points;
  points.Resize(3);
  points[0] = Vector3( xHalfSize, 0.0f,  -xHalfSize);
  points[1] = Vector3( 0.0f, 0.0f, 0.0f );
  points[2] = Vector3( -xHalfSize, 0.0f,  -xHalfSize);
  path.SetProperty( Path::Property::POINTS, points );

  Dali::Property::Array controlPoints;
  controlPoints.Resize(4);
  controlPoints[0] = Vector3( xHalfSize, 0.0f, 0.0f );
  controlPoints[1] = Vector3( xHalfSize, 0.0f, 0.0f );
  controlPoints[2] = Vector3(-xHalfSize, 0.0f, 0.0f );
  controlPoints[3] = Vector3(-xHalfSize, 0.0f, 0.0f );
  path.SetProperty( Path::Property::CONTROL_POINTS, controlPoints );

  ScrollViewPagePathEffect effect = ScrollViewPagePathEffect::New(path,
                                                                  Vector3::ZERO,
                                                                  Toolkit::ScrollView::Property::SCROLL_FINAL_X,
                                                                  Vector3(size.x,size.y,0.0f),
                                                                  3);
  scrollView.ApplyEffect(effect);

  unsigned int pageCounter(0);
  for(ActorIter pageIter = gPages.begin(); pageIter != gPages.end(); ++pageIter)
  {
    Actor page = *pageIter;
    page.RemoveConstraints();
    Constraint constraint = Constraint::New<Vector3>( page, Actor::Property::SIZE, EqualToConstraint() );
    constraint.AddSource( ParentSource( Actor::Property::SIZE ) );
    constraint.Apply();
    effect.ApplyToPage(page, pageCounter++);
  }
  Wait(application);

  scrollView.ScrollTo(1);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }

  // test that the test page has reached centre of screen
  Vector3 pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, Vector3::ZERO, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  CleanupTest();
  END_TEST;
}
