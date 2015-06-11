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
#include <dali-toolkit/devel-api/builder/builder.h>
#include <dali/integration-api/events/touch-event-integ.h>

#define STRINGIFY(A)#A

using namespace Dali;
using namespace Toolkit;

namespace
{

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


int UtcDaliBuilderAnimationP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
        "{"
        "   \"paths\":"
        "   {"
        "     \"path0\":"
        "     {"
        "       \"points\":[ [-150, -50, 0], [0.0,70.0,0.0], [190.0,-150.0,0.0] ],"
        "       \"curvature\":0.35"
        "     }"
        "   },"
        "  \"animations\": {"
        "    \"animate\": {"
        "      \"loop\": true,"
        "      \"end-action\": \"BAKE\","
        "      \"disconnect-action\": \"BAKE\","
        "      \"properties\":"
        "      [{"
        "        \"actor\": \"greeting\","
        "        \"property\": \"position\","
        "        \"value\": [300, 300, -1000],"
        "        \"alpha-function\": \"EASE_IN_OUT\","
        "        \"relative\": true,"
        "        \"time-period\": {"
        "          \"delay\": 0,"
        "          \"duration\": 3"
        "        }"
        "      },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"visible\","
        "         \"alpha-function\": \"LINEAR\","
        "         \"value\": true"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"size-width\","
        "         \"alpha-function\": \"REVERSE\","
        "         \"value\": 10.0"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alpha-function\": \"EASE_IN\","
        "         \"value\": [10.0,20.0,30.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alpha-function\": \"EASE_OUT\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alpha-function\": \"EASE_IN_OUT\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alpha-function\": \"EASE_IN_SINE\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alpha-function\": \"EASE_OUT_SINE\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alpha-function\": \"EASE_IN_OUT_SINE\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alpha-function\": \"BOUNCE\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alpha-function\": \"SIN\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alpha-function\": \"EASE_OUT_BACK\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       }"
        "      ]"
        "    },"
        "    \"path-animation\": {"
        "      \"duration\": 3.0,"
        "      \"end-action\": \"DISCARD\","
        "      \"disconnect-action\": \"BAKE_FINAL\","
        "      \"properties\": [{"
        "        \"actor\": \"greeting\","
        "        \"path\":\"path0\","
        "        \"forward\":[1,0,0],"
        "        \"alpha-function\": \"EASE_IN_OUT\","
        "        \"time-period\": {"
        "          \"delay\": 0,"
        "          \"duration\": 3"
        "        }"
        "      }]"
        "    }"
        "  },"
        "  \"stage\": [{"
        "    \"name\": \"greeting\","
        "    \"type\": \"TextLabel\","
        "    \"text\": \"Touch me\","
        "    \"styles\": [\"basic-text\"],"
        "    \"position\": [0, -120, 0],"
        "    \"size\": [200, 200, 1],"
        "    \"orientation\": [0, 0, 30],"
        "    \"signals\": [{"
        "      \"name\": \"touched\","
        "      \"action\": \"play\","
        "      \"animation\": \"animate\""
        "    }]"
        "  }]"
        "}");

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors( Stage::GetCurrent().GetRootLayer() );

  Animation anim = builder.CreateAnimation("animate");

  DALI_TEST_CHECK( anim );

  anim = builder.CreateAnimation("path-animation");

  DALI_TEST_CHECK( anim );

  // trigger play
  // Emit touch event and check that our quit method is called
  Integration::TouchEvent touchEvent;
  touchEvent.points.push_back( TouchPoint ( 0, TouchPoint::Down, 10.0f, 10.0f ) );
  application.ProcessEvent( touchEvent );

  // Render and notify
  application.SendNotification();
  application.Render();


  END_TEST;
}

