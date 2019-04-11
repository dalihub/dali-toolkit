/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <iterator>
#include <vector>
#include <algorithm>
#include <cstdlib>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/devel-api/builder/builder.h>
#include <dali-toolkit/devel-api/builder/base64-encoding.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/dali-toolkit.h>
#include <test-button.h>
#include <test-animation-data.h>
#include <dummy-control.h>


#define STRINGIFY(A)#A

using namespace Dali;
using namespace Toolkit;

namespace BuilderControlProperty
{

enum
{
  INTEGER_PROPERTY = Toolkit::Control::CONTROL_PROPERTY_END_INDEX + 1,
  MATRIX3_PROPERTY,
  MATRIX_PROPERTY,
  NONE_PROPERTY
};

namespace
{

BaseHandle Create()
{
  return Toolkit::Control::New();
}

int gSetPropertyCalledCount = 0;

void SetProperty( BaseObject* object, Property::Index propertyIndex, const Property::Value& value )
{
  ++gSetPropertyCalledCount;
}

Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  return Property::Value();
}

} // unnamed namespace

// Properties
Dali::TypeRegistration typeRegistration( "BuilderControl", typeid( Toolkit::Control ), Create );

Dali::PropertyRegistration propertyInteger( typeRegistration, "integerProperty", INTEGER_PROPERTY, Property::INTEGER, &SetProperty, &GetProperty );
Dali::PropertyRegistration propertyMatrix3( typeRegistration, "matrix3Property", MATRIX3_PROPERTY, Property::MATRIX3, &SetProperty, &GetProperty );
Dali::PropertyRegistration propertyMatrix(  typeRegistration, "matrixProperty",  MATRIX_PROPERTY,  Property::MATRIX,  &SetProperty, &GetProperty );
Dali::PropertyRegistration propertyNone(    typeRegistration, "noneProperty",    NONE_PROPERTY,    Property::NONE,    &SetProperty, &GetProperty );

}

namespace
{

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
           "\"type\": \"Layer\","
           "\"size\": [100,100,1],"
           "\"parentOrigin\": \"TOP_LEFT\","
           "\"anchorPoint\": \"TOP_LEFT\","
           "\"maximumSize\": [100,100],"
           "\"orientation\": [10,10,10,10],"
           "\"clippingBox\": [10,10,10,10],"
           "\"signals\": [{"
             "\"name\": \"touch\","
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
  Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
  touchEvent.points.push_back( point );
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
        "   \"constants\":"
        "   {"
        "     \"ALPHA_FUNCTION\":\"EASE_IN_OUT\""
        "   },"
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
        "      \"endAction\": \"BAKE\","
        "      \"disconnectAction\": \"BAKE\","
        "      \"properties\":"
        "      [{"
        "        \"actor\": \"greeting\","
        "        \"property\": \"position\","
        "        \"value\": [300, 300, -1000],"
        "        \"alphaFunction\": \"{ALPHA_FUNCTION}\","
        "        \"relative\": true,"
        "        \"timePeriod\": {"
        "          \"delay\": 0,"
        "          \"duration\": 3"
        "        }"
        "      },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"visible\","
        "         \"alphaFunction\": \"LINEAR\","
        "         \"value\": true"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"sizeWidth\","
        "         \"alphaFunction\": \"REVERSE\","
        "         \"value\": 10.0"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alphaFunction\": \"EASE_IN\","
        "         \"value\": [10.0,20.0,30.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alphaFunction\": \"EASE_OUT\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alphaFunction\": \"EASE_IN_OUT\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alphaFunction\": \"EASE_IN_SINE\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alphaFunction\": \"EASE_OUT_SINE\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alphaFunction\": \"EASE_IN_OUT_SINE\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alphaFunction\": \"BOUNCE\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alphaFunction\": \"SIN\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       },"
        "       {"
        "         \"actor\": \"greeting\","
        "         \"property\": \"orientation\","
        "         \"alphaFunction\": \"EASE_OUT_BACK\","
        "         \"value\": [0.0, 0.0, 0.0, 1.0]"
        "       }"
        "      ]"
        "    },"
        "    \"pathAnimation\": {"
        "      \"duration\": 3.0,"
        "      \"endAction\": \"DISCARD\","
        "      \"disconnectAction\": \"BAKE_FINAL\","
        "      \"properties\": [{"
        "        \"actor\": \"greeting\","
        "        \"path\":\"path0\","
        "        \"forward\":[1,0,0],"
        "        \"alphaFunction\": \"EASE_IN_OUT\","
        "        \"timePeriod\": {"
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
        "    \"inherit\": [\"basicText\"],"
        "    \"position\": [0, -120, 0],"
        "    \"size\": [200, 200, 1],"
        "    \"orientation\": [0, 0, 30],"
        "    \"signals\": [{"
        "      \"name\": \"touch\","
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

  Property::Map map;
  map["ALPHA_FUNCTION"] = "EASE_IN_SQUARE";
  anim = builder.CreateAnimation("animate", map);

  DALI_TEST_CHECK( anim );

  anim = builder.CreateAnimation("pathAnimation");

  DALI_TEST_CHECK( anim );

