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
#include <dali-toolkit/public-api/builder/builder.h>
#include <dali/integration-api/events/touch-event-integ.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
//
// Note: To avoid escaping double quotes single quotes are used and then replaced
//       before parsing. JSON uses double quotes
//

  std::string JSON_TEXT_ACTOR("\
{                                                              \
  'templates':                                                 \
  {                                                            \
    'basic-text':                                              \
    {                                                          \
      'type':'TextActor',                                      \
      'text':'Template Hello',                                 \
      'size': [150,170,1],                                     \
      'position':[-10,10,0]                                    \
    }                                                          \
  },                                                           \
  'styles':                                                    \
  {                                                            \
    'basic-text':                                              \
    {                                                          \
      'text':'Hello',                                          \
      'font':'',                                               \
      'parent-origin':[0.0,0.0,0],                             \
      'anchor-point' :[0.5,0.5,0],                             \
      'size': [150,170,1],                                     \
      'position':[-10,10,0]                                    \
    }                                                          \
  },                                                           \
  'animations':                                                \
  {                                                            \
    'rotate':                                                  \
    {                                                          \
      'duration': 10,                                          \
      'properties':                                            \
      [                                                        \
        {                                                      \
          'actor':'text',                                      \
          'property':'orientation',                            \
          'value':[0, 3, 0, 0],                                \
          'alpha-function': 'EASE_IN_OUT',                     \
          'time-period': {'delay': 0, 'duration': 3 }          \
        }                                                      \
      ]                                                        \
    }                                                          \
  },                                                           \
  'stage':                                                     \
  [                                                            \
    {                                                          \
      'name':'text',                                           \
      'type':'basic-text',                                     \
      'text':'Hello'                                           \
    },                                                         \
    {                                                          \
      'name':'text2',                                          \
      'type':'basic-text',                                     \
      'text':'Hello',                                          \
      'signals':                                               \
      [                                                        \
        { 'name': 'on-stage', 'action':'set', 'actor':'text2', 'property':'text', 'value':'Jaylo' } \
      ]                                                        \
    }                                                          \
  ],                                                           \
  'other':                                                     \
  [                                                            \
    {                                                          \
      'name':'other-text',                                     \
      'type':'basic-text',                                     \
      'text':'Hello'                                           \
    }                                                          \
  ]                                                            \
}                                                              \
");


  std::string JSON_CORE_ACTOR_TREE("\
{                                                                                         \
    'templates':                                                                          \
    {                                                                                     \
        'my-camera': {                                                                    \
                      'type':'CameraActor',                                               \
                      'camera-type':'FreeLook',                                           \
                      'field-of-view': 0.125,                                             \
                      'aspect-ratio':5.0,                                                 \
                      'near-plane-distance': 100,                                         \
                      'far-plane-distance': 200                                           \
                      },                                                                  \
        'basic-text': {                                                                   \
                    'type':'TextActor',                                                   \
                    'text':'Hello',                                                       \
                    'font':'Freesans',                                                    \
                    'smooth-edge':0.2,                                                    \
                    'position': [-10.0, 10.0, -1000.0],                                   \
                    'size': [300.0, 250.0, 0.0]                                           \
                   }                                                                      \
    },                                                                                    \
    'styles':                                                                             \
    {                                                                                     \
       'theme2-text': {                                                                   \
                   'type':'TextActor',                                                    \
                   'text':'Hello',                                                        \
                   'font':'Freesans',                                                     \
                   'smooth-edge':0.8                                                      \
                  }                                                                       \
    },                                                                                    \
    'stage':                                                                              \
    [                                                                                     \
        {'name':'txt1',                                                                   \
         'type':'TextActor',                                                              \
         'text':'Hello World',                                                            \
         'font':'freesans',                                                               \
         'parent-origin':'CENTER',                                                        \
         'actors':                                                                        \
         [                                                                                \
           { 'type':'basic-text', 'text':'Hello', 'position-y':50 },                      \
           { 'type':'basic-text', 'text':'Hello', 'position-y':100 },                     \
           { 'type':'basic-text', 'text':'Hello', 'position-y':150 },                     \
           { 'type':'basic-text', 'text':'Hello', 'position-y':200 },                     \
           { 'type':'basic-text', 'text':'Hello', 'position-y':250 }                      \
         ]                                                                                \
        }                                                                                 \
    ]                                                                                     \
}                                                                                         \
");