int UtcDaliBuilderAnimationN(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
        "{"
        "   \"paths\":"
        "   {"
        "     \"path0\":"
        "     {"
        "       \"points\":[ [-150, -50, 0], [0.0,70.0,0.0], [190.0,-150.0,0.0] ],"
        "       \"curvature\":0.35"
        "     }"
        "   },"
        "  \"animations\": {"
        "    \"animate\": {"
        "      \"loop\": true,"
        "      \"end-action\": \"BAKE\","
        "      \"disconnect-action\": \"BAKE\","
        "      \"properties\":"
        "      [{"
        "        \"actor\": \"greeting\","
        "        \"property\": \"positioninvalid\","
        "        \"value\": [300, 300, -1000],"
        "        \"alpha-function\": \"EASE_IN_OUT\","
        "        \"relative\": true,"
        "        \"time-period\": {"
        "          \"delay\": 0,"
        "          \"duration\": 3"
        "        }"
        "      }"
        "      ]"
        "    },"
        "    \"animate2\": {"
        "      \"loop\": true,"
        "      \"end-action\": \"BAKE\","
        "      \"disconnect-action\": \"BAKE\","
        "      \"properties\":"
        "      [{"
        "        \"actor\": \"greeting\","
        "        \"property\": \"positioninvalid\","
        "        \"value\": [300, 300, -1000],"
        "        \"alpha-function\": \"EGGS_OVER_EASY\","
        "        \"relative\": true,"
        "        \"time-period\": {"
        "          \"delay\": 0,"
        "          \"duration\": 3"
        "        }"
        "      }"
        "      ]"
        "    },"
        "    \"path-animation\": {"
        "      \"duration\": 3.0,"
        "      \"end-action\": \"DISCARD\","
        "      \"disconnect-action\": \"BAKE_FINAL\","
        "      \"properties\": [{"
        "        \"actor\": \"greeting\","
        "        \"path\":\"pathDoesntExist\","
        "        \"forward\":[1,0,0],"
        "        \"alpha-function\": \"EASE_IN_OUT\","
        "        \"time-period\": {"
        "          \"delay\": 0,"
        "          \"duration\": 3"
        "        }"
        "      }]"
        "    }"
        "  },"
        "  \"stage\": [{"
        "    \"name\": \"greeting\","
        "    \"type\": \"TextLabel\","
        "    \"text\": \"Touch me\","
        "    \"styles\": [\"basic-text\"],"
        "    \"position\": [0, -120, 0],"
        "    \"size\": [200, 200, 1],"
        "    \"orientation\": [0, 0, 30],"
        "    \"signals\": [{"
        "      \"name\": \"touched\","
        "      \"action\": \"play\","
        "      \"animation\": \"animate\""
        "    }]"
        "  }]"
        "}");


  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors( Stage::GetCurrent().GetRootLayer() );

  Animation anim = builder.CreateAnimation("animate");

  // log warning line coverage
  anim = builder.CreateAnimation("path-animation");

  anim = builder.CreateAnimation("animate");

  anim = builder.CreateAnimation("animate2");

  DALI_TEST_CHECK(true);

  END_TEST;

}

int UtcDaliBuilderConstantsP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{"
      "\"constants\":"
      "{"
      "  \"IMAGE_PATH\": \"apath\","
      "  \"WIDTH\": 22.3,"
      "  \"ANCHOR\": \"TOP_LEFT\","
      "  \"PADDING\": [1,2,3,4]"
      "},"
      "\"stage\":"
      "[{"
      "  \"type\": \"ImageActor\","
      "  \"size\": [100,100,1],"
      "  \"parent-origin\": \"TOP_LEFT\","
      "  \"anchor-point\": \"{ANCHOR}\","
      "  \"padding\": \"{PADDING}\","
      "  \"image\": { \"filename\": \"dir/{IMAGE_PATH}\" },"
      "  \"size-width\": \"{WIDTH}\","
      "  \"signals\": [{"
      "    \"name\": \"touched\","
      "    \"action\": \"quit\""
      "  }]"
      "}]"
      "}"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors ( Stage::GetCurrent().GetRootLayer() );
  DALI_TEST_CHECK( builder );

  END_TEST;
}

