/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include "dummy-control.h"

using namespace Dali;
using namespace Toolkit;


void utc_dali_toolkit_transition_data_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_transition_data_cleanup(void)
{
  test_return_value = TET_PASS;
}

Property::Map CreateMap()
{
  Property::Map map;

  map["target"] = "Actor1";
  map["property"] = "color";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = Color::RED;
  map["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_IN_OUT_BACK")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));
  return map;
}

void CHECK_ARRAY_EQUALS( Property::Array test, Property::Value result )
{
  if( result.GetType() == Property::ARRAY )
  {
    // Compare arrays
    Property::Array *resultArray = result.GetArray();
    DALI_TEST_EQUALS( test.Count(), resultArray->Count(), TEST_LOCATION );
    for( size_t i=0; i < std::min(test.Count(), resultArray->Count()); ++i )
    {
      Property::Value a = test.GetElementAt(i);
      Property::Value b = resultArray->GetElementAt(i);
      DALI_TEST_EQUALS( a.GetType(), b.GetType(), TEST_LOCATION );
      DALI_TEST_EQUALS( a, b, 0.001, TEST_LOCATION );
    }
  }
  else if( result.GetType() == Property::VECTOR4 )
  {
    Vector4 value = result.Get<Vector4>();
    DALI_TEST_CHECK( test.Count() >= 4 );
    for( size_t i=0; i < 4; ++i )
    {
      Property::Value a = test.GetElementAt(i);
      DALI_TEST_EQUALS( a.GetType(), Property::FLOAT, TEST_LOCATION );
      DALI_TEST_EQUALS( a.Get<float>(), value[i], 0.001, TEST_LOCATION );
    }
  }
  else
  {
    DALI_TEST_CHECK( 0 );
  }
}

void CHECK_MAP_EQUALS( Property::Map test, Property::Map result )
{
  DALI_TEST_EQUALS(test.Count(), result.Count(), TEST_LOCATION);

  for( unsigned int i=0; i< test.Count(); ++i )
  {
    KeyValuePair keyValue = test.GetKeyValue(i);
    Property::Value* value;

    if( keyValue.first.type == Property::Key::STRING )
    {
      value = result.Find(keyValue.first.stringKey);
    }
    else
    {
      value = result.Find(keyValue.first.indexKey);
    }

    DALI_TEST_CHECK( value != NULL );
    if( value != NULL )
    {
      if( keyValue.second.GetType() == Property::MAP )
      {
        DALI_TEST_EQUALS( keyValue.second.GetType(), value->GetType(), TEST_LOCATION );
        CHECK_MAP_EQUALS( *(keyValue.second.GetMap()), *(value->GetMap()) );
      }
      else if( keyValue.second.GetType() == Property::ARRAY )
      {
        CHECK_ARRAY_EQUALS( *(keyValue.second.GetArray()), *value );
      }
      else if( keyValue.second.GetType() == Property::STRING )
      {
        DALI_TEST_EQUALS( keyValue.second.GetType(), value->GetType(), TEST_LOCATION );
        std::string str;
        value->Get(str);
        DALI_TEST_EQUALS( keyValue.second, str.c_str(), TEST_LOCATION );
      }
      else
      {
        DALI_TEST_EQUALS( keyValue.second.GetType(), value->GetType(), TEST_LOCATION );
        DALI_TEST_EQUALS( keyValue.second, *value, 0.001f, TEST_LOCATION );
      }
    }
  }
}


int UtcDaliTransitionDataNew(void)
{
  TestApplication application;

  Property::Map map = CreateMap();
  Dali::Toolkit::TransitionData transition = TransitionData::New( map );
  DALI_TEST_CHECK( transition );

  END_TEST;
}

int UtcDaliTransitionDataDownCast(void)
{
  TestApplication application;

  Property::Map map = CreateMap();

  BaseHandle handle = TransitionData::New( map );
  DALI_TEST_CHECK( handle );

  TransitionData transitionData = TransitionData::DownCast( handle );
  DALI_TEST_CHECK( transitionData );
  END_TEST;
}

