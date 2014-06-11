//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/pan-gesture-event.h>


using namespace Dali;
using namespace Toolkit;

void dali_scroll_view_effect_startup(void)
{
  test_return_value = TET_UNDEF;
}

void dali_scroll_view_effect_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

const int MILLISECONDS_PER_SECOND = 1000;
const int RENDER_FRAME_INTERVAL = 16;                           ///< Duration of each frame in ms. (at approx 60FPS)
const int RENDER_ANIMATION_TEST_DURATION_MS = 1000;             ///< 1000ms to test animation
const int RENDER_DELAY_SCROLL = 1000;                           ///< duration to wait for any scroll to complete.

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

static ActorContainer gPages;                                ///< Keeps track of all the pages for applying effects.

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
static void OnScrollStart( const Vector3& position )
{
  gOnScrollStartCalled = true;
}

/**
 * Invoked when scrolling updates (via dragging)
 *
 * @param[in] position The current scroll position.
 */
static void OnScrollUpdate( const Vector3& position )
{
  gOnScrollUpdateCalled = true;
}

/**
 * Invoked when scrolling finishes
 *
 * @param[in] position The current scroll position.
 */
static void OnScrollComplete( const Vector3& position )
{
  gOnScrollCompleteCalled = true;
}


ScrollView SetupTestScrollView(int rows, int columns, Vector2 size)
{
  ScrollView scrollView = ScrollView::New();
  scrollView.SetSize(size);
  scrollView.SetAnchorPoint(AnchorPoint::CENTER);
  scrollView.SetParentOrigin(ParentOrigin::CENTER);
  scrollView.ApplyConstraint( Constraint::New<Dali::Vector3>( Dali::Actor::SIZE, Dali::ParentSource( Dali::Actor::SIZE ), Dali::EqualToConstraint() ) );
  // Disable Refresh signal (TET environment cannot use adaptor's Timer)
  scrollView.SetWrapMode(false);
  scrollView.SetRefreshInterval(0);
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
  container.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );

  gPages.clear();
  for(int row = 0;row<rows;row++)
  {
    for(int column = 0;column<columns;column++)
    {
      Actor page = Actor::New();
      page.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
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


int UtcDaliScrollViewPageCubeEffectSetup(void)
{
  tet_infoline(" UtcDaliScrollViewPageCubeEffectSetup");

  ScrollViewPageCubeEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewPageCubeEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewPageCubeEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
  END_TEST;
}


int UtcDaliScrollViewPageCarouselEffectSetup(void)
{
  tet_infoline(" UtcDaliScrollViewCarouselEffectSetup");

  ScrollViewPageCarouselEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewPageCarouselEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewPageCarouselEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
  END_TEST;
}

int UtcDaliScrollViewCarouselEffectSetup(void)
{
  tet_infoline(" UtcDaliScrollViewCarouselEffectSetup");

  ScrollViewCarouselEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewCarouselEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewCarouselEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
  END_TEST;
}

int UtcDaliScrollViewDepthEffectSetup(void)
{
  tet_infoline(" UtcDaliScrollViewDepthEffectSetup");

  ScrollViewDepthEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewDepthEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewDepthEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
  END_TEST;
}


int UtcDaliScrollViewPageCubeEffectTest(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScrollViewPageCubeEffectTest");

  Vector2 size = Stage::GetCurrent().GetSize();

  ScrollView scrollView = SetupTestScrollView(1, 3, size);
  Actor testPage = gPages[1];
  Wait(application, 500);

  ScrollViewPageCubeEffect effect = ScrollViewPageCubeEffect::New();
  scrollView.ApplyEffect(effect);

  for(ActorIter pageIter = gPages.begin(); pageIter != gPages.end(); ++pageIter)
  {
    Actor page = *pageIter;
    page.RemoveConstraints();
    page.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    effect.ApplyToPage(page, Vector2(Math::PI_2, 0.0f));
  }
  Wait(application);

  scrollView.ScrollTo(1);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }
  // test that the first page has reached centre of screen
  Vector3 pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, Vector3::ZERO, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  CleanupTest();
  END_TEST;
}

int UtcDaliScrollViewPageCarouselEffectTest(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScrollViewPageCarouselEffectTest");

  Vector2 size = Stage::GetCurrent().GetSize();

  ScrollView scrollView = SetupTestScrollView(1, 3, size);
  Actor testPage = gPages[1];
  Wait(application, 500);

  ScrollViewPageCarouselEffect effect = ScrollViewPageCarouselEffect::New();
  scrollView.ApplyEffect(effect);

  for(ActorIter pageIter = gPages.begin(); pageIter != gPages.end(); ++pageIter)
  {
    Actor page = *pageIter;
    page.RemoveConstraints();
    page.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    effect.ApplyToPage(page);
  }
  Wait(application);

  scrollView.ScrollTo(1, 0.5f, DirectionBiasNone);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }
  // test that the first page has reached centre of screen
  Vector3 pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, Vector3::ZERO, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  CleanupTest();
  END_TEST;
}

int UtcDaliScrollViewCarouselEffectTest(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScrollViewCarouselEffectTest");

  Vector2 size = Stage::GetCurrent().GetSize();

  ScrollView scrollView = SetupTestScrollView(1, 3, size);
  Actor testPage = gPages[1];
  Wait(application, 500);

  ScrollViewCarouselEffect effect = ScrollViewCarouselEffect::New();
  scrollView.ApplyEffect(effect);

  Actor actor = AddActorToPage(testPage, 0.5f, 0.5f, 3, 3);
  Wait(application);
  Vector3 actorPrePosition = actor.GetCurrentPosition();

  effect.ApplyToActor( actor, Vector2(1.2f, 1.2f) );

  scrollView.ScrollTo(Vector3(size.x, 0.0f, 0.0f), 0.5f, DirectionBiasNone, DirectionBiasNone);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }
  // test that the first page has reached centre of screen
  Vector3 actorPostPosition = actor.GetCurrentPosition();
  // just check the actor has moved
  DALI_TEST_CHECK((actorPostPosition - actorPrePosition).Length() > Math::MACHINE_EPSILON_1);
  CleanupTest();
  END_TEST;
}

int UtcDaliScrollViewDepthEffectTest(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScrollViewDepthEffectTest");

  Vector2 size = Stage::GetCurrent().GetSize();

  ScrollView scrollView = SetupTestScrollView(1, 3, size);
  Actor testPage = gPages[1];
  Wait(application, 500);

  ScrollViewDepthEffect effect = ScrollViewDepthEffect::New();
  scrollView.ApplyEffect(effect);

  Actor actor = AddActorToPage(testPage, 0.5f, 0.5f, 3, 3);
  Wait(application);
  Vector3 actorPrePosition = actor.GetCurrentPosition();

  const Vector2 positionExtent(0.5f, 2.5f);
  const Vector2 offsetExtent(1.0f, 1.0f);
  const float positionScale(1.5f);
  const float scaleExtent(0.5f);

  effect.ApplyToActor( actor, positionExtent, offsetExtent, positionScale, scaleExtent );

  scrollView.ScrollTo(1);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }
  // test that the first page has reached centre of screen
  Vector3 actorPostPosition = actor.GetCurrentPosition();
  // just check the actor has moved
  DALI_TEST_CHECK((actorPostPosition - actorPrePosition).Length() > Math::MACHINE_EPSILON_1);
  CleanupTest();
  END_TEST;
}
