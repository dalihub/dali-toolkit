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
#include <dali-toolkit/public-api/builder/builder.h>

using namespace Dali;
using namespace Toolkit;

namespace
{
//
// Note: To avoid escaping double quotes single quotes are used and then replaced
//       before parsing. JSON uses double quotes
//
  std::string JSON_TEXTSTYLE_ONLY("\
{                                                                  \
    'text-styles':                                                 \
    {                                                              \
        'title-text-style':{'font-name': 'Vera',                   \
                            'font-style': 'Bold',                  \
                            'point-size': 12.0,                    \
                            'weight': 'light',                     \
                            'text-color': [0.0,0.5,0.5,1],         \
                            'italic': false,                       \
                            'underline': false,                    \
                            'shadow': true,                        \
                            'glow': true,                          \
                            'outline': true,                       \
                            'shadow-color': [0.0,1.0,0.0,1.0],     \
                            'shadow-offset': [3.0,2.0],            \
                            'shadow-size': 2.0,                    \
                            'glow-color': [0.9,0.6,0.3,1.0],       \
                            'glow-intensity':0.1,                  \
                            'smooth-edge': 0.45,                   \
                            'outline-color': [1.0,0.5,0.0,1.0],    \
                            'outline-thickness': [0.7,0.6]         \
      }                                                            \
    }                                                              \
}                                                                  \
");

  std::string JSON_TEXT_ACTOR("\
{                                                              \
  'styles':                                                    \
  {                                                            \
    'basic-text':                                              \
    {                                                          \
      'type':'TextActor',                                      \
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
          'property':'rotation',                               \
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
    'styles':                                                                             \
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
                   },                                                                     \
        'theme2-text': {                                                                  \
                    'type':'TextActor',                                                   \
                    'text':'Hello',                                                       \
                    'font':'Freesans',                                                    \
                    'smooth-edge':0.8                                                     \
                   }                                                                      \
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

} // namespace



void builder_startup(void)
{
  test_return_value = TET_UNDEF;
}

void builder_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliBuilderTextActorCreateFromStyle(void)
{
  ToolkitTestApplication application;
  Stage stage = Stage::GetCurrent();

  tet_infoline(" UtcDaliBuilderTextActorCreateFromStyle");

  Builder builder = Builder::New();

  builder.LoadFromString(ReplaceQuotes(JSON_TEXT_ACTOR));

  TextActor actor( TextActor::DownCast( builder.CreateFromStyle("basic-text") ) );

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

  DALI_TEST_CHECK(actor.GetText() == "Hello");

  actor = TextActor::DownCast( builder.CreateFromStyle("*(&^") );
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

  BaseHandle handle = builder.CreateFromStyle("my-camera");
  CameraActor camera = CameraActor::DownCast(handle);

  DALI_TEST_CHECK(camera);

  Property::Value v;

  v = camera.GetProperty( camera.GetPropertyIndex("field-of-view") );
  DALI_TEST_CHECK( 0.125f == v.Get<float>() );

  v = camera.GetProperty( camera.GetPropertyIndex("aspect-ratio") );
  DALI_TEST_CHECK( 5.0f == v.Get<float>() );

  handle   = builder.CreateFromStyle("basic-text");
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