int UtcDaliTransitionDataCopyConstructor(void)
{
  TestApplication application;

  Property::Map map = CreateMap();

  TransitionData transitionData = TransitionData::New( map );
  DALI_TEST_CHECK( transitionData );

  TransitionData td2( transitionData );
  DALI_TEST_CHECK( td2 );
  DALI_TEST_EQUALS( td2.Count(), 1, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTransitionDataAssignmentOperator(void)
{
  TestApplication application;

  Property::Map map = CreateMap();

  TransitionData transitionData = TransitionData::New( map );
  DALI_TEST_CHECK( transitionData );

  TransitionData td2;
  DALI_TEST_CHECK( !td2 );

  td2 = transitionData;
  DALI_TEST_CHECK( td2 );

  DALI_TEST_EQUALS( td2.Count(), 1, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTransitionDataCount(void)
{
  TestApplication application;

  Property::Map map = CreateMap();
  TransitionData transitionData = TransitionData::New( map );
  DALI_TEST_CHECK( transitionData );
  DALI_TEST_EQUALS( transitionData.Count(), 1, TEST_LOCATION );

  Property::Array array;
  array.PushBack( map );
  array.PushBack( map );
  array.PushBack( map );

  TransitionData transitionData2 = TransitionData::New( array );
  DALI_TEST_CHECK( transitionData2 );
  DALI_TEST_EQUALS( transitionData2.Count(), 3, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTransitionDataMap1P(void)
{
  TestApplication application;

  tet_printf("Testing animation of a visual property using stylesheet equivalent maps\n");

  Property::Map map;
  map["target"] = "visual1";
  map["property"] = "mixColor";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = Color::RED;
  map["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_IN_OUT")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  DummyControl actor = DummyControl::New();
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  actor.SetName("Actor1");
  actor.SetColor(Color::CYAN);
  Stage::GetCurrent().Add(actor);

  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());

  Property::Map visualMap;
  visualMap[Visual::Property::TYPE] = Visual::COLOR;
  visualMap[ColorVisual::Property::MIX_COLOR] = Color::MAGENTA;
  Visual::Base visual = VisualFactory::Get().CreateVisual( visualMap );
  visual.SetName( "visual1" );

  Property::Index visualIndex = Control::CONTROL_PROPERTY_END_INDEX + 1;
  dummyImpl.RegisterVisual( visualIndex, visual );

  Animation anim = dummyImpl.CreateTransition( transition );
  DALI_TEST_CHECK( anim );

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index mixColorIndex = renderer.GetPropertyIndex( ColorVisual::Property::MIX_COLOR );
  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(mixColorIndex), Color::MAGENTA, TEST_LOCATION);

  anim.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(500); // Start animation
  application.Render(500); // Halfway thru anim
  application.SendNotification();
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(mixColorIndex), (Color::MAGENTA+Color::RED)*0.5f, TEST_LOCATION);

  application.Render(500); // End of anim
  application.SendNotification();
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(mixColorIndex), Color::RED, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTransitionDataMap2P(void)
{
  TestApplication application;

  tet_printf("Testing animation of a visual property using programmatic maps\n");

  Property::Map map;
  map["target"] = "visual1";
  //Control::CONTROL_PROPERTY_END_INDEX + 1
  map["property"] = ColorVisual::Property::MIX_COLOR;
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = Color::RED;
  map["animator"] = Property::Map()
    .Add("alphaFunction", "LINEAR")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  DummyControl actor = DummyControl::New();
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  actor.SetName("Actor1");
  actor.SetColor(Color::CYAN);
  Stage::GetCurrent().Add(actor);

  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());

  Property::Map visualMap;
  visualMap[Visual::Property::TYPE] = Visual::COLOR;
  visualMap[ColorVisual::Property::MIX_COLOR] = Color::MAGENTA;
  Visual::Base visual = VisualFactory::Get().CreateVisual( visualMap );
  visual.SetName( "visual1" );

  Property::Index visualIndex = Control::CONTROL_PROPERTY_END_INDEX + 1;
  dummyImpl.RegisterVisual( visualIndex, visual );

  Animation anim = dummyImpl.CreateTransition( transition );
  DALI_TEST_CHECK( anim );

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index mixColorIndex = renderer.GetPropertyIndex( ColorVisual::Property::MIX_COLOR );
  application.SendNotification();
  application.Render(0);

  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(mixColorIndex), Color::MAGENTA, TEST_LOCATION);

  anim.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(500); // Start animation
  application.Render(500); // Halfway thru anim
  application.SendNotification();
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(mixColorIndex), (Color::MAGENTA+Color::RED)*0.5f, TEST_LOCATION);

  application.Render(500); // End of anim
  application.SendNotification();
  DALI_TEST_EQUALS( renderer.GetProperty<Vector4>(mixColorIndex), Color::RED, TEST_LOCATION );

  END_TEST;
}


int UtcDaliTransitionDataMap3P(void)
{
  TestApplication application;

  tet_printf("Testing animation of a visual's placement actor property\n");

  Property::Map map;
  map["target"] = "visual1";
  map["property"] = "color";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = Color::RED;
  map["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_IN_OUT")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  DummyControl actor = DummyControl::New();
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  actor.SetName("Actor1");
  actor.SetColor(Color::CYAN);
  Stage::GetCurrent().Add(actor);

  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());

  Property::Map visualMap;
  visualMap[Visual::Property::TYPE] = Visual::COLOR;
  visualMap[ColorVisual::Property::MIX_COLOR] = Color::MAGENTA;
  Visual::Base visual = VisualFactory::Get().CreateVisual( visualMap );
  visual.SetName( "visual1" );

  Property::Index visualIndex = Control::CONTROL_PROPERTY_END_INDEX + 1;
  dummyImpl.RegisterVisual( visualIndex, visual );

  Animation anim = dummyImpl.CreateTransition( transition );
  DALI_TEST_CHECK( anim );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::MAGENTA, TEST_LOCATION);

  anim.Play();

  application.SendNotification();
  application.Render(0);
  application.Render(500);
  application.Render(500); // Halfway thru map1 anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), (Color::MAGENTA+Color::RED)*0.5f, TEST_LOCATION);

  application.Render(500); // End of map1 anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::RED, TEST_LOCATION );
  END_TEST;
}


