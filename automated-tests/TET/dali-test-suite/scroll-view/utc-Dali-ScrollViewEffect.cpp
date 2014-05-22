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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/pan-gesture-event.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Toolkit;

static void Startup();
static void Cleanup();

namespace
{
static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}
} // namespace

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliScrollViewCustomEffectSetup();
static void UtcDaliScrollViewCubeEffectSetup();
static void UtcDaliScrollViewPageCubeEffectSetup();
static void UtcDaliScrollViewSpiralEffectSetup();
static void UtcDaliScrollViewPageCarouselEffectSetup();
static void UtcDaliScrollViewCarouselEffectSetup();
static void UtcDaliScrollViewDepthEffectSetup();
static void UtcDaliScrollViewSlideEffectSetup();
static void UtcDaliScrollViewTwistEffectSetup();

static void UtcDaliScrollViewCubeEffectTest();
static void UtcDaliScrollViewPageCubeEffectTest();
static void UtcDaliScrollViewSpiralEffectTest();
static void UtcDaliScrollViewPageCarouselEffectTest();
static void UtcDaliScrollViewCarouselEffectTest();
static void UtcDaliScrollViewDepthEffectTest();
static void UtcDaliScrollViewSlideEffectTest();
static void UtcDaliScrollViewTwistEffectTest();
static void UtcDaliScrollViewCustomEffectTest();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliScrollViewCustomEffectSetup, POSITIVE_TC_IDX },
    { UtcDaliScrollViewCubeEffectSetup, POSITIVE_TC_IDX },
    { UtcDaliScrollViewPageCubeEffectSetup, POSITIVE_TC_IDX },
    { UtcDaliScrollViewSpiralEffectSetup, POSITIVE_TC_IDX },
    { UtcDaliScrollViewPageCarouselEffectSetup, POSITIVE_TC_IDX },
    { UtcDaliScrollViewCarouselEffectSetup, POSITIVE_TC_IDX },
    { UtcDaliScrollViewDepthEffectSetup, POSITIVE_TC_IDX },
    { UtcDaliScrollViewSlideEffectSetup, POSITIVE_TC_IDX },
    { UtcDaliScrollViewTwistEffectSetup, POSITIVE_TC_IDX },
    { UtcDaliScrollViewCubeEffectTest, POSITIVE_TC_IDX },
    { UtcDaliScrollViewPageCubeEffectTest, POSITIVE_TC_IDX },
    { UtcDaliScrollViewSpiralEffectTest, POSITIVE_TC_IDX },
    { UtcDaliScrollViewPageCarouselEffectTest, POSITIVE_TC_IDX },
    { UtcDaliScrollViewCarouselEffectTest, POSITIVE_TC_IDX },
    { UtcDaliScrollViewDepthEffectTest, POSITIVE_TC_IDX },
    { UtcDaliScrollViewSlideEffectTest, POSITIVE_TC_IDX },
    { UtcDaliScrollViewTwistEffectTest, POSITIVE_TC_IDX },
    { UtcDaliScrollViewCustomEffectTest, POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

namespace // unnamed namespace
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
static bool gOnScrollClampedCalled;                     ///< Whether the OnScrollClamped signal was invoked.
static bool gOnSnapStartCalled;                         ///< Whether the OnSnapStart signal was invoked.
static ClampState3 gLastClampPosition;                  ///< Clamping information from OnScrollClampedEvent.
static SnapType gLastSnapType;                          ///< Snaping information from SnapEvent.
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

/**
 * Invoked when scrolling clamped.
 *
 * @param[in] event The position/scale/rotation axes that were clamped.
 */
static void OnScrollClamped( const ScrollView::ClampEvent& event )
{
  gOnScrollClampedCalled = true;
  gLastClampPosition = event.position;
}

/**
 * Invoked when a snap or flick started.
 *
 * @param[in] event The type of snap and the target position/scale/rotation.
 */
static void OnSnapStart( const ScrollView::SnapEvent& event )
{
  gOnSnapStartCalled = true;
  gLastSnapType = event.type;
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

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliScrollViewCustomEffectSetup()
{
  tet_infoline(" UtcDaliScrollViewCustomEffectSetup");

  ScrollViewCustomEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewCustomEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewCustomEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );

}

static void UtcDaliScrollViewCubeEffectSetup()
{
  tet_infoline(" UtcDaliScrollViewCubeEffectSetup");

  ScrollViewCubeEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewCubeEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewCubeEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
}

static void UtcDaliScrollViewPageCubeEffectSetup()
{
  tet_infoline(" UtcDaliScrollViewPageCubeEffectSetup");

  ScrollViewPageCubeEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewPageCubeEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewPageCubeEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
}

static void UtcDaliScrollViewSpiralEffectSetup()
{
  tet_infoline(" UtcDaliScrollViewSpiralEffectSetup");

  ScrollViewPageSpiralEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewPageSpiralEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewPageSpiralEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
}

static void UtcDaliScrollViewPageCarouselEffectSetup()
{
  tet_infoline(" UtcDaliScrollViewCarouselEffectSetup");

  ScrollViewPageCarouselEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewPageCarouselEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewPageCarouselEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
}

static void UtcDaliScrollViewCarouselEffectSetup()
{
  tet_infoline(" UtcDaliScrollViewCarouselEffectSetup");

  ScrollViewCarouselEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewCarouselEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewCarouselEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
}

static void UtcDaliScrollViewDepthEffectSetup()
{
  tet_infoline(" UtcDaliScrollViewDepthEffectSetup");

  ScrollViewDepthEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewDepthEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewDepthEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
}

static void UtcDaliScrollViewSlideEffectSetup()
{
  tet_infoline(" UtcDaliScrollViewSlideEffectSetup");

  ScrollViewSlideEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewSlideEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewSlideEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
}

static void UtcDaliScrollViewTwistEffectSetup()
{
  tet_infoline(" UtcDaliScrollViewTwistEffectSetup");

  ScrollViewTwistEffect effect;

  DALI_TEST_CHECK( !effect );

  BaseHandle handle = ScrollViewTwistEffect::New();

  DALI_TEST_CHECK( handle );

  effect = ScrollViewTwistEffect::DownCast(handle);

  DALI_TEST_CHECK( effect );
}

static void UtcDaliScrollViewCubeEffectTest()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScrollViewCubeEffectTest");

  Vector2 size = Stage::GetCurrent().GetSize();

  ScrollView scrollView = SetupTestScrollView(1, 3, size);
  Actor page = gPages[1];
  Wait(application, 500);

  ScrollViewCubeEffect effect = ScrollViewCubeEffect::New();
  scrollView.ApplyEffect(effect);

  Actor actor = AddActorToPage(page, 0.5f, 0.5f, 3, 3);
  Wait(application);
  Vector3 actorPrePosition = actor.GetCurrentPosition();

  effect.ApplyToActor(actor, page, Vector3(-105.0f, 30.0f, -240.0f), Vector2(Math::PI * 0.5f, Math::PI * 0.5f), Vector2(0.25f, 0.25f) * size);

  Actor actor2 = AddActorToPage(page, 0.5f, 0.5f, 3, 3);
  effect.ApplyToActor(actor2, Vector3(-105.0f, 30.0f, -240.0f), Vector2(Math::PI * 0.5f, Math::PI * 0.5f), Vector2(0.25f, 0.25f) * size);

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
}

