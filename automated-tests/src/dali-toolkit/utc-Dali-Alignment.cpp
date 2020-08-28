/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-toolkit/devel-api/controls/alignment/alignment.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

void utc_dali_toolkit_alignment_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_alignment_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
/// Compare an int (Or'd Alignment::Type) with an Alignment::Type value
void DALI_TEST_EQUALS( int value1, Alignment::Type value2, const char* location )
{
  ::DALI_TEST_EQUALS< Alignment::Type >( static_cast< Alignment::Type >( value1 ), value2, location );
}

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}
} // namespace


int UtcDaliAlignmentConstructorNegative(void)
{
  ToolkitTestApplication application;

  Alignment alignment;

  try
  {
    Alignment::Padding padding;
    alignment.SetPadding(padding);
    tet_result(TET_FAIL);
  }
  catch (DaliException& e)
  {
    DALI_TEST_ASSERT(e, "alignment", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliAlignmentConstructorPositive(void)
{
  ToolkitTestApplication application;

  Alignment alignment = Alignment::New();

  try
  {
    Alignment::Padding padding;
    alignment.SetPadding(padding);
    tet_result(TET_PASS);
  }
  catch (DaliException& exception)
  {
    tet_result(TET_FAIL);
  }

  Actor actor = alignment;
  alignment = Alignment::DownCast( actor );

  DALI_TEST_CHECK( alignment );
  END_TEST;
}

int UtcDaliAlignmentConstructorRegister(void)
{
  ToolkitTestApplication application;

  //To ensure the object is registered after creation
  ObjectRegistry registry = application.GetCore().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect(&TestCallback);
  {
    Alignment alignment = Alignment::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliAlignmentSetAlignmentTypePositiveOffStage(void)
{
  ToolkitTestApplication application;

  // Default, HORIZONTAL_CENTER, VERTICAL_CENTER - Ensure they do not change!
  {
    Alignment alignment = Alignment::New();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER));
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);
  }

  // HORIZONTAL_LEFT, VERTICAL_CENTER
  {
    Alignment alignment = Alignment::New();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::HORIZONTAL_LEFT);
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);
  }

  // HORIZONTAL_RIGHT, VERTICAL_CENTER
  {
    Alignment alignment = Alignment::New();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::HORIZONTAL_RIGHT);
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);
  }

  // HORIZONTAL_LEFT, VERTICAL_TOP
  {
    Alignment alignment = Alignment::New();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_LEFT | Alignment::VERTICAL_TOP));
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);
  }

  // HORIZONTAL_CENTER, VERTICAL_TOP
  {
    Alignment alignment = Alignment::New();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::VERTICAL_TOP);
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);
  }

  // HORIZONTAL_RIGHT, VERTICAL_TOP
  {
    Alignment alignment = Alignment::New();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_RIGHT | Alignment::VERTICAL_TOP));
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);
  }

  // HORIZONTAL_LEFT, VERTICAL_BOTTOM
  {
    Alignment alignment = Alignment::New();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_LEFT | Alignment::VERTICAL_BOTTOM));
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);
  }

  // HORIZONTAL_CENTER, VERTICAL_BOTTOM
  {
    Alignment alignment = Alignment::New();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::VERTICAL_BOTTOM);
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);
  }

  // HORIZONTAL_RIGHT, VERTICAL_BOTTOM
  {
    Alignment alignment = Alignment::New();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_RIGHT | Alignment::VERTICAL_BOTTOM));
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);
  }
  END_TEST;
}