int UtcDaliTransitionDataMap4P(void)
{
  TestApplication application;

  tet_printf("Testing animation of a visual's placement actor property using bezier curve\n");

  Property::Map map;
  map["target"] = "Actor1";
  map["property"] = "position";
  map["initialValue"] = Vector3(0, 0, 0);
  map["targetValue"] = Vector3(100, 100, 0);
  map["animator"] = Property::Map()
    .Add("alphaFunction", Vector4(0.71, -0.57, 0.42, 1.38) )
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.0f)
         .Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  DummyControl actor = DummyControl::New();
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  actor.SetName("Actor1");
  Stage::GetCurrent().Add(actor);

  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  Animation anim = dummyImpl.CreateTransition( transition );
  DALI_TEST_CHECK( anim );

  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(0,0,0), 0.001f, TEST_LOCATION);

  anim.Play();

  application.SendNotification();
  application.Render(0);

  application.Render(250); // 25%
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(-10,-10,0), 1.0, TEST_LOCATION); // High epsilon as we don't have exact figure for bezier curve at 50%

  application.Render(250); // Halfway thru map1 anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(24,24,0), 1.0, TEST_LOCATION); // High epsilon as we don't have exact figure for bezier curve at 50%

  application.Render(250); // End of map1 anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(100,100,0), 1.0, TEST_LOCATION); // High epsilon as we don't have exact figure for bezier curve

  application.Render(250); // End of map1 anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(100,100,0), TEST_LOCATION );
  END_TEST;
}

int UtcDaliTransitionDataMap1N(void)
{
  TestApplication application;

  Property::Map map;
  map["target"] = "Actor1";
  map["property"] = "randomProperty";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = Color::RED;
  map["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_OUT")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  DummyControl actor = DummyControl::New();
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  actor.SetName("Actor1");
  actor.SetColor(Color::CYAN);
  Stage::GetCurrent().Add(actor);

  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  Animation anim = dummyImpl.CreateTransition( transition );
  DALI_TEST_CHECK( ! anim );

  CHECK_MAP_EQUALS( map, transition.GetAnimatorAt(0) );
  END_TEST;
}