  // trigger play
  // Emit touch event and check that our quit method is called
  Integration::TouchEvent touchEvent;
  Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
  touchEvent.points.push_back( point );
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
        "   \"constants\":"
        "   {"
        "     \"TEXT\": \"Touch Me\","
        "     \"NAME\": \"greeting\" "
        "   },"
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
        "      \"endAction\": \"BAKE\","
        "      \"disconnectAction\": \"BAKE\","
        "      \"properties\":"
        "      [{"
        "        \"actor\": \"{NAME}\","
        "        \"property\": \"positioninvalid\","
        "        \"value\": [300, 300, -1000],"
        "        \"alphaFunction\": \"EASE_IN_OUT\","
        "        \"relative\": true,"
        "        \"timePeriod\": {"
        "          \"delay\": 0,"
        "          \"duration\": 3"
        "        }"
        "      }"
        "      ]"
        "    },"
        "    \"animate2\": {"
        "      \"loop\": true,"
        "      \"endAction\": \"BAKE\","
        "      \"disconnectAction\": \"BAKE\","
        "      \"properties\":"
        "      [{"
        "        \"actor\": \"{NAME}\","
        "        \"property\": \"positioninvalid\","
        "        \"value\": [300, 300, -1000],"
        "        \"alphaFunction\": \"EGGS_OVER_EASY\","
        "        \"relative\": true,"
        "        \"timePeriod\": {"
        "          \"delay\": 0,"
        "          \"duration\": 3"
        "        }"
        "      }"
        "      ]"
        "    },"
        "    \"pathAnimation\": {"
        "      \"duration\": 3.0,"
        "      \"endAction\": \"DISCARD\","
        "      \"disconnectAction\": \"BAKE_FINAL\","
        "      \"properties\": [{"
        "        \"actor\": \"greeting\","
        "        \"path\":\"pathDoesntExist\","
        "        \"forward\":[1,0,0],"
        "        \"alphaFunction\": \"EASE_IN_OUT\","
        "        \"timePeriod\": {"
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
        "    \"inherit\": [\"basicText\"],"
        "    \"position\": [0, -120, 0],"
        "    \"size\": [200, 200, 1],"
        "    \"orientation\": [0, 0, 30],"
        "    \"signals\": [{"
        "      \"name\": \"touch\","
        "      \"action\": \"play\","
        "      \"animation\": \"animate\""
        "    }]"
        "  },"
        "  {"
        "    \"name\": \"greeting2\","
        "    \"type\": \"TextLabel\","
        "    \"text\": \"Touch me\""
        "  }]"
        "}");


  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors( Stage::GetCurrent().GetRootLayer() );

  Animation anim = builder.CreateAnimation("animate");

  // log warning line coverage
  anim = builder.CreateAnimation("pathAnimation");
  DALI_TEST_CHECK(anim);

  anim = builder.CreateAnimation("animate");
  DALI_TEST_CHECK(anim);

  anim = builder.CreateAnimation("animate2");
  DALI_TEST_CHECK(anim);

  // create referencing a different actor aka animation templates
  Property::Map map;
  map["NAME"] = "greeting2";
  anim = builder.CreateAnimation("animate2", map);
  DALI_TEST_CHECK(anim);

  // alternative actor to use for FindChildByName
  anim = builder.CreateAnimation("animate2", Dali::Stage::GetCurrent().GetRootLayer());
  DALI_TEST_CHECK(anim);

  // alternative actor to use for FindChildByName
  anim = builder.CreateAnimation("animate2", map, Dali::Stage::GetCurrent().GetRootLayer());
  DALI_TEST_CHECK(anim);


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
      "  \"type\": \"ImageView\","
      "  \"name\": \"{NAME}\","
      "  \"size\": [100,100,1],"
      "  \"parentOrigin\": \"TOP_LEFT\","
      "  \"anchorPoint\": \"{ANCHOR}\","
      "  \"padding\": \"{PADDING}\","
      "  \"image\": { \"url\": \"dir/{IMAGE_PATH}\" },"
      "  \"sizeWidth\": \"{WIDTH}\","
      "  \"signals\": [{"
      "    \"name\": \"touch\","
      "    \"action\": \"quit\""
      "  }]"
      "}]"
      "}"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );

  builder.AddConstant( "NAME", "image" );

  Property::Map map = builder.GetConstants();

  Dali::Property::Value* pValue = map.Find( "NAME" );

  DALI_TEST_CHECK( pValue );

  pValue = map.Find( "IMAGE_PATH" );

  DALI_TEST_CHECK( pValue );

  Dali::Property::Value value = builder.GetConstant( "WIDTH" );

  DALI_TEST_CHECK( value.GetType() != Property::NONE );

  builder.AddActors ( Stage::GetCurrent().GetRootLayer() );
  DALI_TEST_CHECK( builder );

  Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName("image");
  DALI_TEST_CHECK( actor );

  END_TEST;
}

int UtcDaliBuilderTemplatesAndStylesP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "\"constants\":"
      "{"
      "  \"SIZE\": [10,20,30]"
      "},"
      "\"styles\":\n"
      "{\n"
      "  \"imageStyle\": \n"
      "  {\n"
      "    \"color\": [1,0,0,1],\n"
      "    \"actors\": {\n"
      "      \"childImage\": {\n"
      "        \"color\": \"34\"\n"
      "      }\n"
      "    }\n"
      "  }\n"
      "},\n"
      "\"templates\":\n"
      "{\n"
      "  \"imageViewTemplate\": { \n"
      "    \"type\": \"ImageView\",\n"
      "    \"styles\": [\"imageStyle\"]\n"
      "  },\n"
      "  \"imageTree\": { \n"
      "    \"type\": \"ImageView\",\n"
      "    \"styles\": [\"imageStyle\"],\n"
      "    \"name\": \"image\",\n"
      "    \"size\": \"{SIZE}\",\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touch\",\n"
      "      \"action\": \"quit\"\n"
      "    }],\n"
      "    \"actors\": [\n"
      "      {\n"
      "        \"type\":\"ImageView\",\n"
      "        \"name\":\"childImage\", \n"
      "        \"color\": \n"
      "          {\n"
      "            \"r\": 10,\n"
      "            \"g\": 10,\n"
      "            \"b\": 10,\n"
      "            \"a\": 100\n"
      "          }\n"
      "      },\n"
      "      {\n"
      "        \"type\":\"imageViewTemplate\",\n"
      "        \"name\":\"childImage2\"\n"
      "      }\n"
      "    ]\n"
      "  }\n"
      "},\n"
      "\"stage\":"
      "[{"
      "  \"type\": \"imageTree\","
      "  \"size\": [100,100,1]"
      "}]"
      "}\n"
  );

  std::string stylejson(
    "{\n"
    " \"color\": [1,0,0,1],\n"
    " \"actors\": {\n"
    "   \"childImage\": {\n"
    "     \"color\": \"#344353\"\n"
    "   }\n"
    " }\n"
    "}\n"
    );

  std::string templatejson(
    "{ \n"
    "  \"type\": \"ImageView\",\n"
    "  \"styles\": [\"imageStyle\"],\n"
    "  \"name\": \"image\",\n"
    "  \"size\": \"{SIZE}\",\n"
    "  \"signals\": [{\n"
    "    \"name\": \"touch\",\n"
    "    \"action\": \"quit\"\n"
    "  }],\n"
    "  \"actors\": [\n"
    "    {\n"
    "      \"type\":\"ImageView\",\n"
    "      \"name\":\"childImage\" \n"
    "    }\n"
    "  ]\n"
    "}\n"
    );

  Builder builder = Builder::New();
  builder.LoadFromString( json );

  ImageView actor = ImageView::DownCast( builder.Create( "imageTree" ) );
  DALI_TEST_CHECK( actor );

  Dali::Property::Map map;
  map["SIZE"] = Vector3(100,100,1);
  actor = ImageView::DownCast( builder.Create( "imageTree",  map ) );
  DALI_TEST_CHECK( actor );

  // create from json snippet
  actor = ImageView::DownCast( builder.CreateFromJson( templatejson ) );
  DALI_TEST_CHECK( actor );


  // NB: already applied in create
  DALI_TEST_CHECK( builder.ApplyStyle( "imageStyle",  actor ) );

  // apply from json snippet
  DALI_TEST_CHECK( builder.ApplyFromJson( actor, stylejson ) );

  END_TEST;
}