int UtcDaliAlignmentSetAlignmentTypePositiveOnStage(void)
{
  ToolkitTestApplication application;

  // Default, HORIZONTAL_CENTER, VERTICAL_CENTER - Ensure they do not change!
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER));
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_LEFT, VERTICAL_CENTER
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::HORIZONTAL_LEFT);
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_RIGHT, VERTICAL_CENTER
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::HORIZONTAL_RIGHT);
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_LEFT, VERTICAL_TOP
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_LEFT | Alignment::VERTICAL_TOP));
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_CENTER, VERTICAL_TOP
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::VERTICAL_TOP);
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_RIGHT, VERTICAL_TOP
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_RIGHT | Alignment::VERTICAL_TOP));
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_LEFT, VERTICAL_BOTTOM
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_LEFT | Alignment::VERTICAL_BOTTOM));
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_CENTER, VERTICAL_BOTTOM
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::VERTICAL_BOTTOM);
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_RIGHT, VERTICAL_BOTTOM
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    // Check default values
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);

    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_RIGHT | Alignment::VERTICAL_BOTTOM));
    alignment.SetAlignmentType(type);
    DALI_TEST_CHECK(alignment.GetAlignmentType() & type);

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }
  END_TEST;
}

int UtcDaliAlignmentSetAlignmentTypeNegative(void)
{
  ToolkitTestApplication application;

  // Setting HORIZONTAL_LEFT, HORIZONTAL_CENTER
  {
    Alignment alignment = Alignment::New();
    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_LEFT | Alignment::HORIZONTAL_CENTER));
    alignment.SetAlignmentType(type);
    // center will prevail in conflict
    DALI_TEST_CHECK( Alignment::HORIZONTAL_CENTER & alignment.GetAlignmentType() );
    DALI_TEST_CHECK( !(Alignment::HORIZONTAL_LEFT & alignment.GetAlignmentType()) );
  }

  // Setting HORIZONTAL_CENTER, HORIZONTAL_RIGHT
  {
    Alignment alignment = Alignment::New();
    Alignment::Type type(Alignment::Type(Alignment::HORIZONTAL_CENTER | Alignment::HORIZONTAL_RIGHT));

    alignment.SetAlignmentType(type);
    // center will prevail in conflict
    DALI_TEST_CHECK( Alignment::HORIZONTAL_CENTER & alignment.GetAlignmentType() );
    DALI_TEST_CHECK( !(Alignment::HORIZONTAL_RIGHT & alignment.GetAlignmentType()) );
  }

  // Setting VERTICAL_TOP, VERTICAL_CENTER
  {
    Alignment alignment = Alignment::New();
    Alignment::Type type(Alignment::Type(Alignment::VERTICAL_TOP | Alignment::VERTICAL_CENTER));
    alignment.SetAlignmentType(type);
    // center will prevail in conflict
    DALI_TEST_CHECK( Alignment::VERTICAL_CENTER & alignment.GetAlignmentType() );
    DALI_TEST_CHECK( !(Alignment::VERTICAL_TOP & alignment.GetAlignmentType()) );
  }

  // Setting VERTICAL_CENTER, VERTICAL_BOTTOM
  {
    Alignment alignment = Alignment::New();
    Alignment::Type type(Alignment::Type(Alignment::VERTICAL_TOP | Alignment::VERTICAL_BOTTOM));
    alignment.SetAlignmentType(type);
    // top will prevail in conflict
    DALI_TEST_CHECK( Alignment::VERTICAL_TOP & alignment.GetAlignmentType() );
    DALI_TEST_CHECK( !(Alignment::VERTICAL_BOTTOM & alignment.GetAlignmentType()) );
  }
  END_TEST;
}