std::string ReplaceQuotes(const std::string &in_s)
{
  std::string s(in_s);
  // wrong as no embedded quote but had regex link problems
  std::replace(s.begin(), s.end(), '\'', '"');
  return s;
}

struct BuilderFunctor
{
  BuilderFunctor( bool& called ) : mCalled( called )
  {
    mCalled = false;
  }

  void operator()()
  {
    mCalled = true;
  }

  bool& mCalled;
};

} // namespace



void builder_startup(void)
{
  test_return_value = TET_UNDEF;
}

void builder_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliBuilderTextActorCreate(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" UtcDaliBuilderTextActorCreate");

  Builder builder = Builder::New();

  builder.LoadFromString(ReplaceQuotes(JSON_TEXT_ACTOR));

  TextActor actor( TextActor::DownCast( builder.Create("basic-text") ) );

  DALI_TEST_CHECK( actor );

  stage.GetRootLayer().Add( actor );

  application.SendNotification();
  application.Render();

  Vector3 v;

  v = actor.GetCurrentPosition();
  DALI_TEST_CHECK(v.x == -10.0);
  DALI_TEST_CHECK(v.y ==  10.0);
  DALI_TEST_CHECK(v.z ==  0.0);

  v = actor.GetCurrentSize();
  DALI_TEST_CHECK(v.x == 150.0);
  DALI_TEST_CHECK(v.y == 170.0);
  DALI_TEST_CHECK(v.z == 1.0);

  DALI_TEST_CHECK(actor.GetText() == "Template Hello");

  actor = TextActor::DownCast( builder.Create("*(&^") );
  DALI_TEST_CHECK(!actor);

  END_TEST;
}

int UtcDaliBuilderTextActorCreateAnimation(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBuilderTextActorCreateAnimation");

  Builder builder = Builder::New();

  builder.LoadFromString(ReplaceQuotes(JSON_TEXT_ACTOR));

  builder.AddActors( Stage::GetCurrent().GetRootLayer() );

  Animation anim = builder.CreateAnimation("rotate");
  DALI_TEST_CHECK( anim );

  DALI_TEST_CHECK( 10.0f == anim.GetDuration() );

  END_TEST;
}

int UtcDaliBuilderTextActorApplyFromStyle(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBuilderTextActorApplyFromStyle");

  Builder builder = Builder::New();

  builder.LoadFromString(ReplaceQuotes(JSON_TEXT_ACTOR));

  TextActor actor = TextActor::New("a");

  builder.ApplyStyle("basic-text", actor);

  DALI_TEST_CHECK( actor );

  Stage::GetCurrent().GetRootLayer().Add( actor );

  application.SendNotification();
  application.Render();

  Vector3 v;

  v = actor.GetCurrentPosition();
  DALI_TEST_CHECK(v.x == -10.0);
  DALI_TEST_CHECK(v.y ==  10.0);
  DALI_TEST_CHECK(v.z ==  0.0);

  v = actor.GetCurrentSize();
  DALI_TEST_CHECK(v.x == 150.0);
  DALI_TEST_CHECK(v.y == 170.0);
  DALI_TEST_CHECK(v.z == 1.0);

  DALI_TEST_CHECK(actor.GetText() == "Hello");

  END_TEST;
}

int UtcDaliBuilderAddActors(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBuilderAddActors");

  Builder builder = Builder::New();

  builder.LoadFromString(ReplaceQuotes(JSON_TEXT_ACTOR));

  builder.AddActors( Stage::GetCurrent().GetRootLayer() );

  application.SendNotification();
  application.Render();

  TextActor actor = TextActor::DownCast( Stage::GetCurrent().GetRootLayer().FindChildByName("text") );

  DALI_TEST_CHECK( actor );
  DALI_TEST_CHECK(actor.GetText() == "Hello");

  END_TEST;
}

int UtcDaliBuilderAddActorsOther(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBuilderAddActorsOther");

  Actor rootActor = Stage::GetCurrent().GetRootLayer();

  Builder builder = Builder::New();

  builder.LoadFromString(ReplaceQuotes(JSON_TEXT_ACTOR));

  builder.AddActors( "other", rootActor  );

  application.SendNotification();
  application.Render();

  TextActor actor = TextActor::DownCast( Stage::GetCurrent().GetRootLayer().FindChildByName("other-text") );

  DALI_TEST_CHECK( actor );
  DALI_TEST_CHECK(actor.GetText() == "Hello");

  END_TEST;
}