int UtcDaliBuilderTemplatesAndStylesP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "\"styles\":\n"
      "{\n"
      "  \"image-style\": \n"
      "  {\n"
      "    \"color\": [1,0,0,1],\n"
      "    \"actors\": {\n"
      "      \"child-image\": {\n"
      "        \"color\": [0,1,0,1]\n"
      "      }\n"
      "    }\n"
      "  }\n"
      "},\n"
      "\"templates\":\n"
      "{\n"
      "  \"image-tree\": { \n"
      "    \"type\": \"ImageActor\",\n"
      "    \"styles\": [\"image-style\"],\n"
      "    \"name\": \"image\",\n"
      "    \"size\": [100,100,1],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touched\",\n"
      "      \"action\": \"quit\"\n"
      "    }],\n"
      "    \"actors\": [\n"
      "      {\n"
      "        \"type\":\"ImageActor\",\n"
      "        \"name\":\"child-image\" \n"
      "      }\n"
      "    ]\n"
      "  }\n"
      "}\n"
      "}\n"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );

  ImageActor actor = ImageActor::DownCast( builder.Create( "image-tree" ) );
  DALI_TEST_CHECK( actor );

  // NB: already applied in create
  DALI_TEST_CHECK( builder.ApplyStyle( "image-style", actor ) );

  END_TEST;
}

int UtcDaliBuilderRenderTasksP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "\"render-tasks\":\n"
      "{\n"
      "  \"task0\": {\n"
      "    \"source-actor\": \"image\",\n"
      "    \"camera-actor\": \"camera\" \n"
      "  }\n"
      "},\n"
      "\"stage\":\n"
      "[\n"
      "  { \n"
      "    \"type\": \"CameraActor\",\n"
      "    \"name\": \"image\"\n"
      "  }, \n"
      "  { \n"
      "    \"type\": \"ImageActor\",\n"
      "    \"name\": \"image\",\n"
      "    \"size\": [100,100,1],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touched\",\n"
      "      \"action\": \"quit\"\n"
      "    }],\n"
      "    \"actors\": [\n"
      "      {\n"
      "        \"type\":\"ImageActor\",\n"
      "        \"name\":\"child-image\" \n"
      "      }\n"
      "    ]\n"
      "  }\n"
      "]\n"
      "}\n"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );

  unsigned int count = Stage::GetCurrent().GetRenderTaskList().GetTaskCount();

  // coverage
  builder.CreateRenderTask( "task0" );

  DALI_TEST_CHECK( count <
                   Stage::GetCurrent().GetRenderTaskList().GetTaskCount() );

  END_TEST;
}

int UtcDaliBuilderChildActionP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "  \"stage\":\n"
      "  [{\n"
      "    \"type\": \"Actor\",\n"
      "    \"name\": \"actor\",\n"
      "    \"size\": [100,100,1],\n"
      "    \"parent-origin\": \"TOP_LEFT\",\n"
      "    \"anchor-point\": \"TOP_LEFT\",\n"
      "    \"actors\": [{\n"
      "      \"type\": \"Actor\",\n"
      "      \"name\": \"sub-actor\"\n"
      "    }],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touched\",\n"
      "      \"action\": \"hide\",\n"
      "      \"actor\": \"actor\",\n"
      "      \"child-actor\": \"sub-actor\"\n"
      "    }]\n"
      "  }]\n"
      "}\n"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors ( Stage::GetCurrent().GetRootLayer() );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit touch event and check that our quit method is called
  Integration::TouchEvent touchEvent;
  touchEvent.points.push_back( TouchPoint ( 0, TouchPoint::Down, 10.0f, 10.0f ) );
  application.ProcessEvent( touchEvent );

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName("sub-actor");
  DALI_TEST_CHECK( actor );

  DALI_TEST_CHECK( !actor.IsVisible() );

  END_TEST;
}