int UtcDaliAlignmentGetAlignmentType(void)
{
  ToolkitTestApplication application;

  // Default, HorizonalCenter, VERTICAL_CENTER
  {
    Alignment alignment = Alignment::New();
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();
    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_LEFT, VERTICAL_CENTER
  {
    Alignment alignment = Alignment::New(Alignment::HORIZONTAL_LEFT);
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_LEFT | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();
    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_RIGHT, VERTICAL_CENTER
  {
    Alignment alignment = Alignment::New(Alignment::HORIZONTAL_RIGHT);
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_RIGHT | Alignment::VERTICAL_CENTER, alignment.GetAlignmentType(), TEST_LOCATION);
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();
    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_LEFT, VERTICAL_TOP
  {
    Alignment alignment = Alignment::New(Alignment::HORIZONTAL_LEFT, Alignment::VERTICAL_TOP);
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_LEFT | Alignment::VERTICAL_TOP, alignment.GetAlignmentType(), TEST_LOCATION);
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();
    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_CENTER, VERTICAL_TOP
  {
    Alignment alignment = Alignment::New(Alignment::HORIZONTAL_CENTER, Alignment::VERTICAL_TOP);
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_TOP, alignment.GetAlignmentType(), TEST_LOCATION);
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();
    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_RIGHT, VERTICAL_TOP
  {
    Alignment alignment = Alignment::New(Alignment::HORIZONTAL_RIGHT, Alignment::VERTICAL_TOP);
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_RIGHT | Alignment::VERTICAL_TOP, alignment.GetAlignmentType(), TEST_LOCATION);
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();
    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_LEFT, VERTICAL_BOTTOM
  {
    Alignment alignment = Alignment::New(Alignment::HORIZONTAL_LEFT, Alignment::VERTICAL_BOTTOM);
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_LEFT | Alignment::VERTICAL_BOTTOM, alignment.GetAlignmentType(), TEST_LOCATION);
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();
    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_CENTER, VERTICAL_BOTTOM
  {
    Alignment alignment = Alignment::New(Alignment::HORIZONTAL_CENTER, Alignment::VERTICAL_BOTTOM);
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_CENTER | Alignment::VERTICAL_BOTTOM, alignment.GetAlignmentType(), TEST_LOCATION);
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();
    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // HORIZONTAL_RIGHT, VERTICAL_BOTTOM
  {
    Alignment alignment = Alignment::New(Alignment::HORIZONTAL_RIGHT, Alignment::VERTICAL_BOTTOM);
    DALI_TEST_EQUALS(Alignment::HORIZONTAL_RIGHT | Alignment::VERTICAL_BOTTOM, alignment.GetAlignmentType(), TEST_LOCATION);
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();
    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }
  END_TEST;
}

int UtcDaliAlignmentSetScaling(void)
{
  ToolkitTestApplication application;

  // SCALE_TO_FILL
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(Alignment::SCALE_NONE, alignment.GetScaling(), TEST_LOCATION);
    alignment.SetScaling(Alignment::SCALE_TO_FILL);
    DALI_TEST_EQUALS(Alignment::SCALE_TO_FILL, alignment.GetScaling(), TEST_LOCATION);
    application.Render();
    application.SendNotification();

    // For complete line coverage
    alignment.SetAlignmentType(Alignment::HORIZONTAL_LEFT);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::HORIZONTAL_RIGHT);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::VERTICAL_TOP);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::VERTICAL_BOTTOM);
    application.Render();
    application.SendNotification();

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // SCALE_TO_FIT_KEEP_ASPECT
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(Alignment::SCALE_NONE, alignment.GetScaling(), TEST_LOCATION);
    alignment.SetScaling(Alignment::SCALE_TO_FIT_KEEP_ASPECT);
    DALI_TEST_EQUALS(Alignment::SCALE_TO_FIT_KEEP_ASPECT, alignment.GetScaling(), TEST_LOCATION);
    application.Render();
    application.SendNotification();

    // For complete line coverage
    alignment.SetAlignmentType(Alignment::HORIZONTAL_LEFT);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::HORIZONTAL_RIGHT);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::VERTICAL_TOP);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::VERTICAL_BOTTOM);
    application.Render();
    application.SendNotification();

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // SCALE_TO_FILL_KEEP_ASPECT
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(Alignment::SCALE_NONE, alignment.GetScaling(), TEST_LOCATION);
    alignment.SetScaling(Alignment::SCALE_TO_FILL_KEEP_ASPECT);
    DALI_TEST_EQUALS(Alignment::SCALE_TO_FILL_KEEP_ASPECT, alignment.GetScaling(), TEST_LOCATION);
    application.Render();
    application.SendNotification();

    // For complete line coverage
    alignment.SetAlignmentType(Alignment::HORIZONTAL_LEFT);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::HORIZONTAL_RIGHT);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::VERTICAL_TOP);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::VERTICAL_BOTTOM);
    application.Render();
    application.SendNotification();

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // SHRINK_TO_FIT
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(Alignment::SCALE_NONE, alignment.GetScaling(), TEST_LOCATION);
    alignment.SetScaling(Alignment::SHRINK_TO_FIT);
    DALI_TEST_EQUALS(Alignment::SHRINK_TO_FIT, alignment.GetScaling(), TEST_LOCATION);
    application.Render();
    application.SendNotification();

    // For complete line coverage
    alignment.SetAlignmentType(Alignment::HORIZONTAL_LEFT);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::HORIZONTAL_RIGHT);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::VERTICAL_TOP);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::VERTICAL_BOTTOM);
    application.Render();
    application.SendNotification();

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }

  // SHRINK_TO_FIT_KEEP_ASPECT
  {
    Alignment alignment = Alignment::New();
    alignment.Add(Actor::New());
    application.GetScene().Add(alignment);
    application.Render();
    application.SendNotification();

    DALI_TEST_EQUALS(Alignment::SCALE_NONE, alignment.GetScaling(), TEST_LOCATION);
    alignment.SetScaling(Alignment::SHRINK_TO_FIT_KEEP_ASPECT);
    DALI_TEST_EQUALS(Alignment::SHRINK_TO_FIT_KEEP_ASPECT, alignment.GetScaling(), TEST_LOCATION);
    application.Render();
    application.SendNotification();

    // For complete line coverage
    alignment.SetAlignmentType(Alignment::HORIZONTAL_LEFT);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::HORIZONTAL_RIGHT);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::VERTICAL_TOP);
    application.Render();
    application.SendNotification();
    alignment.SetAlignmentType(Alignment::VERTICAL_BOTTOM);
    application.Render();
    application.SendNotification();

    application.GetScene().Remove(alignment);
    application.Render();
    application.SendNotification();
  }
  END_TEST;
}