int UtcDaliBuilderStyles(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBuilderStyles");

  Builder builder = Builder::New();

  builder.LoadFromString(ReplaceQuotes(JSON_CORE_ACTOR_TREE));

  BaseHandle handle = builder.Create("my-camera");
  CameraActor camera = CameraActor::DownCast(handle);

  DALI_TEST_CHECK(camera);

  Property::Value v;

  v = camera.GetProperty( camera.GetPropertyIndex("field-of-view") );
  DALI_TEST_CHECK( 0.125f == v.Get<float>() );

  v = camera.GetProperty( camera.GetPropertyIndex("aspect-ratio") );
  DALI_TEST_CHECK( 5.0f == v.Get<float>() );

  handle   = builder.Create("basic-text");
  TextActor textActor = TextActor::DownCast(handle);

  v = textActor.GetProperty( textActor.GetPropertyIndex("smooth-edge") );

  DALI_TEST_CHECK( 0.2f == v.Get<float>() );

  // test ApplyStyle another
  builder.ApplyStyle("theme2-text", textActor);

  v = textActor.GetProperty( textActor.GetPropertyIndex("smooth-edge") );
  DALI_TEST_CHECK( 0.8f == v.Get<float>() );

  END_TEST;
}

int UtcDaliBuilderSetProperty(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBuilderSetProperty");

  Builder builder = Builder::New();

  builder.LoadFromString(ReplaceQuotes(JSON_TEXT_ACTOR));

  builder.AddActors( Stage::GetCurrent().GetRootLayer() );

  application.SendNotification();
  application.Render();

  TextActor actor = TextActor::DownCast( Stage::GetCurrent().GetRootLayer().FindChildByName("text2") );

  DALI_TEST_CHECK( actor );
  DALI_TEST_CHECK( actor.GetText() == "Jaylo" );

  END_TEST;
}

int UtcDaliBuilderCreateFromJson(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBuilderCreateFromJson");

  Builder builder = Builder::New();

  TextActor actor = TextActor::DownCast( builder.CreateFromJson("foobar") );

  DALI_TEST_CHECK( !actor );

  actor = TextActor::DownCast(
    builder.CreateFromJson(
      ReplaceQuotes("{'type':'TextActor','text':'Hi'}") ) );

  DALI_TEST_CHECK( actor );

  DALI_TEST_CHECK( actor.GetText() == "Hi" );

  END_TEST;
}

int UtcDaliBuilderApplyFromJson(void)
{
  ToolkitTestApplication application;

  tet_infoline(" UtcDaliBuilderApplyFromJson");

  Builder builder = Builder::New();

  TextActor actor = TextActor::DownCast(
    builder.CreateFromJson(
      ReplaceQuotes("{'type':'TextActor','text':'Hi'}") ) );

  DALI_TEST_CHECK( actor );

  DALI_TEST_CHECK( actor.GetText() == "Hi" );

  DALI_TEST_CHECK( !builder.ApplyFromJson(actor, ReplaceQuotes("foobar") ) );

  builder.ApplyFromJson(actor, ReplaceQuotes("{'text':'low'}") );

  DALI_TEST_CHECK( actor.GetText() == "low" );

  END_TEST;
}

int UtcDaliBuilderQuitSignal(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{"
         "\"stage\":"
         "[{"
           "\"type\": \"Actor\","
           "\"size\": [100,100,1],"
           "\"parent-origin\": \"TOP_LEFT\","
           "\"anchor-point\": \"TOP_LEFT\","
           "\"signals\": [{"
             "\"name\": \"touched\","
             "\"action\": \"quit\""
           "}]"
         "}]"
      "}"
  );
  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors ( Stage::GetCurrent().GetRootLayer() );

  // Connect to builder's quit signal
  bool functorCalled( false );
  builder.QuitSignal().Connect( &application, BuilderFunctor( functorCalled ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit touch event and check that our quit method is called
  Integration::TouchEvent touchEvent;
  touchEvent.points.push_back( TouchPoint ( 0, TouchPoint::Down, 10.0f, 10.0f ) );
  application.ProcessEvent( touchEvent );
  DALI_TEST_CHECK( functorCalled );

  END_TEST;
}