static void UtcDaliScrollViewPageCubeEffectTest()
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
}

static void UtcDaliScrollViewSpiralEffectTest()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScrollViewSpiralEffectTest");

  Vector2 size = Stage::GetCurrent().GetSize();

  ScrollView scrollView = SetupTestScrollView(1, 3, size);
  Actor testPage = gPages[1];
  Wait(application, 500);

  ScrollViewPageSpiralEffect effect = ScrollViewPageSpiralEffect::New();
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
}

static void UtcDaliScrollViewPageCarouselEffectTest()
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
}

static void UtcDaliScrollViewCarouselEffectTest()
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
}

static void UtcDaliScrollViewDepthEffectTest()
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
}

static void UtcDaliScrollViewSlideEffectTest()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScrollViewSlideEffectTest");

  Vector2 size = Stage::GetCurrent().GetSize();
  Vector3 pageSize(size.x, size.y, 0.0f);

  ScrollView scrollView = SetupTestScrollView(1, 3, size);
  Actor testPage = gPages[1];
  Wait(application, 500);

  ScrollViewSlideEffect effect = ScrollViewSlideEffect::New();
  effect.SetDelayReferenceOffset(pageSize * 0.25);
  DALI_TEST_EQUALS(effect.GetDelayReferenceOffset(), pageSize * 0.25, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  effect.SetMaxDelayDuration(0.5f);
  DALI_TEST_EQUALS(effect.GetMaxDelayDuration(), 0.5f, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  effect.SetSlideDirection(false);
  DALI_TEST_CHECK(!effect.GetSlideDirection());

  scrollView.ApplyEffect(effect);

  Actor actor = AddActorToPage(testPage, 0.5f, 0.5f, 3, 3);
  Wait(application);
  Vector3 actorPrePosition = actor.GetCurrentPosition();

  effect.ApplyToActor(actor, 0.0f, 0.5f);

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
}

static void UtcDaliScrollViewTwistEffectTest()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScrollViewTwistEffectTest");

  Vector2 size = Stage::GetCurrent().GetSize();

  ScrollView scrollView = SetupTestScrollView(1, 3, size);
  Actor testPage = gPages[1];
  Wait(application, 500);

  ScrollViewTwistEffect effect = ScrollViewTwistEffect::New();
  float shrinkDist = 0.2f;
  effect.SetMinimumDistanceForShrink(shrinkDist);
  DALI_TEST_CHECK((shrinkDist - effect.GetMinimumDistanceForShrink()) < Math::MACHINE_EPSILON_0);
  effect.EnableEffect(true);
  scrollView.ApplyEffect(effect);

  Actor actor = AddActorToPage(testPage, 0.5f, 0.5f, 3, 3);
  Wait(application);
  Vector3 actorPrePosition = actor.GetCurrentPosition();

  effect.ApplyToActor( actor,
      true,
      Vector2(Math::PI_2, Math::PI_2),
      0.0f);

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
}