int UtcDaliAlignmentGetScaling(void)
{
  ToolkitTestApplication application;

  // SCALE_TO_FILL
  {
    Alignment alignment = Alignment::New();
    DALI_TEST_CHECK(alignment.GetScaling() == Alignment::SCALE_NONE);

    alignment.SetScaling(Alignment::SCALE_TO_FILL);
    DALI_TEST_CHECK(alignment.GetScaling() == Alignment::SCALE_TO_FILL);
  }

  // SCALE_TO_FIT_KEEP_ASPECT
  {
    Alignment alignment = Alignment::New();
    DALI_TEST_CHECK(alignment.GetScaling() == Alignment::SCALE_NONE);

    alignment.SetScaling(Alignment::SCALE_TO_FIT_KEEP_ASPECT);
    DALI_TEST_CHECK(alignment.GetScaling() == Alignment::SCALE_TO_FIT_KEEP_ASPECT);
  }

  // SCALE_TO_FILL_KEEP_ASPECT
  {
    Alignment alignment = Alignment::New();
    DALI_TEST_CHECK(alignment.GetScaling() == Alignment::SCALE_NONE);

    alignment.SetScaling(Alignment::SCALE_TO_FILL_KEEP_ASPECT);
    DALI_TEST_CHECK(alignment.GetScaling() == Alignment::SCALE_TO_FILL_KEEP_ASPECT);
  }

  // SHRINK_TO_FIT
  {
    Alignment alignment = Alignment::New();
    DALI_TEST_CHECK(alignment.GetScaling() == Alignment::SCALE_NONE);

    alignment.SetScaling(Alignment::SHRINK_TO_FIT);
    DALI_TEST_CHECK(alignment.GetScaling() == Alignment::SHRINK_TO_FIT);
  }

  // SHRINK_TO_FIT_KEEP_ASPECT
  {
    Alignment alignment = Alignment::New();
    DALI_TEST_CHECK(alignment.GetScaling() == Alignment::SCALE_NONE);

    alignment.SetScaling(Alignment::SHRINK_TO_FIT_KEEP_ASPECT);
    DALI_TEST_CHECK(alignment.GetScaling() == Alignment::SHRINK_TO_FIT_KEEP_ASPECT);
  }

  END_TEST;
}

