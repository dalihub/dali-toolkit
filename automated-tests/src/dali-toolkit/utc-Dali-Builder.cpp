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