int UtcDaliBuilderRenderTasksP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "\"renderTasks\":\n"
      "{\n"
      "  \"task0\": {\n"
      "    \"sourceActor\": \"image\",\n"
      "    \"cameraActor\": \"camera\" \n"
      "  }\n"
      "},\n"
      "\"stage\":\n"
      "[\n"
      "  { \n"
      "    \"type\": \"CameraActor\",\n"
      "    \"name\": \"camera\"\n"
      "  }, \n"
      "  { \n"
      "    \"type\": \"ImageView\",\n"
      "    \"name\": \"image\",\n"
      "    \"size\": [100,100,1],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touch\",\n"
      "      \"action\": \"quit\"\n"
      "    }],\n"
      "    \"actors\": [\n"
      "      {\n"
      "        \"type\":\"ImageView\",\n"
      "        \"name\":\"childImage\" \n"
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
      "    \"parentOrigin\": \"TOP_LEFT\",\n"
      "    \"anchorPoint\": \"TOP_LEFT\",\n"
      "    \"actors\": [{\n"
      "      \"type\": \"Actor\",\n"
      "      \"name\": \"subActor\"\n"
      "    }],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touch\",\n"
      "      \"action\": \"hide\",\n"
      "      \"actor\": \"actor\",\n"
      "      \"childActor\": \"subActor\"\n"
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
  Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
  touchEvent.points.push_back( point );
  application.ProcessEvent( touchEvent );

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName("subActor");
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
      "    \"parentOrigin\": \"TOP_LEFT\",\n"
      "    \"anchorPoint\": \"TOP_LEFT\",\n"
      "    \"actors\": [{\n"
      "      \"type\": \"Actor\",\n"
      "      \"name\": \"subActor\"\n"
      "    }],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touch\",\n"
      "      \"action\": \"set\",\n"
      "      \"actor\": \"subActor\",\n"
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
  Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
  touchEvent.points.push_back( point );
  application.ProcessEvent( touchEvent );

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName("subActor");
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
      "    \"parentOrigin\": \"TOP_LEFT\",\n"
      "    \"anchorPoint\": \"TOP_LEFT\",\n"
      "    \"actors\": [{\n"
      "      \"type\": \"Actor\",\n"
      "      \"name\": \"subActor\"\n"
      "    }],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touch\",\n"
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
  Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
  touchEvent.points.push_back( point );
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
      "    \"parentOrigin\": \"TOP_LEFT\",\n"
      "    \"anchorPoint\": \"TOP_LEFT\",\n"
      "    \"actors\": [{\n"
      "      \"type\": \"Actor\",\n"
      "      \"name\": \"subActor\"\n"
      "    }],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touch\",\n"
      "      \"action\": \"hide\"\n"
      "    }],\n"
      "    \"notifications\": [{\n"
      "      \"property\": \"visible\",\n"
      "      \"condition\": \"False\",\n"
      "      \"action\": \"show\"\n"
      "    },\n"
      "    {\n"
      "      \"property\": \"positionX\",\n"
      "      \"condition\": \"LessThan\",\n"
      "      \"arg0\": 0.0,\n"
      "      \"action\": \"show\"\n"
      "    },\n"
      "    {\n"
      "      \"property\": \"positionY\",\n"
      "      \"condition\": \"GreaterThan\",\n"
      "      \"arg0\": 200.0,\n"
      "      \"action\": \"show\"\n"
      "    },\n"
      "    {\n"
      "      \"property\": \"positionZ\",\n"
      "      \"condition\": \"Inside\",\n"
      "      \"arg0\": 0.0,\n"
      "      \"arg1\": 10.0,\n"
      "      \"action\": \"show\"\n"
      "    },\n"
      "    {\n"
      "      \"property\": \"positionZ\",\n"
      "      \"condition\": \"Outside\",\n"
      "      \"arg0\": 40.0,\n"
      "      \"arg1\": 50.0,\n"
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
  Integration::Point point;
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
  touchEvent.points.push_back( point );
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

int UtcDaliBuilderPropertyNotificationN(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "  \"stage\":\n"
      "  [{\n"
      "    \"type\": \"Actor\",\n"
      "    \"notifications\": [{\n"
      "      \"property\": \"visible\",\n"
      "      \"condition\": \"ErrorCondition\",\n"
      "      \"action\": \"show\"\n"
      "    }]\n"
      "  }]\n"
      "}\n"
  );

  try
  {
    Builder builder = Builder::New();
    builder.LoadFromString( json );
    builder.AddActors ( Stage::GetCurrent().GetRootLayer() );
    DALI_TEST_CHECK( false );
  }
  catch(...)
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}



int UtcDaliBuilderCustomPropertyP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "\"templates\":\n"
      "{\n"
      "  \"imageTree\": { \n"
      "    \"type\": \"ImageView\",\n"
      "    \"name\": \"image\",\n"
      "    \"size\": [100,100,1],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touch\",\n"
      "      \"action\": \"quit\"\n"
      "    }],\n"
      "    \"properties\": {\n"
      "      \"newproperty\": true\n"
      "    },\n"
      "    \"animatableProperties\": {\n"
      "      \"newAnimatableproperty\": 3\n"
      "    },\n"
      "    \"actors\": [\n"
      "      {\n"
      "        \"type\":\"ImageView\",\n"
      "        \"name\":\"childImage\" \n"
      "      }\n"
      "    ]\n"
      "  }\n"
      "}\n"
      "}\n"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );

  ImageView actor = ImageView::DownCast( builder.Create( "imageTree" ) );
  DALI_TEST_CHECK( actor );

  // NB: already applied in create
  Property::Index index = actor.GetPropertyIndex("newproperty");
  DALI_TEST_CHECK( Property::INVALID_INDEX != index );
  Property::Value value = actor.GetProperty(index);
  DALI_TEST_CHECK( value.Get<bool>() == true );

  index = actor.GetPropertyIndex("newAnimatableproperty");
  DALI_TEST_CHECK( Property::INVALID_INDEX != index );
  value = actor.GetProperty(index);
  DALI_TEST_CHECK( value.Get<int>() == 3 );

  END_TEST;
}

int UtcDaliBuilderCustomShaderP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
    "{\n"
    "  \"stage\": [\n"
    "    {\n"
    "      \"type\": \"ImageView\",\n"
    "      \"name\": \"Image1\",\n"
    "      \"position\": [\n"
    "        0.40461349487305,\n"
    "        0.9150390625,\n"
    "        0.0\n"
    "      ],\n"
    "      \"parentOrigin\": [0.5, 0.5, 0.5],\n"
    "      \"size\": [200, 200, 0],\n"
    "      \"effect\": \"Ripple2D\",\n"
    "      \"image\": {\n"
    "        \"url\": \"{DALI_IMAGE_DIR}gallery-medium-25.jpg\",\n"
    "        \"desiredWidth\": 200,\n"
    "        \"desiredHeight\": 80,\n"
    "        \"shader\": {\n"
    "           \"fragmentShader\": \"precision mediump float;\\nuniform sampler2D sTexture;\\nuniform vec4 uColor;\\nuniform float uAmplitude;\\nuniform float uTime;\\nvarying vec2 vTexCoord;\\nvoid main()\\n{\\n  highp vec2 pos = -1.0 + 2.0 * vTexCoord;\\n  highp float len = length(pos);\\n  highp vec2 texCoord = vTexCoord + pos/len * sin( len * 12.0 - uTime * 4.0 ) * uAmplitude;\\n  gl_FragColor = texture2D(sTexture, texCoord) * uColor;}\\n\\n\"\n"
    "        }\n"
    "      },\n"
    "      \"customAnimatableProperties\": {\n"
    "         \"uAmplitude\": 0.02,\n"
    "         \"uTime\": 0.0\n"
    "      },\n"
    "      \"signals\": [\n"
    "        {\n"
    "          \"name\": \"onStage\",\n"
    "          \"action\": \"play\",\n"
    "          \"animation\": \"Animation_1\"\n"
    "        }\n"
    "      ]\n"
    "    }\n"
    "  ],\n"
    "  \"animations\": {\n"
    "    \"Animation_1\": {\n"
    "      \"loop\":true,\n"
    "      \"properties\": [\n"
    "        {\n"
    "          \"actor\": \"Image1\",\n"
    "          \"property\": \"uTime\",\n"
    "          \"value\": 10.0,\n"
    "          \"alphaFunction\": \"LINEAR\",\n"
    "          \"timePeriod\": {\n"
    "            \"delay\": 0,\n"
    "            \"duration\": 10.0\n"
    "          }\n"
    "        }\n"
    "      ]\n"
    "    }\n"
    "  }\n"
    "}\n"

  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );

  builder.AddActors ( "stage", Stage::GetCurrent().GetRootLayer() );

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName("Image1");

  // coverage
  DALI_TEST_CHECK( actor );

  END_TEST;
}