int UtcDaliAlignmentSetPaddingPositive(void)
{
  ToolkitTestApplication application;

  Alignment alignment = Alignment::New();

  Alignment::Padding padding(1.0f, 1.5f, 2.f, 0.5f);
  DALI_TEST_CHECK( fabs( padding.left - alignment.GetPadding().left ) > GetRangedEpsilon( padding.left, alignment.GetPadding().left ) );
  DALI_TEST_CHECK( fabs( padding.right - alignment.GetPadding().right ) > GetRangedEpsilon( padding.right, alignment.GetPadding().right ) );
  DALI_TEST_CHECK( fabs( padding.top - alignment.GetPadding().top ) > GetRangedEpsilon( padding.top, alignment.GetPadding().top ) );
  DALI_TEST_CHECK( fabs( padding.bottom - alignment.GetPadding().bottom ) > GetRangedEpsilon( padding.bottom, alignment.GetPadding().bottom ) );

  alignment.SetPadding(padding);
  DALI_TEST_CHECK( fabs( padding.left - alignment.GetPadding().left ) < GetRangedEpsilon( padding.left, alignment.GetPadding().left ) );
  DALI_TEST_CHECK( fabs( padding.right - alignment.GetPadding().right ) < GetRangedEpsilon( padding.right, alignment.GetPadding().right ) );
  DALI_TEST_CHECK( fabs( padding.top - alignment.GetPadding().top ) < GetRangedEpsilon( padding.top, alignment.GetPadding().top ) );
  DALI_TEST_CHECK( fabs( padding.bottom - alignment.GetPadding().bottom ) < GetRangedEpsilon( padding.bottom, alignment.GetPadding().bottom ) );
  END_TEST;
}