int UtcDaliBuilderSetPropertyActionP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "  \"stage\":\n"
      "  [{\n"
      "    \"type\": \"Actor\",\n"
      "    \"name\": \"actor\",\n"
      "    \"size\": [100,100,1],\n"
      "    \"parent-origin\": \"TOP_LEFT\",\n"
      "    \"anchor-point\": \"TOP_LEFT\",\n"
      "    \"actors\": [{\n"
      "      \"type\": \"Actor\",\n"
      "      \"name\": \"sub-actor\"\n"
      "    }],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touched\",\n"
      "      \"action\": \"set\",\n"
      "      \"actor\": \"sub-actor\",\n"
      "      \"property\": \"visible\",\n"
      "      \"value\": false\n"
      "    }]\n"
      "  }]\n"
      "}\n"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors ( Stage::GetCurrent().GetRootLayer() );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit touch event and check that our quit method is called
  Integration::TouchEvent touchEvent;
  touchEvent.points.push_back( TouchPoint ( 0, TouchPoint::Down, 10.0f, 10.0f ) );
  application.ProcessEvent( touchEvent );

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName("sub-actor");
  DALI_TEST_CHECK( actor );

  DALI_TEST_CHECK( !actor.IsVisible() );

  END_TEST;
}

int UtcDaliBuilderGenericActionP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "  \"stage\":\n"
      "  [{\n"
      "    \"type\": \"Actor\",\n"
      "    \"name\": \"actor\",\n"
      "    \"size\": [100,100,1],\n"
      "    \"parent-origin\": \"TOP_LEFT\",\n"
      "    \"anchor-point\": \"TOP_LEFT\",\n"
      "    \"actors\": [{\n"
      "      \"type\": \"Actor\",\n"
      "      \"name\": \"sub-actor\"\n"
      "    }],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touched\",\n"
      "      \"action\": \"hide\"\n"
      "    }]\n"
      "  }]\n"
      "}\n"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors ( Stage::GetCurrent().GetRootLayer() );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit touch event and check that our quit method is called
  Integration::TouchEvent touchEvent;
  touchEvent.points.push_back( TouchPoint ( 0, TouchPoint::Down, 10.0f, 10.0f ) );
  application.ProcessEvent( touchEvent );

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName("actor");
  DALI_TEST_CHECK( actor );

  DALI_TEST_CHECK( !actor.IsVisible() );

  END_TEST;
}

int UtcDaliBuilderPropertyNotificationP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "  \"stage\":\n"
      "  [{\n"
      "    \"type\": \"Actor\",\n"
      "    \"name\": \"actor\",\n"
      "    \"size\": [100,100,1],\n"
      "    \"parent-origin\": \"TOP_LEFT\",\n"
      "    \"anchor-point\": \"TOP_LEFT\",\n"
      "    \"actors\": [{\n"
      "      \"type\": \"Actor\",\n"
      "      \"name\": \"sub-actor\"\n"
      "    }],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touched\",\n"
      "      \"action\": \"hide\"\n"
      "    }],\n"
      "    \"notifications\": [{\n"
      "      \"property\": \"visible\",\n"
      "      \"condition\": \"False\",\n"
      "      \"action\": \"show\"\n"
      "    }]\n"
      "  }]\n"
      "}\n"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors ( Stage::GetCurrent().GetRootLayer() );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit touch event and check that our quit method is called
  Integration::TouchEvent touchEvent;
  touchEvent.points.push_back( TouchPoint ( 0, TouchPoint::Down, 10.0f, 10.0f ) );
  application.ProcessEvent( touchEvent );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName("actor");
  DALI_TEST_CHECK( actor );

  DALI_TEST_CHECK( actor.IsVisible() );

  END_TEST;
}