int UtcDaliBuilderLoadFromStringN(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "asdfsadf dsf asdf asdf {"
         "\"stage\":"
         "[{"
           "\"type\": \"Actor\","
           "\"size\": [100,100,1],"
           "\"parentOrigin\": \"TOP_LEFT\","
           "\"anchorPoint\": \"TOP_LEFT\","
           "\"signals\": [{"
             "\"name\": \"touch\","
             "\"action\": \"quit\""
           "}]"
         "}]"
      "}"
  );
  Builder builder = Builder::New();

  bool assert1 = false;

  try
  {
    builder.LoadFromString( json );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_EQUALS(e.condition, "!\"Cannot parse JSON\"", TEST_LOCATION);
    assert1 = true;
  }

  DALI_TEST_CHECK( assert1 );

  END_TEST;
}


int UtcDaliBuilderAddActorsP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "  \"arbitarysection\":\n"
      "  [{\n"
      "    \"type\": \"Actor\",\n"
      "    \"name\": \"actor\",\n"
      "    \"size\": [100,100,1],\n"
      "    \"parentOrigin\": \"TOP_LEFT\",\n"
      "    \"anchorPoint\": \"TOP_LEFT\",\n"
      "    \"actors\": [{\n"
      "      \"type\": \"Actor\",\n"
      "      \"name\": \"subActor\",\n"
      "      \"visible\": false\n"
      "    }],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touch\",\n"
      "      \"action\": \"hide\",\n"
      "      \"actor\": \"actor\",\n"
      "      \"childActor\": \"subActor\"\n"
      "    }]\n"
      "  }]\n"
      "}\n"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors ( "arbitarysection", Stage::GetCurrent().GetRootLayer() );

  // Render and notify
  application.SendNotification();
  application.Render();

  Actor actor = Stage::GetCurrent().GetRootLayer().FindChildByName("subActor");
  DALI_TEST_CHECK( actor );

  DALI_TEST_CHECK( !actor.IsVisible() );

  END_TEST;
}

int UtcDaliBuilderFrameBufferP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
    "{\n"
    "  \"constants\":\n"
    "  {\n"
    "    \"FB_WIDTH\": 200.0,\n"
    "    \"FB_HEIGHT\": 200.0,\n"
    "    \"FB_SIZE\": [200,200],\n"
    "    \"FB_ASPECT_RATIO\": 1\n"
    "  },\n"
    "  \"stage\": [\n"
    "    {\n"
    "      \"type\": \"ImageView\",\n"
    "      \"name\": \"fbOnStage\",\n"
    "      \"position\": [\n"
    "        0.40461349487305,\n"
    "        0.9150390625,\n"
    "        0.0\n"
    "      ],\n"
    "      \"parentOrigin\": [0.5, 0.5, 0.5],\n"
    "      \"size\": [300, 300, 0],\n"
    "      \"image\": \"fb0\",\n"
    "      \"clearColor\": [1,0,0,1]\n"
    "    },\n"
    "    {\n"
    "      \"type\": \"ImageView\",\n"
    "      \"name\": \"Image1\",\n"
    "      \"size\": [200, 200, 0],\n"
    "      \"parentOrigin\": [0.5, 0.5, 0.5],\n"
    "      \"effect\": \"Ripple2D\",\n"
    "      \"image\": {\n"
    "        \"url\": \"{DALI_IMAGE_DIR}gallery-medium-25.jpg\"\n"
    "      },\n"
    "      \"signals\": [\n"
    "        {\n"
    "          \"name\": \"onStage\",\n"
    "          \"action\": \"play\",\n"
    "          \"animation\": \"Animation_1\"\n"
    "        }\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"type\":\"CameraActor\",\n"
    "      \"name\":\"fbCam\",\n"
    "      \"aspectRatio\": \"{FB_ASPECT_RATIO}\",\n"
    "      \"projectionMode\": \"PERSPECTIVE_PROJECTION\",\n"
    "      \"fieldOfView\": 0.785,\n"
    "      \"invertYAxis\": true\n"
    "    }\n"
    "  ],\n"
    "  \"frameBufferImages\":\n"
    "  {\n"
    "    \"fb0\":\n"
    "    {\n"
    "      \"type\": \"FrameBufferImage\",\n"
    "      \"width\": { \"typeCast\":\"float\", \"value\":\"{FB_WIDTH}\" },\n"
    "      \"height\": { \"typeCast\":\"float\", \"value\":\"{FB_HEIGHT}\" }\n"
    "    }\n"
    "  },\n"
    "  \"renderTasks\":\n"
    "  {\n"
    "    \"stage\":\n"
    "    [\n"
    "      {\n"
    "        \"sourceActor\": \"fbOnStage\"\n"
    "      },\n"
    "      {\n"
    "        \"sourceActor\": \"Image1\",\n"
    "        \"targetFrameBuffer\": \"fb0\",\n"
    "        \"viewportSize\":\"{FB_SIZE}\",\n"
    "        \"cameraActor\":\"fbCam\"\n"
    "      }\n"
    "    ]\n"
    "  },\n"
    "  \"paths\": {},\n"
    "  \"animations\": {\n"
    "    \"Animation_1\": {\n"
    "      \"loop\":true,\n"
    "      \"properties\": [\n"
    "        {\n"
    "          \"actor\": \"Image1\",\n"
    "          \"property\": \"uTime\",\n"
    "          \"value\": 10.0,\n"
    "          \"alphaFunction\": \"LINEAR\",\n"
    "          \"timePeriod\": {\n"
    "            \"delay\": 0,\n"
    "            \"duration\": 10.0\n"
    "          },\n"
    "          \"gui-builder-timeline-color\": \"#8dc0da\"\n"
    "        }\n"
    "      ]\n"
    "    }\n"
    "  }\n"
    "}\n");

  Builder builder = Builder::New();

  // frame buffer coverage
  builder.LoadFromString( json );

  // Render and notify
  application.SendNotification();
  application.Render();

  Dali::FrameBufferImage frameBuffer = builder.GetFrameBufferImage( "fb0" );
  DALI_TEST_CHECK( frameBuffer );

  Dali::FrameBufferImage frameBuffer2 = builder.GetFrameBufferImage( "fb0" );
  DALI_TEST_CHECK( frameBuffer2 );
  DALI_TEST_CHECK( frameBuffer == frameBuffer2 );

  DALI_TEST_CHECK( true );

  END_TEST;
}