static void UtcDaliScrollViewCustomEffectTest()
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliScrollViewCustomEffectTest");

  Vector2 size = Stage::GetCurrent().GetSize();
  Vector3 pageSize(size.x, size.y, 0.0f);

  ScrollView scrollView = SetupTestScrollView(1, 3, size);
  Actor testPage = gPages[1];
  Wait(application, 500);
  Vector3 pageStartPos, pagePos;
  pageStartPos = pagePos = testPage.GetCurrentPosition();
  //scrollView.RemoveConstraintsFromChildren();

  ScrollViewCustomEffect effect = ScrollViewCustomEffect::DownCast(scrollView.ApplyEffect(ScrollView::PageEffectCarousel));

  for(ActorIter pageIter = gPages.begin(); pageIter != gPages.end(); ++pageIter)
  {
    Actor page = *pageIter;
    page.RemoveConstraints();
    page.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    effect.ApplyToPage(page, pageSize);
  }
  Wait(application);
  pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, pageStartPos, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  scrollView.ScrollTo(1);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }
  ResetScrollCallbackResults();
  // test that the first page has reached centre of screen
  pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, Vector3::ZERO, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // scroll back to page 0
  scrollView.ScrollTo(0);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }
  ResetScrollCallbackResults();
  pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, pageStartPos, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  scrollView.RemoveEffect(effect);

  effect = ScrollViewCustomEffect::New();
  effect.SetPageTranslation(Vector3(20.0f, 20.0f, 5.0f));
  effect.SetPageTranslation(Vector3(20.0f, 20.0f, 5.0f), Vector3(20.0f, 20.0f, -5.0f));
  effect.SetPageTranslationIn(Vector3(20.0f, 20.0f, 5.0f));
  effect.SetPageTranslationOut(Vector3(20.0f, 20.0f, -5.0f));
  effect.SetPageTranslation(Vector3(20.0f, 0.0f, 0.0f));
  effect.SetSwingAngle(Math::PI, Vector3::YAXIS);
  effect.SetPageSpacing(Vector2(20.0f, 20.0f));
  scrollView.ApplyEffect(effect);

  for(ActorIter pageIter = gPages.begin(); pageIter != gPages.end(); ++pageIter)
  {
    Actor page = *pageIter;
    page.RemoveConstraints();
    page.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    effect.ApplyToPage(page, pageSize);
  }
  Wait(application);
  pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, pageStartPos, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  scrollView.ScrollTo(1);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }
  ResetScrollCallbackResults();
  // test that the first page has reached centre of screen
  pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, Vector3::ZERO, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // scroll back to page 0
  scrollView.ScrollTo(0);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }
  ResetScrollCallbackResults();
  pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, pageStartPos, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  scrollView.RemoveEffect(effect);
  effect = ScrollViewCustomEffect::New();
  effect.SetSwingAngle(Math::PI, Vector3::YAXIS);
  effect.SetSwingAnchor(AnchorPoint::CENTER_LEFT);
  effect.SetPageTranslation(Vector3(size.x, size.y, 0));
  effect.SetOpacityThreshold(0.66f);
  scrollView.ApplyEffect(effect);

  for(ActorIter pageIter = gPages.begin(); pageIter != gPages.end(); ++pageIter)
  {
    Actor page = *pageIter;
    page.RemoveConstraints();
    page.ApplyConstraint( Constraint::New<Vector3>( Actor::SIZE, ParentSource( Actor::SIZE ), EqualToConstraint() ) );
    effect.ApplyToPage(page, pageSize);
  }
  Wait(application);

  scrollView.ScrollTo(1);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }
  ResetScrollCallbackResults();
  // test that the first page has reached centre of screen
  pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, Vector3::ZERO, Math::MACHINE_EPSILON_0, TEST_LOCATION);

  // scroll back to page 0
  scrollView.ScrollTo(0);
  while(!gOnScrollCompleteCalled)
  {
    Wait(application);
  }
  ResetScrollCallbackResults();
  pagePos = testPage.GetCurrentPosition();
  DALI_TEST_EQUALS(pagePos, pageStartPos, Math::MACHINE_EPSILON_0, TEST_LOCATION);
  scrollView.RemoveEffect(effect);


  effect.SetPageTranslateAlphaFunction(AlphaFunctions::Linear);
  effect.SetPageTranslateAlphaFunction(AlphaFunctions::Linear, AlphaFunctions::Linear);
  effect.SetPageTranslateAlphaFunctionIn(AlphaFunctions::Linear);
  effect.SetPageTranslateAlphaFunctionOut(AlphaFunctions::Linear);
  effect.SetGlobalPageRotation(Math::PI, Vector3::YAXIS);
  effect.SetAngledOriginPageRotation(Vector3(Math::PI, Math::PI, 0.0f));
  effect.SetGlobalPageRotation(Math::PI, Vector3::YAXIS, Math::PI, Vector3::YAXIS);
  effect.SetGlobalPageRotationIn(Math::PI, Vector3::YAXIS);
  effect.SetGlobalPageRotationOut(Math::PI, Vector3::YAXIS);
  effect.SetGlobalPageRotationOrigin(Vector3::ZERO);
  effect.SetGlobalPageRotationOrigin(Vector3::ZERO, Vector3::ZERO);
  effect.SetGlobalPageRotationOriginIn(Vector3::ZERO);
  effect.SetGlobalPageRotationOriginOut(Vector3::ZERO);
  effect.SetSwingAngle(Math::PI, Vector3::YAXIS);
  effect.SetSwingAngle(Math::PI, Vector3::YAXIS, Math::PI, Vector3::YAXIS);
  effect.SetSwingAngleIn(Math::PI, Vector3::YAXIS);
  effect.SetSwingAngleOut(Math::PI, Vector3::YAXIS);
  effect.SetSwingAngleAlphaFunction(AlphaFunctions::Linear);
  effect.SetSwingAngleAlphaFunction(AlphaFunctions::Linear, AlphaFunctions::Linear);
  effect.SetSwingAngleAlphaFunctionIn(AlphaFunctions::Linear);
  effect.SetSwingAngleAlphaFunctionOut(AlphaFunctions::Linear);
  effect.SetSwingAnchor(AnchorPoint::CENTER, AnchorPoint::CENTER_LEFT);
  effect.SetSwingAnchorIn(AnchorPoint::CENTER);
  effect.SetSwingAnchorOut(AnchorPoint::CENTER);
  effect.SetSwingAnchorAlphaFunction(AlphaFunctions::Linear);
  effect.SetSwingAnchorAlphaFunction(AlphaFunctions::Linear, AlphaFunctions::Linear);
  effect.SetSwingAnchorAlphaFunctionIn(AlphaFunctions::Linear);
  effect.SetSwingAnchorAlphaFunctionOut(AlphaFunctions::Linear);
  effect.SetOpacityThreshold(0.5f);
  effect.SetOpacityThreshold(0.5f, 0.5f);
  effect.SetOpacityThresholdIn(0.5f);
  effect.SetOpacityThresholdOut(0.5f);
  effect.SetOpacityAlphaFunction(AlphaFunctions::Linear);
  effect.SetOpacityAlphaFunction(AlphaFunctions::Linear, AlphaFunctions::Linear);
  effect.SetOpacityAlphaFunctionIn(AlphaFunctions::Linear);
  effect.SetOpacityAlphaFunctionOut(AlphaFunctions::Linear);
  CleanupTest();
}
