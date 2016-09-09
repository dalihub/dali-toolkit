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
 *
 */

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/progress-bar/progress-bar.h>

using namespace Dali;
using namespace Dali::Toolkit;
using Dali::Toolkit::ProgressBar;


void utc_dali_toolkit_progressbar_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_toolkit_progressbar_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

static bool gObjectCreatedCallBackCalled;

static void TestCallback(BaseHandle handle)
{
  gObjectCreatedCallBackCalled = true;
}

}

int UtcDaliProgressBarNew(void)
{
  ToolkitTestApplication application;
  tet_infoline(" UtcDaliProgressBarNew");

  // Create the ProgressBar actor
  ProgressBar progressBar;

  DALI_TEST_CHECK( !progressBar );

  progressBar = ProgressBar::New();

  DALI_TEST_CHECK( progressBar );

  ProgressBar progressBar2(progressBar);

  DALI_TEST_CHECK( progressBar2 == progressBar );

  ProgressBar progressBar3;
  progressBar3 = progressBar2;

  DALI_TEST_CHECK( progressBar3 == progressBar2 );

  //Additional check to ensure object is created by checking if it's registered
  ObjectRegistry registry = Stage::GetCurrent().GetObjectRegistry();
  DALI_TEST_CHECK( registry );

  gObjectCreatedCallBackCalled = false;
  registry.ObjectCreatedSignal().Connect( &TestCallback );
  {
    ProgressBar progressBar = ProgressBar::New();
  }
  DALI_TEST_CHECK( gObjectCreatedCallBackCalled );
  END_TEST;
}

int UtcDaliProgressBarDestructor(void)
{
  ToolkitTestApplication application;

  ProgressBar* progressBar = new ProgressBar();
  delete progressBar;

  DALI_TEST_CHECK( true );
  END_TEST;
}

int UtcDaliProgressBarDownCast(void)
{
  ToolkitTestApplication application;

  Handle handle = ProgressBar::New();

  ProgressBar progressBar = ProgressBar::DownCast( handle );

  DALI_TEST_CHECK( progressBar == handle );
  END_TEST;
}

static bool gProgressBarValueChangedCallBackCalled = false;

static void OnProgressBarValueChanged( ProgressBar progressBar, float value )
{
  gProgressBarValueChangedCallBackCalled = true;
}

int UtcDaliProgressBarSignals(void)
{
  ToolkitTestApplication application;  // Exceptions require ToolkitTestApplication
  tet_infoline(" UtcDaliProgressBarSignals");

  // Create the ProgressBar actor
  ProgressBar progressBar = ProgressBar::New();
  Stage::GetCurrent().Add( progressBar );
  progressBar.SetParentOrigin(ParentOrigin::TOP_LEFT);
  progressBar.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  progressBar.SetSize( Vector2( Stage::GetCurrent().GetSize().x, 20.0f ) );
  progressBar.SetPosition( 0.0f, 0.0f );

  progressBar.ValueChangedSignal().Connect( &OnProgressBarValueChanged );
  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.2f);

  application.SendNotification();
  application.Render();

  //gProgressBarValueChangedCallBackCalled = false;

  DALI_TEST_CHECK(gProgressBarValueChangedCallBackCalled);
  END_TEST;
}

int UtcDaliProgressBarSetPropertyP(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliProgressBarSetPropertyP" );

  ProgressBar progressBar = ProgressBar::New();
  progressBar.SetParentOrigin(ParentOrigin::TOP_LEFT);
  progressBar.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  progressBar.SetSize( Vector2( Stage::GetCurrent().GetSize().x, 20.0f ) );
  progressBar.SetPosition( 0.0f, 0.0f );

  Stage::GetCurrent().Add(progressBar);
  application.SendNotification();
  application.Render();

  float val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.0f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.2f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.2f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.8f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.8f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.4f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.4f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.0f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.0f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 1.0f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 1.0f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, -1.0f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 1.0f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.9f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.9f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 1.1f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.9f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 2.0f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.9f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.0f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.0f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.9f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.9f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.09f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.09f, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.1f);
  val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.1f, TEST_LOCATION);

  {
    Property::Map map;
    map["rendererType"] = "image";
    map["size"] = Vector2(200, 200);
    map["url"] = "track2.png";
    progressBar.SetProperty(ProgressBar::Property::TRACK_VISUAL, map);
    map["url"] = "progress2.png";
    progressBar.SetProperty(ProgressBar::Property::PROGRESS_VISUAL, map);

    Property::Value value = progressBar.GetProperty(ProgressBar::Property::TRACK_VISUAL);
    Property::Map* resultMap = value.GetMap();
    DALI_TEST_CHECK( resultMap );
    Property::Value* url = resultMap->Find("url");
    DALI_TEST_CHECK( url ) ;
    DALI_TEST_EQUALS( *url, "track2.png", TEST_LOCATION );

    value = progressBar.GetProperty(ProgressBar::Property::PROGRESS_VISUAL);
    resultMap = value.GetMap();
    DALI_TEST_CHECK( resultMap );
    url = resultMap->Find("url");
    DALI_TEST_CHECK( url ) ;
    DALI_TEST_EQUALS( *url, "progress2.png", TEST_LOCATION );

   }

  END_TEST;
}

int UtcDaliProgressBarSetPropertyP1(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliProgressBarSetPropertyP1" );

  ProgressBar progressBar = ProgressBar::New();
  progressBar.SetParentOrigin(ParentOrigin::TOP_LEFT);
  progressBar.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  progressBar.SetSize( Vector2( Stage::GetCurrent().GetSize().x, 20.0f ) );
  progressBar.SetPosition( 0.0f, 0.0f );

  Stage::GetCurrent().Add(progressBar);
  application.SendNotification();
  application.Render();

  float val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.0f, TEST_LOCATION);

  // test to download a file of 100k in chunks
  float lowerBound = 0, upperBound = 100, progressValue = 0, chunkValue = 0;

  while( chunkValue <= upperBound )
  {
    progressValue = (chunkValue - lowerBound ) / ( upperBound - lowerBound );
    progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, progressValue);
    val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
    DALI_TEST_EQUALS(val, progressValue, TEST_LOCATION);
    chunkValue = chunkValue + 10;
  }

  // test to download a file of 1000k in chunks
  lowerBound = 0, upperBound = 1000, progressValue = 0, chunkValue = 0;

  while( chunkValue <= upperBound )
  {
    progressValue = (chunkValue - lowerBound ) / ( upperBound - lowerBound );
    progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, progressValue);
    val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
    DALI_TEST_EQUALS(val, progressValue, TEST_LOCATION);
    chunkValue = chunkValue + 100;
  }

  END_TEST;
}