int UtcDaliBuilderPathConstraintsP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
    "{\n"
    "  \"constants\":\n"
    "  {\n"
    "    \"FB_WIDTH\": 200.0,\n"
    "    \"FB_HEIGHT\": 200.0,\n"
    "    \"FB_SIZE\": [200,200],\n"
    "    \"FB_ASPECT_RATIO\": 1\n"
    "  },\n"
    "  \"stage\": [\n"
    "    {\n"
    "      \"type\": \"ImageView\",\n"
    "      \"name\": \"Image1\",\n"
    "      \"size\": [200, 200, 0],\n"
    "      \"parentOrigin\": [0.5, 0.5, 0.5],\n"
    "      \"effect\": \"Ripple2D\",\n"
    "      \"image\": {\n"
    "        \"url\": \"{DALI_IMAGE_DIR}gallery-medium-25.jpg\"\n"
    "      },\n"
    "      \"signals\": [\n"
    "        {\n"
    "          \"name\": \"onStage\",\n"
    "          \"action\": \"play\",\n"
    "          \"animation\": \"pathAnimation\"\n"
    "        },\n"
    "        {\n"
    "          \"name\": \"onStage\",\n"
    "          \"action\": \"applyConstraint\",\n"
    "          \"constrainer\": \"constrainer0\",\n"
    "          \"properties\":\n"
    "          [\n"
    "            {\n"
    "              \"source\": \"Image1\",\n"
    "              \"sourceProperty\": \"positionX\",\n"
    "              \"target\": \"Image1\",\n"
    "              \"targetProperty\": \"colorRed\",\n"
    "              \"range\": [-300,300]\n"
    "            }\n"
    "          ]\n"
    "        },\n"
    "        {\n"
    "          \"name\": \"onStage\",\n"
    "          \"action\": \"applyConstraint\",\n"
    "          \"constrainer\": \"constrainer1\",\n"
    "          \"properties\":\n"
    "          [\n"
    "            {\n"
    "              \"source\": \"Image1\",\n"
    "              \"sourceProperty\": \"positionX\",\n"
    "              \"target\": \"Image1\",\n"
    "              \"targetProperty\": \"colorBlue\",\n"
    "              \"range\": [-300,300]\n"
    "            }\n"
    "          ]\n"
    "        },\n"
    "        {\n"
    "          \"name\": \"offStage\",\n"
    "          \"action\": \"removeConstraints\",\n"
    "          \"constrainer\": \"constrainer0\",\n"
    "          \"properties\":\n"
    "          [\n"
    "            {\n"
    "              \"source\": \"Image1\",\n"
    "              \"sourceProperty\": \"positionX\",\n"
    "              \"target\": \"Image1\",\n"
    "              \"targetProperty\": \"colorRed\",\n"
    "              \"range\": [-300,300]\n"
    "            }\n"
    "          ]\n"
    "        },\n"
    "        {\n"
    "          \"name\": \"offStage\",\n"
    "          \"action\": \"removeConstraints\",\n"
    "          \"constrainer\": \"constrainer1\",\n"
    "          \"properties\":\n"
    "          [\n"
    "            {\n"
    "              \"source\": \"Image1\",\n"
    "              \"sourceProperty\": \"positionX\",\n"
    "              \"target\": \"Image1\",\n"
    "              \"targetProperty\": \"colorBlue\",\n"
    "              \"range\": [-300,300]\n"
    "            }\n"
    "          ]\n"
    "        }\n"
    "      ]\n"
    "    }\n"
    "  ],\n"
    "  \"paths\":\n"
    "  {\n"
    "    \"path0\":\n"
    "    {\n"
    "      \"points\":[ [-150, -50, 0], [0.0,70.0,0.0], [190.0,-150.0,0.0] ],\n"
    "      \"curvature\":0.35\n"
    "    }\n"
    "  },\n"
    "  \"constrainers\":\n"
    "  {\n"
    "    \"constrainer0\":\n"
    "    {\n"
    "      \"type\": \"PathConstrainer\",\n"
    "      \"points\": [ [0, 0, 0], [0,0,0], [0,0,0] ],\n"
    "      \"controlPoints\": [ [0, 0, 0], [0,0,0], [0,0,0] ]\n"
    "    },\n"
    "    \"constrainer1\":\n"
    "    {\n"
    "      \"type\": \"LinearConstrainer\",\n"
    "      \"value\": [ 0, 0, 0 ]\n"
    "    }\n"
    "  },\n"
    "  \"animations\": {\n"
    "    \"pathAnimation\": {\n"
    "      \"duration\": 3.0,\n"
    "      \"properties\":\n"
    "      [{\n"
    "        \"actor\": \"Image1\",\n"
    "        \"path\":\"path0\",\n"
    "        \"forward\":[1,0,0],\n"
    "        \"alphaFunction\": \"EASE_IN_OUT\",\n"
    "        \"timePeriod\": {\n"
    "          \"delay\": 0,\n"
    "          \"duration\": 3\n"
    "        }\n"
    "      },\n"
    "       {\n"
    "         \"actor\": \"Image1\",\n"
    "         \"property\": \"uTime\",\n"
    "         \"value\": 10.0,\n"
    "         \"alphaFunction\": \"LINEAR\",\n"
    "         \"timePeriod\": {\n"
    "           \"delay\": 0,\n"
    "           \"duration\": 10.0\n"
    "         },\n"
    "         \"gui-builder-timeline-color\": \"#8dc0da\"\n"
    "       }]\n"
    "    },\n"
    "    \"Animation_1\": {\n"
    "      \"loop\":true,\n"
    "      \"properties\": [\n"
    "        {\n"
    "          \"actor\": \"Image1\",\n"
    "          \"property\": \"uTime\",\n"
    "          \"value\": 10.0,\n"
    "          \"alphaFunction\": \"LINEAR\",\n"
    "          \"timePeriod\": {\n"
    "            \"delay\": 0,\n"
    "            \"duration\": 10.0\n"
    "          },\n"
    "          \"gui-builder-timeline-color\": \"#8dc0da\"\n"
    "        }\n"
    "      ]\n"
    "    }\n"
    "  }\n"
    "}\n");

  Builder builder = Builder::New();

  // frame buffer coverage
  builder.LoadFromString( json );

  // Render and notify
  application.SendNotification();
  application.Render();

  Dali::Path path =  builder.GetPath( "path0" );
  DALI_TEST_CHECK( path );

  Dali::Path path2 =  builder.GetPath( "path0" );
  DALI_TEST_CHECK( path2 );
  DALI_TEST_CHECK( path == path2 );

  Dali::PathConstrainer constrainer0 = builder.GetPathConstrainer( "constrainer0" );
  DALI_TEST_CHECK( constrainer0 );

  Dali::PathConstrainer constrainer0_2 = builder.GetPathConstrainer( "constrainer0" );
  DALI_TEST_CHECK( constrainer0_2 );
  DALI_TEST_CHECK( constrainer0 == constrainer0_2 );

  Dali::LinearConstrainer constrainer1 = builder.GetLinearConstrainer( "constrainer1" );
  DALI_TEST_CHECK( constrainer1 );

  Dali::LinearConstrainer constrainer1_2 = builder.GetLinearConstrainer( "constrainer1" );
  DALI_TEST_CHECK( constrainer1 == constrainer1_2 );

  // For coverage

  Actor actor = Actor::New();
  Stage::GetCurrent().Add( actor );
  builder.AddActors( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  actor.GetChildAt( 0 ).Unparent();

  END_TEST;
}