int UtcDaliTransitionDataMapN3(void)
{
  TestApplication application;

  tet_printf("Testing visual lookup with no renderers\n");

  Property::Map map;
  map["target"] = "visual1";
  map["property"] = "mixColor";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = Color::RED;
  map["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_OUT_BACK")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );
  CHECK_MAP_EQUALS( map, transition.GetAnimatorAt(0) );

  DummyControl actor = DummyControl::New();
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  actor.SetName("Actor1");
  actor.SetColor(Color::CYAN);
  // Don't stage actor

  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  Property::Map visualMap;
  visualMap[Visual::Property::TYPE] = Visual::COLOR;
  visualMap[ColorVisual::Property::MIX_COLOR] = Color::MAGENTA;
  Visual::Base visual = VisualFactory::Get().CreateVisual( visualMap );
  visual.SetName( "visual1" );

  Property::Index visualIndex = Control::CONTROL_PROPERTY_END_INDEX + 1;
  dummyImpl.RegisterVisual( visualIndex, visual );

  Animation anim = dummyImpl.CreateTransition( transition );
  DALI_TEST_CHECK( !anim );
  END_TEST;
}


int UtcDaliTransitionDataMapN4(void)
{
  TestApplication application;

  tet_printf("Testing visual doesn't animate with duff bezier data \n");

  Property::Map map;
  map["target"] = "visual1";
  map["property"] = "mixColor";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = Color::RED;
  map["animator"] = Property::Map()
    .Add("alphaFunction", Vector3(.1f,1.0f,0.5f))
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  DummyControl actor = DummyControl::New();
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  actor.SetName("Actor1");
  actor.SetColor(Color::CYAN);
  Stage::GetCurrent().Add(actor);

  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  Property::Map visualMap;
  visualMap[Visual::Property::TYPE] = Visual::COLOR;
  visualMap[ColorVisual::Property::MIX_COLOR] = Color::MAGENTA;
  Visual::Base visual = VisualFactory::Get().CreateVisual( visualMap );
  visual.SetName( "visual1" );

  Property::Index visualIndex = Control::CONTROL_PROPERTY_END_INDEX + 1;
  dummyImpl.RegisterVisual( visualIndex, visual );

  Animation anim = dummyImpl.CreateTransition( transition );
  DALI_TEST_CHECK( !anim );

  application.SendNotification();
  application.Render(0);
  application.SendNotification();

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index mixColorIdx = renderer.GetPropertyIndex(ColorVisual::Property::MIX_COLOR);

  tet_printf( "Test that the property has been set to target value\n");
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(mixColorIdx), Color::RED, 0.001, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionDataMapN5(void)
{
  TestApplication application;

  tet_printf("Testing visual doesn't animate with duff bezier data \n");

  Property::Map map;
  map["target"] = "visual1";
  map["property"] = "mixColor";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = Color::RED;
  map["animator"] = Property::Map()
    .Add("alphaFunction", Property::Array().Add(.1f).Add(1.0f).Add(0.5f))
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  DummyControl actor = DummyControl::New();
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  actor.SetName("Actor1");
  actor.SetColor(Color::CYAN);
  Stage::GetCurrent().Add(actor);

  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  Property::Map visualMap;
  visualMap[Visual::Property::TYPE] = Visual::COLOR;
  visualMap[ColorVisual::Property::MIX_COLOR] = Color::MAGENTA;
  Visual::Base visual = VisualFactory::Get().CreateVisual( visualMap );
  visual.SetName( "visual1" );

  Property::Index visualIndex = Control::CONTROL_PROPERTY_END_INDEX + 1;
  dummyImpl.RegisterVisual( visualIndex, visual );

  Animation anim = dummyImpl.CreateTransition( transition );
  DALI_TEST_CHECK( !anim );

  application.SendNotification();
  application.Render(0);
  application.SendNotification();

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index mixColorIdx = renderer.GetPropertyIndex(ColorVisual::Property::MIX_COLOR);

  tet_printf( "Test that the property has been set to target value\n");
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(mixColorIdx), Color::RED, 0.001, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTransitionDataMapN6(void)
{
  TestApplication application;

  tet_printf("Testing visual doesn't animate with duff bezier data \n");

  Property::Map map;
  map["target"] = "visual1";
  map["property"] = "mixColor";
  map["initialValue"] = Color::MAGENTA;
  map["targetValue"] = Color::RED;
  map["animator"] = Property::Map()
    .Add("alphaFunction", Property::Array().Add("1").Add("Two").Add("3").Add("4"))
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));

  Dali::Toolkit::TransitionData transition = TransitionData::New( map );

  DummyControl actor = DummyControl::New();
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  actor.SetName("Actor1");
  actor.SetColor(Color::CYAN);
  Stage::GetCurrent().Add(actor);

  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  Property::Map visualMap;
  visualMap[Visual::Property::TYPE] = Visual::COLOR;
  visualMap[ColorVisual::Property::MIX_COLOR] = Color::MAGENTA;
  Visual::Base visual = VisualFactory::Get().CreateVisual( visualMap );
  visual.SetName( "visual1" );

  Property::Index visualIndex = Control::CONTROL_PROPERTY_END_INDEX + 1;
  dummyImpl.RegisterVisual( visualIndex, visual );

  Animation anim = dummyImpl.CreateTransition( transition );
  DALI_TEST_CHECK( !anim );

  application.SendNotification();
  application.Render(0);
  application.SendNotification();

  Renderer renderer = actor.GetRendererAt(0);
  Property::Index mixColorIdx = renderer.GetPropertyIndex(ColorVisual::Property::MIX_COLOR);

  tet_printf( "Test that the property has been set to target value\n");
  DALI_TEST_EQUALS(renderer.GetProperty<Vector4>(mixColorIdx), Color::RED, 0.001, TEST_LOCATION);

  END_TEST;
}