int UtcDaliAlignmentSetPaddingNegative(void)
{
  ToolkitTestApplication application;

  Alignment alignment = Alignment::New();

  try
  {
    Alignment::Padding padding(-1.0f, 1.5f, 2.f, 0.f);
    alignment.SetPadding(padding);
    tet_result(TET_FAIL);
  }
  catch (DaliException& e)
  {
    DALI_TEST_ASSERT(e, "( padding.left >= 0.f ) && ( padding.top >= 0.f ) && ( padding.right >= 0.f ) && ( padding.bottom >= 0.f )", TEST_LOCATION );
  }

  try
  {
    Alignment::Padding padding(1.0f, 1.5f, -2.f, 0.f);
    alignment.SetPadding(padding);
    tet_result(TET_FAIL);
  }
  catch (DaliException& e)
  {
    DALI_TEST_ASSERT(e, "( padding.left >= 0.f ) && ( padding.top >= 0.f ) && ( padding.right >= 0.f ) && ( padding.bottom >= 0.f )", TEST_LOCATION );
  }

  try
  {
    Alignment::Padding padding(1.0f, 1.5f, 2.f, -1.f);
    alignment.SetPadding(padding);
    tet_result(TET_FAIL);
  }
  catch (DaliException& e)
  {
    DALI_TEST_ASSERT(e, "( padding.left >= 0.f ) && ( padding.top >= 0.f ) && ( padding.right >= 0.f ) && ( padding.bottom >= 0.f )", TEST_LOCATION );
  }

  try
  {
    Alignment::Padding padding(1.0f, -1.5f, 2.f, 0.f);
    alignment.SetPadding(padding);
    tet_result(TET_FAIL);
  }
  catch (DaliException& e)
  {
    DALI_TEST_ASSERT(e, "( padding.left >= 0.f ) && ( padding.top >= 0.f ) && ( padding.right >= 0.f ) && ( padding.bottom >= 0.f )", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliAlignmentGetPadding(void)
{
  ToolkitTestApplication application;

  Alignment alignment = Alignment::New();
  DALI_TEST_CHECK( fabs( alignment.GetPadding().left ) < GetRangedEpsilon( 0.f, alignment.GetPadding().left ) );
  DALI_TEST_CHECK( fabs( alignment.GetPadding().right ) < GetRangedEpsilon( 0.f, alignment.GetPadding().right ) );
  DALI_TEST_CHECK( fabs( alignment.GetPadding().top ) < GetRangedEpsilon( 0.f, alignment.GetPadding().top ) );
  DALI_TEST_CHECK( fabs( alignment.GetPadding().bottom ) < GetRangedEpsilon( 0.f, alignment.GetPadding().bottom ) );

  Alignment::Padding padding(1.0f, 1.5f, 2.f, 0.f);
  alignment.SetPadding(padding);
  DALI_TEST_CHECK( fabs( padding.left - alignment.GetPadding().left ) < GetRangedEpsilon( padding.left, alignment.GetPadding().left ) );
  DALI_TEST_CHECK( fabs( padding.right - alignment.GetPadding().right ) < GetRangedEpsilon( padding.right, alignment.GetPadding().right ) );
  DALI_TEST_CHECK( fabs( padding.top - alignment.GetPadding().top ) < GetRangedEpsilon( padding.top, alignment.GetPadding().top ) );
  DALI_TEST_CHECK( fabs( padding.bottom - alignment.GetPadding().bottom ) < GetRangedEpsilon( padding.bottom, alignment.GetPadding().bottom ) );
  END_TEST;
}

int UtcDaliAlignmentChildAddAndRemove(void)
{
  ToolkitTestApplication application;

  Alignment alignment = Alignment::New();
  application.GetScene().Add(alignment);

  application.Render();
  application.SendNotification();

  Actor actor = Actor::New();
  alignment.Add(actor);

  DALI_TEST_EQUALS(alignment.GetChildCount(), 1u, TEST_LOCATION);

  application.Render();
  application.SendNotification();

  alignment.Remove(actor);

  DALI_TEST_EQUALS(alignment.GetChildCount(), 0u, TEST_LOCATION);

  application.Render();
  application.SendNotification();

  application.GetScene().Remove(alignment);
  END_TEST;
}

int UtcDaliAlignmentSizeSetP(void)
{
  ToolkitTestApplication application;

  Alignment alignment = Alignment::New();
  application.GetScene().Add(alignment);

  application.Render();
  application.SendNotification();

  Vector2 size( 100.0f, 200.0f );
  alignment.SetProperty( Actor::Property::SIZE, size);

  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  DALI_TEST_EQUALS(size, alignment.GetTargetSize().GetVectorXY(), TEST_LOCATION);

  application.GetScene().Remove(alignment);
  END_TEST;
}

///////////////////////////////////////////////////////////////////////////////
static bool TouchCallback(Actor actor, const TouchEvent& event)
{
  return false;
}

///////////////////////////////////////////////////////////////////////////////

int UtcDaliAlignmentOnTouchEvent(void)
{
  ToolkitTestApplication application;

  Alignment alignment = Alignment::New();
  alignment.SetProperty( Actor::Property::SIZE, Vector2( 100.0f, 100.0f ) );
  alignment.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::TOP_LEFT);
  application.GetScene().Add(alignment);

  alignment.TouchSignal().Connect(&TouchCallback);

  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  Integration::TouchEvent touchEvent(1);
  Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN);
  point.SetScreenPosition( Vector2( 20.0f, 20.0f ) );
  touchEvent.AddPoint(point);
  application.ProcessEvent(touchEvent);

  tet_result(TET_PASS); // For line coverage, as long as there are no exceptions, we assume passed.
  END_TEST;
}

int UtcDaliAlignmentOnKeyEvent(void)
{
  ToolkitTestApplication application;

  Alignment alignment = Alignment::New();
  application.GetScene().Add(alignment);

  alignment.SetKeyInputFocus();

  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  Integration::KeyEvent keyEvent;
  application.ProcessEvent(keyEvent);

  tet_result(TET_PASS); // For line coverage, as long as there are no exceptions, we assume passed.
  END_TEST;
}

int UtcDaliAlignmentOnSizeAnimation(void)
{
  ToolkitTestApplication application;

  Alignment alignment = Alignment::New();
  application.GetScene().Add(alignment);

  Animation animation = Animation::New(100.0f);
  animation.AnimateTo( Property( alignment, Actor::Property::SIZE ), Vector3( 100.0f, 150.0f, 200.0f ) );
  animation.Play();

  application.Render();
  application.SendNotification();
  application.Render();
  application.SendNotification();

  tet_result(TET_PASS); // For line coverage, as long as there are no exceptions, we assume passed.
  END_TEST;
}

int UtcDaliAlignmentCopyAndAssignment(void)
{
  ToolkitTestApplication application;

  Alignment alignment = Alignment::New();
  Alignment emptyAlignment;

  Alignment::Padding padding(100.0f, 150.0f, 200.f, 0.f);
  alignment.SetPadding(padding);

  Alignment alignmentCopy(alignment);
  DALI_TEST_CHECK( fabs( padding.left - alignmentCopy.GetPadding().left ) < GetRangedEpsilon( padding.left, alignmentCopy.GetPadding().left ) );
  DALI_TEST_CHECK( fabs( padding.right - alignmentCopy.GetPadding().right ) < GetRangedEpsilon( padding.right, alignmentCopy.GetPadding().right ) );
  DALI_TEST_CHECK( fabs( padding.top - alignmentCopy.GetPadding().top ) < GetRangedEpsilon( padding.top, alignmentCopy.GetPadding().top ) );
  DALI_TEST_CHECK( fabs( padding.bottom - alignmentCopy.GetPadding().bottom ) < GetRangedEpsilon( padding.bottom, alignmentCopy.GetPadding().bottom ) );

  Alignment alignmentEmptyCopy(emptyAlignment);
  DALI_TEST_CHECK(emptyAlignment == alignmentEmptyCopy);

  Alignment alignmentEquals;
  alignmentEquals = alignment;
  DALI_TEST_CHECK( fabs( padding.left - alignmentEquals.GetPadding().left ) < GetRangedEpsilon( padding.left, alignmentEquals.GetPadding().left ) );
  DALI_TEST_CHECK( fabs( padding.right - alignmentEquals.GetPadding().right ) < GetRangedEpsilon( padding.right, alignmentEquals.GetPadding().right ) );
  DALI_TEST_CHECK( fabs( padding.top - alignmentEquals.GetPadding().top ) < GetRangedEpsilon( padding.top, alignmentEquals.GetPadding().top ) );
  DALI_TEST_CHECK( fabs( padding.bottom - alignmentEquals.GetPadding().bottom ) < GetRangedEpsilon( padding.bottom, alignmentEquals.GetPadding().bottom ) );

  Alignment alignmentEmptyEquals;
  alignmentEmptyEquals = emptyAlignment;
  DALI_TEST_CHECK(emptyAlignment == alignmentEmptyEquals);

  // Self assignment
  alignment = alignment;
  DALI_TEST_CHECK(alignment == alignmentCopy);
  END_TEST;
}