#define CHECK_MAP_ELEMENT( xMap, xKey, xType, xPropType, xExpected, xLocation ) \
  {                                                                       \
    Property::Value* value = xMap->Find( xKey );                          \
    DALI_TEST_EQUALS( value==NULL, false, xLocation);                     \
    if( value != NULL )                                                   \
    {                                                                     \
      DALI_TEST_EQUALS( value->GetType(), xPropType, xLocation );         \
      xType result;                                                       \
      value->Get(result);                                                 \
      DALI_TEST_EQUALS( result, xExpected, TEST_LOCATION );               \
      std::ostringstream oss;                                             \
      oss << "Animation element " << xKey << "= " << result << std::endl; \
      tet_printf( oss.str().c_str() );                                    \
    }                                                                     \
    else                                                                  \
    {                                                                     \
      tet_printf("Can't find map element " xKey "\n");                    \
    }                                                                     \
  }


int UtcDaliBuilderMapping01(void)
{
  ToolkitTestApplication application;

  const char* json =
    "{\n"
    "  \"mappings\":\n"
    "  {\n"
    "    \"buttonPressFadeOut\":{\n"
    "      \"alphaFunction\":\"EASE_OUT\",\n"
    "      \"timePeriod\":{\n"
    "        \"delay\":0.0,\n"
    "        \"duration\":0.4\n"
    "      }\n"
    "    },\n"
    "    \"buttonPressFadeIn\":{\n"
    "      \"alphaFunction\":\"EASE_IN\",\n"
    "      \"timePeriod\":{\n"
    "        \"delay\":0.4,\n"
    "        \"duration\":0.5\n"
    "      }\n"
    "    },\n"
    "    \"transition:buttonPressed\":\n"
    "    [\n"
    "      {\n"
    "        \"target\": \"unselectedBackgroundRenderer\",\n"
    "        \"property\": \"opacity\",\n"
    "        \"value\": 0,\n"
    "        \"animator\":\"<buttonPressFadeOut>\"\n"
    "      }\n"
    "    ],\n"
    "    \"transition:buttonReleased\":\n"
    "    [\n"
    "      {\n"
    "        \"target\": \"unselectedBackgroundRenderer\",\n"
    "        \"property\": \"opacity\",\n"
    "        \"value\": 1,\n"
    "        \"animator\":\"<buttonPressFadeIn>\"\n"
    "      },\n"
    "      {\n"
    "        \"target\": \"unselectedForegroundRenderer\",\n"
    "        \"property\": \"scale\",\n"
    "        \"value\": [ 1, 1, 1 ],\n"
    "        \"animator\":\"<buttonPressFadeIn>\"\n"
    "      },\n"
    "      {\n"
    "        \"target\": \"selectedBackgroundRenderer\",\n"
    "        \"property\": \"opacity\",\n"
    "        \"value\": 0,\n"
    "        \"animator\": \"<buttonPressFadeOut>\"\n"
    "      },\n"
    "      {\n"
    "        \"target\": \"selectedForegroundRenderer\",\n"
    "        \"property\": \"scale\",\n"
    "        \"value\": [ 0, 0, 0 ],\n"
    "        \"animator\":\"<buttonPressFadeOut>\"\n"
    "      }\n"
    "    ]\n"
    "  },\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"pressTransition\":\"<transition:buttonPressed>\",\n"
    "      \"releaseTransition\":\"<transition:buttonReleased>\"\n"
    "    }\n"
    "  }\n"
    "}\n";

  Builder builder = Builder::New();
  builder.LoadFromString( json );

  Test::TestButton testButton = Test::TestButton::New();
  Stage::GetCurrent().Add( testButton );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( builder.ApplyStyle( "testbutton", testButton ) );

  // Now check that it has loaded the transition correctly:
  Property::Value transition = testButton.GetProperty(Test::TestButton::Property::PRESS_TRANSITION);
  DALI_TEST_EQUALS( transition.GetType(), Property::ARRAY, TEST_LOCATION );
  Property::Array* array = transition.GetArray();

  DALI_TEST_EQUALS( array->Size(), 1, TEST_LOCATION );
  Property::Value element = array->GetElementAt(0);
  DALI_TEST_CHECK( element.GetType() == Property::MAP );
  Property::Map* map = element.GetMap();

  CHECK_MAP_ELEMENT(map, "target", std::string, Property::STRING, "unselectedBackgroundRenderer", TEST_LOCATION);
  CHECK_MAP_ELEMENT(map, "property", std::string, Property::STRING, "opacity", TEST_LOCATION);
  CHECK_MAP_ELEMENT(map, "alphaFunction", int, Property::INTEGER, (int)Dali::AlphaFunction::EASE_OUT, TEST_LOCATION);
  CHECK_MAP_ELEMENT(map, "timePeriodDelay", float, Property::FLOAT, 0.0f, TEST_LOCATION);
  CHECK_MAP_ELEMENT(map, "timePeriodDuration", float, Property::FLOAT, 0.4f, TEST_LOCATION);

  END_TEST;
}