int UtcDaliTransitionDataArrayP(void)
{
  TestApplication application;

  Property::Map map1;
  map1["target"] = "Actor1";
  map1["property"] = "color";
  map1["initialValue"] = Color::MAGENTA;
  map1["targetValue"] = Color::RED;
  map1["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_IN_OUT")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 1.0f));

  Property::Map map2;
  map2["target"] = "Actor1";
  map2["property"] = "position";
  map2["initialValue"] = Vector3(100,0,0);
  map2["targetValue"] = Vector3(0,100,0);
  map2["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_IN_OUT")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.0f)
         .Add("duration", 1.0f));

  Property::Map map3;
  map3["target"] = "Actor1";
  map3["property"] = "orientation";
  map3["targetValue"] = Quaternion( Radian(Math::PI_2), Vector3::ZAXIS );

  Property::Array array;
  array.PushBack(map1);
  array.PushBack(map2);
  array.PushBack(map3);

  Dali::Toolkit::TransitionData transition = TransitionData::New( array );

  DummyControl actor = DummyControl::New();
  actor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
  actor.SetName("Actor1");
  actor.SetColor(Color::CYAN);
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(Radian(0), Vector3::ZAXIS), TEST_LOCATION);

  DummyControlImpl& dummyImpl = static_cast<DummyControlImpl&>(actor.GetImplementation());
  Animation anim = dummyImpl.CreateTransition( transition );
  DALI_TEST_CHECK( anim );
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_EQUALS( actor.GetCurrentOrientation(), Quaternion(Radian(Math::PI_2), Vector3::ZAXIS), TEST_LOCATION);
  anim.Play();

  application.SendNotification();
  application.Render(0);   // start map2 anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(100,0,0), TEST_LOCATION);

  application.Render(500); // Start map1 animation, halfway thru map2 anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(50,50,0), TEST_LOCATION);

  application.Render(500); // Halfway thru map1 anim, end of map2 anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentPosition(), Vector3(0,100,0), TEST_LOCATION);
  DALI_TEST_EQUALS( actor.GetCurrentColor(), (Color::MAGENTA+Color::RED)*0.5f, TEST_LOCATION);

  application.Render(500); // End of map1 anim
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentColor(), Color::RED, TEST_LOCATION );

  END_TEST;
}


int UtcDaliTransitionDataGetAnimatorP(void)
{
  TestApplication application;

  Property::Map map1;
  map1["target"] = "Actor1";
  map1["property"] = "color";
  map1["initialValue"] = Color::MAGENTA;
  map1["targetValue"] = Color::RED;
  map1["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_IN_SQUARE")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 0.5f));

  Property::Map map2;
  map2["target"] = "Actor1";
  map2["property"] = "position";
  map2["initialValue"] = Vector3(100,0,0);
  map2["targetValue"] = Vector3(0,100,0);
  map2["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_OUT_SQUARE")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.2f)
         .Add("duration", 2.0f));

  Property::Map map3;
  map3["target"] = "Actor1";
  map3["property"] = "size";
  map3["initialValue"] = Vector2(10,10);
  map3["targetValue"] = Vector2(100,100);
  map3["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_OUT_SINE")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.4f)
         .Add("duration", 3.0f));

  Property::Map map4;
  map4["target"] = "Actor2";
  map4["property"] = "color";
  map4["initialValue"] = Color::BLACK;
  map4["targetValue"] = Color::GREEN;
  map4["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_IN_OUT_SINE")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.5f)
         .Add("duration", 0.5f));

  Property::Map map5;
  map5["target"] = "Actor2";
  map5["property"] = "position";
  map5["initialValue"] = Vector3(100,0,0);
  map5["targetValue"] = Vector3(0,100,0);
  map5["animator"] = Property::Map()
    .Add("alphaFunction", "BOUNCE")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.2f)
         .Add("duration", 2.0f));

  Property::Map map6;
  map6["target"] = "Actor2";
  map6["property"] = "size";
  map6["initialValue"] = Vector2(10,10);
  map6["targetValue"] = Vector2(100,100);
  map6["animator"] = Property::Map()
    .Add("alphaFunction", "SIN")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.4f)
         .Add("duration", 3.0f));

  Property::Map map7;
  map7["target"] = "Actor4";
  map7["property"] = "sizeModeFactor";
  map7["initialValue"] = Vector3(1,1,1);
  map7["targetValue"] = Vector3(2,2,2);
  map7["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_IN_SINE")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.0f)
         .Add("duration", 1.0f));

  Property::Map map8;
  map8["target"] = "Visual1";
  map8["property"] = "colorAlpha";
  map8["targetValue"] = 1.0f;
  map8["animator"] = Property::Map()
    .Add("alphaFunction", "EASE_IN")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.3f)
         .Add("duration", 9.0f));

  Property::Map map9;
  map9["target"] = "Actor2";
  map9["property"] = "scale";
  map9["initialValue"] = Vector3(0,0,0);
  map9["targetValue"] = Vector3(1,1,1);
  map9["animator"] = Property::Map()
    .Add("alphaFunction", "REVERSE")
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.0f)
         .Add("duration", 1.0f));

  Property::Map map10;
  map10["target"] = "Actor2";
  map10["property"] = "scale";
  map10["initialValue"] = Vector3(0,0,0);
  map10["targetValue"] = Vector3(1,1,1);
  map10["animator"] = Property::Map()
    .Add("alphaFunction", Vector4(.23,.4,.8,1.2))
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.0f)
         .Add("duration", 1.0f));

  Property::Map map11;
  map11["target"] = "Actor2";
  map11["property"] = "scale";
  map11["initialValue"] = Vector3(0,0,0);
  map11["targetValue"] = Vector3(1,1,1);
  map11["animator"] = Property::Map()
    .Add("alphaFunction", Property::Array().Add(.23f).Add(.4f).Add(.8f).Add(.2f))
    .Add("timePeriod", Property::Map()
         .Add("delay", 0.0f)
         .Add("duration", 1.0f));

  Property::Map map12;
  map12["target"] = "Actor1";
  map12["property"] = "orientation";
  map12["targetValue"] = Quaternion( Radian(Math::PI_2), Vector3::ZAXIS );

  Property::Array array;
  array.PushBack(map1);
  array.PushBack(map2);
  array.PushBack(map3);
  array.PushBack(map4);
  array.PushBack(map5);
  array.PushBack(map6);
  array.PushBack(map7);
  array.PushBack(map8);
  array.PushBack(map9);
  array.PushBack(map10);
  array.PushBack(map11);
  array.PushBack(map12);

  Dali::Toolkit::TransitionData transition = TransitionData::New( array );

  DALI_TEST_EQUALS( transition.Count(), array.Count(), TEST_LOCATION );

  for( unsigned int i=0; i < array.Count(); ++i )
  {
    Property::Map animatorMap = transition.GetAnimatorAt(i);
    Property::Value& value = array.GetElementAt(i);
    Property::Map* inputMap = value.GetMap();
    DALI_TEST_CHECK( inputMap );
    CHECK_MAP_EQUALS( *inputMap, animatorMap );
  }

  END_TEST;
}