int UtcDaliBuilderMappingCycleCheck(void)
{
  ToolkitTestApplication application;

  std::string json(
    "{\n"
    "  \"mappings\":\n"
    "  {\n"
    "    \"cyclicKey1\":\"<cyclicKey1>\",\n"
    "    \"cyclicKey2\":\"<cyclicKey3>\",\n"
    "    \"cyclicKey3\":\"<cyclicKey2>\",\n"
    "    \"FadeOut\":{\n"
    "      \"alphaFunction\":\"EASE_IN\",\n"
    "      \"timePeriod\":{\n"
    "        \"delay\":\"<cyclicKey3>\",\n"
    "        \"duration\":0.6\n"
    "      }\n"
    "    },\n"
    "    \"transition:buttonPressed\":\n"
    "    [\n"
    "      {\n"
    "        \"target\": \"<cyclicKey1>\",\n"
    "        \"property\": \"<cyclicKey2>\",\n"
    "        \"value\": 0,\n"
    "        \"animator\":\"<FadeOut>\"\n"
    "      }\n"
    "    ]\n"
    "  },\n"
    "  \"styles\":\n"
    "  {\n"
    "    \"testbutton\":\n"
    "    {\n"
    "      \"pressTransition\":\"<transition:buttonPressed>\",\n"
    "      \"releaseTransition\":\"<cyclicKey2>\",\n"
    "      \"disabledTransition\":\"<cyclicKey3>\",\n"
    "      \"enabledTransition\":\"<unknownKey>\"\n"
    "    }\n"
    "  }\n"
    "}\n");

  Builder builder = Builder::New();
  builder.LoadFromString( json );

  Test::TestButton testButton = Test::TestButton::New();
  Stage::GetCurrent().Add( testButton );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( builder.ApplyStyle( "testbutton", testButton ) );

  // Now check that it has loaded the transition correctly:
  Property::Value transition = testButton.GetProperty(Test::TestButton::Property::PRESS_TRANSITION);
  DALI_TEST_EQUALS( transition.GetType(), Property::ARRAY, TEST_LOCATION );
  Property::Array* array = transition.GetArray();

  DALI_TEST_EQUALS( array->Size(), 1, TEST_LOCATION );
  Property::Value element = array->GetElementAt(0);
  DALI_TEST_CHECK( element.GetType() == Property::MAP );
  Property::Map* map = element.GetMap();

  CHECK_MAP_ELEMENT(map, "target", std::string, Property::STRING, "", TEST_LOCATION);
  CHECK_MAP_ELEMENT(map, "property", std::string, Property::STRING, "", TEST_LOCATION);
  CHECK_MAP_ELEMENT(map, "timePeriodDuration", float, Property::FLOAT, 0.6f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliBuilderTypeCasts(void)
{
  ToolkitTestApplication application;

  std::string json(
    "{"
       "\"stage\":"
       "[{"
         "\"type\": \"Layer\","
         "\"maximumSize\": { \"typeCast\":\"vector2\", \"value\":[100,15] },"
         "\"position\":    { \"typeCast\":\"vector3\", \"value\":[100,10,1] },"
         "\"color\":       { \"typeCast\":\"vector4\", \"value\":[0.5,0.5,0.5,1] },"
         "\"sensitive\":   { \"typeCast\":\"boolean\", \"value\":false },"
         "\"orientation\": { \"typeCast\":\"rotation\", \"value\":[10,10,10,10] },"
         "\"colorMode\":   { \"typeCast\":\"string\", \"value\":\"USE_OWN_MULTIPLY_PARENT_COLOR\" },"
         "\"clippingBox\": { \"typeCast\":\"rect\", \"value\":[10,10,10,10] },"
         "\"padding\":     { \"typeCast\":\"extents\", \"value\":[10,10,10,10] }"
      "}]"
    "}"
  );

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors( rootActor );

  application.SendNotification();
  application.Render();

  Actor createdActor = rootActor.GetChildAt( 0 );
  DALI_TEST_EQUALS( createdActor.GetMaximumSize(), Vector2(100.0f,15.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( createdActor.GetCurrentPosition(), Vector3(100.0f,10.0f,1.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( createdActor.GetCurrentColor(), Vector4(0.5f,0.5f,0.5f,1.0f), TEST_LOCATION );
  DALI_TEST_EQUALS( createdActor.IsSensitive(), false, TEST_LOCATION );
  DALI_TEST_EQUALS( createdActor.GetColorMode(), USE_OWN_MULTIPLY_PARENT_COLOR, TEST_LOCATION );

  END_TEST;
}

int UtcDaliBuilderBuilderControl(void)
{
  ToolkitTestApplication application;

  std::string json(
    "{"
       "\"stage\":"
       "[{"
         "\"type\": \"BuilderControl\","
         "\"integerProperty\": 10,"
         "\"matrix3Property\": [ 1,2,3,4,5,6,7,8,9 ],"
         "\"matrixProperty\":  [ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 ],"
         "\"noneProperty\": 10"
      "}]"
    "}"
  );

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors( rootActor );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( BuilderControlProperty::gSetPropertyCalledCount, 4, TEST_LOCATION );

  END_TEST;
}

int UtcDaliBuilderCustomControl(void)
{
  ToolkitTestApplication application;

  std::string json(
    "{"
       "\"stage\":"
       "[{"
         "\"type\": \"DummyControl\","
         "\"name\": \"I can haz custom Control\""
      "}]"
    "}"
  );

  Actor rootActor = Actor::New();
  Stage::GetCurrent().Add( rootActor );

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors( rootActor );

  application.SendNotification();
  application.Render();

  Actor customControl = rootActor.FindChildByName( "I can haz custom Control" );

  // Test that we have the correct type of custom control
  DummyControl dummyControl = DummyControl::DownCast( customControl );
  DALI_TEST_CHECK( dummyControl );
  if( dummyControl )
  {
    DALI_TEST_CHECK( typeid(dummyControl.GetImplementation()) == typeid(DummyControlImpl) );
  }

  END_TEST;
}

int UtcDaliBuilderActionsWithParams(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "\"stage\":\n"
      "[\n"
      "  { \n"
      "    \"type\": \"ImageView\",\n"
      "    \"name\": \"image\",\n"
      "    \"size\": [100,100,1],\n"
      "    \"signals\": [{\n"
      "      \"name\": \"touch\",\n"
      "      \"action\": \"show\",\n"
      "      \"parameters\": {\n"
      "        \"property1\" : 10,\n"
      "        \"property2\" : [1,2],\n"
      "        \"property3\" : [1,2,3],\n"
      "        \"property4\" : [1,2,3,4]\n"
      "      }\n"
      "    }]\n"
      "  }\n"
      "]\n"
      "}\n"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );
  builder.AddActors( Stage::GetCurrent().GetRootLayer() );

  DALI_TEST_CHECK( true ); // For Coverage

  END_TEST;
}

int UtcDaliBuilderConfigurationP(void)
{
  ToolkitTestApplication application;

  // JSON with a quit event when the actor is touched
  std::string json(
      "{\n"
      "  \"config\":\n"
      "  {\n"
      "    \"alwaysShowFocus\":true\n"
      "  }\n"
      "}\n"
  );

  Builder builder = Builder::New();
  builder.LoadFromString( json );

  Property::Map map = builder.GetConfigurations();

  Dali::Property::Value* pValue = map.Find( "alwaysShowFocus" );

  DALI_TEST_CHECK( pValue );

  bool value = pValue->Get<bool>();

  DALI_TEST_CHECK( value );

  END_TEST;
}


int UtcDaliBase64EncodingP(void)
{
  std::vector<uint32_t> data = { 0, 1, 2, 3, 4, 5, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()  };

  Property::Value value;
  EncodeBase64PropertyData( value, data );

  std::cout << "Max uint32_t:" << std::numeric_limits<uint32_t>::max() << std::endl;
  std::cout << "Input data:  ";
  std::ostream_iterator<uint32_t> out_it (std::cout,", ");
  std::copy ( data.begin(), data.end(), out_it );
  std::cout << std::endl;

  std::string output;
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output, "AAAAAAEAAAACAAAAAwAAAAQAAAAFAAAAAAAAAP////8", TEST_LOCATION );

  std::cout << "Output data:  " << output << std::endl;

  END_TEST;
}

int UtcDaliBase64EncodingN(void)
{
  tet_infoline( "Test encoding an empty vector returns empty string" );
  std::vector<uint32_t> data;

  Property::Value value;
  EncodeBase64PropertyData( value, data );

  std::string output;
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output.empty(), true, TEST_LOCATION );

  END_TEST;
}

template <typename T>
int b64l(std::vector<T>&data)
{
  auto lengthInBytes = 4*data.size();
  return ceil( lengthInBytes * 1.33333f );
}

int UtcDaliBase64EncodingP02(void)
{
  tet_infoline( "Test encoding vectors of lengths m .. m+4 encode and decode back to the same length vectors" );

  std::vector<uint32_t> testData;
  for(int i=0; i<8; ++i ) // 8 chosen to stay within single string output
  {
    testData.push_back(i);
  }
  Property::Value value;
  EncodeBase64PropertyData( value, testData );

  std::string output;
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output.empty(), false, TEST_LOCATION);
  DALI_TEST_EQUALS( output.length(), b64l(testData), TEST_LOCATION );

  std::vector<uint32_t> outData;
  DecodeBase64PropertyData( value, outData );
  DALI_TEST_EQUALS( testData.size(), outData.size(), TEST_LOCATION );
  DALI_TEST_EQUALS( std::equal( testData.begin(), testData.end(), outData.begin()), true, TEST_LOCATION );

  // n+1
  testData.push_back( 12345 );
  EncodeBase64PropertyData( value, testData );

  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output.empty(), false, TEST_LOCATION);
  DALI_TEST_EQUALS( output.length(), b64l(testData), TEST_LOCATION );

  outData.clear();
  DecodeBase64PropertyData( value, outData );
  DALI_TEST_EQUALS( testData.size(), outData.size(), TEST_LOCATION );
  DALI_TEST_EQUALS( std::equal( testData.begin(), testData.end(), outData.begin()), true, TEST_LOCATION );

  // n+2
  testData.push_back( 67890 );
  EncodeBase64PropertyData( value, testData );

  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output.empty(), false, TEST_LOCATION);
  DALI_TEST_EQUALS( output.length(), b64l(testData), TEST_LOCATION );

  outData.clear();
  DecodeBase64PropertyData( value, outData );
  DALI_TEST_EQUALS( testData.size(), outData.size(), TEST_LOCATION );
  DALI_TEST_EQUALS( std::equal( testData.begin(), testData.end(), outData.begin()), true, TEST_LOCATION );

  // n+3
  testData.push_back( -1 );
  EncodeBase64PropertyData( value, testData );

  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output.empty(), false, TEST_LOCATION);
  DALI_TEST_EQUALS( output.length(), b64l(testData), TEST_LOCATION );

  outData.clear();
  DecodeBase64PropertyData( value, outData );
  DALI_TEST_EQUALS( testData.size(), outData.size(), TEST_LOCATION );
  DALI_TEST_EQUALS( std::equal( testData.begin(), testData.end(), outData.begin()), true, TEST_LOCATION );


  END_TEST;
}


int UtcDaliBase64EncodingP03(void)
{
  tet_infoline( "Test encoding a vector of length 12 has output within single string" );

  std::vector<uint32_t> testData;
  for(int i=0; i<12; ++i )
  {
    testData.push_back(i);
  }
  Property::Value value;
  EncodeBase64PropertyData( value, testData );

  std::string output;
  DALI_TEST_CHECK( value.Get( output ) );
  DALI_TEST_EQUALS( output.empty(), false, TEST_LOCATION);
  DALI_TEST_EQUALS( output.length(), b64l(testData), TEST_LOCATION );

  std::vector<uint32_t> outData;
  DecodeBase64PropertyData( value, outData );
  DALI_TEST_EQUALS( testData.size(), outData.size(), TEST_LOCATION );

  END_TEST;
}


int UtcDaliBase64EncodingP04(void)
{
  tet_infoline( "Test encoding a vector of length 13 has output split over 2 strings" );

  std::vector<uint32_t> testData;
  for(int i=0; i<13; ++i )
  {
    testData.push_back(i);
  }
  Property::Value value;
  EncodeBase64PropertyData( value, testData );

  auto array = value.GetArray();
  DALI_TEST_CHECK( array );

  DALI_TEST_EQUALS( array->Count(), 2, TEST_LOCATION );

  std::vector<uint32_t> outData;
  DecodeBase64PropertyData( value, outData );
  DALI_TEST_EQUALS( testData.size(), outData.size(), TEST_LOCATION );

  END_TEST;
}


int UtcDaliBase64EncodingP05(void)
{
  tet_infoline( "Test encoding a vector of length 24 has output split over 2 strings" );

  std::vector<uint32_t> testData;
  for(int i=0; i<24; ++i )
  {
    testData.push_back(i);
  }
  Property::Value value;
  EncodeBase64PropertyData( value, testData );

  auto array = value.GetArray();
  DALI_TEST_CHECK( array );

  DALI_TEST_EQUALS( array->Count(), 2, TEST_LOCATION );

  std::vector<uint32_t> outData;
  DecodeBase64PropertyData( value, outData );
  DALI_TEST_EQUALS( testData.size(), outData.size(), TEST_LOCATION );

  END_TEST;
}


int UtcDaliBase64EncodingP06(void)
{
  tet_infoline( "Test encoding a vector of arbitrary length decodes OK." );

  std::vector<uint32_t> testData;
  for(int i=0; i<97; ++i )
  {
    testData.push_back(i);
  }
  Property::Value value;
  EncodeBase64PropertyData( value, testData );

  auto array = value.GetArray();
  DALI_TEST_CHECK( array );

  std::vector<uint32_t> outData;
  DecodeBase64PropertyData( value, outData );
  DALI_TEST_EQUALS( testData.size(), outData.size(), TEST_LOCATION );

  END_TEST;
}


int UtcDaliBase64DecodingN01(void)
{
  tet_infoline( "Test decoding empty string results in empty data" );

  Property::Value value("");
  std::vector<uint32_t> outputData;
  DecodeBase64PropertyData( value, outputData);
  DALI_TEST_EQUALS( outputData.size(), 0, TEST_LOCATION );
  END_TEST;
}


int UtcDaliBase64DecodingN02(void)
{
  tet_infoline( "Test decoding array with non-string values results in empty data" );

  Property::Array array;
  array.Resize(2);
  array[0] = "Stuff, things";
  array[1] = 1;
  Property::Value value(array);

  std::vector<uint32_t> outputData;
  DecodeBase64PropertyData( value, outputData);
  DALI_TEST_EQUALS( outputData.size(), 0, TEST_LOCATION );
  END_TEST;
}

int UtcDaliBase64DecodingP01(void)
{
  tet_infoline( "Test decoding string of known data gives expected result");

  std::string testInput("//////7+/v4DAgEA");
  std::vector<uint32_t> expectedResults = { 0xffffffff, 0xfefefefe, 0x00010203 };

  std::vector<uint32_t> outputData;
  DecodeBase64PropertyData(Property::Value(testInput), outputData);

  DALI_TEST_EQUALS( std::equal( expectedResults.begin(), expectedResults.end(), outputData.begin() ), true,
                    TEST_LOCATION );

  END_TEST;
}
