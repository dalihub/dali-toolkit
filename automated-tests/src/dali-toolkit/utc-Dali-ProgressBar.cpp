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
#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>

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
static const char* trackImage = TEST_RESOURCE_DIR "/progress-bar-skin-track-progress.png";
static const char* progressImage = TEST_RESOURCE_DIR "/progress-bar-skin-progress-progress.png";
static const char* secondaryProgressImage = TEST_RESOURCE_DIR "/progress-bar-skin-secondary-progress.png";
static const char* indeterminateImage = TEST_RESOURCE_DIR "/progress-bar-skin-indeterminate.png";
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

static void OnProgressBarValueChanged( ProgressBar progressBar, float value, float secondaryValue )
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

  DALI_TEST_CHECK(gProgressBarValueChangedCallBackCalled);

  //Check Secondary progress value can make signal
  gProgressBarValueChangedCallBackCalled = false;
  progressBar.SetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VALUE, 0.2f);

  application.SendNotification();
  application.Render();

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
  progressBar.ValueChangedSignal().Connect( &OnProgressBarValueChanged );
  Stage::GetCurrent().Add(progressBar);
  application.SendNotification();
  application.Render();

  Property::Map map;
  map["visualType"] = "IMAGE";
  map[Toolkit::ImageVisual::Property::URL] = trackImage;
  progressBar.SetProperty(ProgressBar::Property::TRACK_VISUAL, map);

  Property::Value value = progressBar.GetProperty(ProgressBar::Property::TRACK_VISUAL);
  Property::Map* resultMap = value.GetMap();
  DALI_TEST_CHECK( resultMap );
  Property::Value* urlValue = resultMap->Find( Toolkit::ImageVisual::Property::URL );
  std::string urlString;
  urlValue->Get( urlString );
  DALI_TEST_CHECK( !urlString.empty() ) ;
  DALI_TEST_EQUALS( urlString, trackImage, TEST_LOCATION );

  Property::Map colorMap;
  colorMap["visualType"] = "COLOR";
  colorMap[Toolkit::ColorVisual::Property::MIX_COLOR] = Color::RED;
  progressBar.SetProperty(ProgressBar::Property::TRACK_VISUAL, colorMap);

  value = progressBar.GetProperty(ProgressBar::Property::TRACK_VISUAL);
  resultMap = value.GetMap();
  DALI_TEST_CHECK( resultMap );
  Property::Value* colorValue = resultMap->Find( Toolkit::ColorVisual::Property::MIX_COLOR );
  Vector4 color = Color::TRANSPARENT;
  colorValue->Get( color );
  DALI_TEST_CHECK( color != Color::TRANSPARENT );
  DALI_TEST_EQUALS( color, Color::RED, TEST_LOCATION );

  map[Toolkit::ImageVisual::Property::URL] = progressImage;
  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VISUAL, map);

  value = progressBar.GetProperty(ProgressBar::Property::PROGRESS_VISUAL);
  resultMap = value.GetMap();
  DALI_TEST_CHECK( resultMap );
  urlValue = resultMap->Find( Toolkit::ImageVisual::Property::URL );
  urlValue->Get( urlString );
  DALI_TEST_CHECK( !urlString.empty() ) ;
  DALI_TEST_EQUALS( urlString, progressImage, TEST_LOCATION );

  progressBar.SetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VISUAL, secondaryProgressImage);

  value = progressBar.GetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VISUAL);
  resultMap = value.GetMap();
  DALI_TEST_CHECK( resultMap );
  urlValue = resultMap->Find( Toolkit::ImageVisual::Property::URL );
  urlValue->Get( urlString );
  DALI_TEST_CHECK( !urlString.empty() ) ;
  DALI_TEST_EQUALS( urlString, secondaryProgressImage, TEST_LOCATION );

  map[Toolkit::ImageVisual::Property::URL] = secondaryProgressImage;
  progressBar.SetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VISUAL, map);

  value = progressBar.GetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VISUAL);
  resultMap = value.GetMap();
  DALI_TEST_CHECK( resultMap );
  urlValue = resultMap->Find( Toolkit::ImageVisual::Property::URL );
  urlValue->Get( urlString );
  DALI_TEST_CHECK( !urlString.empty() ) ;
  DALI_TEST_EQUALS( urlString, secondaryProgressImage, TEST_LOCATION );

  map[Toolkit::ImageVisual::Property::URL] = indeterminateImage;
  progressBar.SetProperty(ProgressBar::Property::INDETERMINATE_VISUAL, map);

  value = progressBar.GetProperty(ProgressBar::Property::INDETERMINATE_VISUAL);
  resultMap = value.GetMap();
  DALI_TEST_CHECK( resultMap );
  urlValue = resultMap->Find( Toolkit::ImageVisual::Property::URL );
  urlValue->Get( urlString );
  DALI_TEST_CHECK( !urlString.empty() ) ;
  DALI_TEST_EQUALS( urlString, indeterminateImage, TEST_LOCATION );

  progressBar.SetProperty(ProgressBar::Property::LABEL_VISUAL, "test1");

  value = progressBar.GetProperty(ProgressBar::Property::LABEL_VISUAL);
  resultMap = value.GetMap();
  DALI_TEST_CHECK( resultMap );
  Property::Value* textValue = resultMap->Find(Toolkit::TextVisual::Property::TEXT);

  std::string textString;
  textValue->Get( textString );
  DALI_TEST_CHECK( !textString.empty() );
  DALI_TEST_EQUALS( textString, std::string( "test1" ), TEST_LOCATION );

  Property::Map textVisualMap2;
  textVisualMap2.Insert( Toolkit::Visual::Property::TYPE, Toolkit::DevelVisual::TEXT );
  textVisualMap2.Insert( Toolkit::TextVisual::Property::TEXT, "test2" );
  textVisualMap2.Insert( Toolkit::TextVisual::Property::POINT_SIZE, 12.0f );
  progressBar.SetProperty(ProgressBar::Property::LABEL_VISUAL, textVisualMap2);

  value = progressBar.GetProperty(ProgressBar::Property::LABEL_VISUAL);
  resultMap = value.GetMap();
  DALI_TEST_CHECK( resultMap );
  textValue = resultMap->Find(Toolkit::TextVisual::Property::TEXT);

  textValue->Get( textString );
  DALI_TEST_CHECK( !textString.empty() );
  DALI_TEST_EQUALS( textString, std::string( "test2" ), TEST_LOCATION );

  progressBar.SetProperty(ProgressBar::Property::LABEL_VISUAL, "test3");

  value = progressBar.GetProperty(ProgressBar::Property::LABEL_VISUAL);
  resultMap = value.GetMap();
  DALI_TEST_CHECK( resultMap );
  textValue = resultMap->Find(Toolkit::TextVisual::Property::TEXT);

  std::string textString2;
  textValue->Get( textString2 );
  DALI_TEST_CHECK( !textString2.empty() );
  DALI_TEST_EQUALS( textString2, std::string( "test3" ), TEST_LOCATION );

  Property::Map transitionMap;
  transitionMap["target"] = "indeterminateVisual";
  transitionMap["property"] = "offset";
  transitionMap["initialValue"] = Vector2( 0.0f, 0.0f );
  transitionMap["targetValue"] = Vector2( 10.0f, 0.0f );
  transitionMap["animator"] = Property::Map().Add("alphaFunction", "EASE_IN_OUT_BACK")
                                             .Add("timePeriod", Property::Map().Add("delay", 0.5f).Add("duration", 1.0f));
  Property::Array array;
  array.PushBack( transitionMap );
  progressBar.SetProperty(ProgressBar::Property::INDETERMINATE_VISUAL_ANIMATION, array);

  Property::Map transitionMap2;
  transitionMap2["target"] = "indeterminateVisual";
  transitionMap2["property"] = "offset";
  transitionMap2["initialValue"] = Vector2( 0.0f, 0.0f );
  transitionMap2["targetValue"] = Vector2( 15.0f, 0.0f );
  transitionMap2["animator"] = Property::Map().Add("alphaFunction", "EASE_IN_OUT_BACK")
                                             .Add("timePeriod", Property::Map().Add("delay", 0.5f).Add("duration", 1.0f));
  progressBar.SetProperty(ProgressBar::Property::INDETERMINATE_VISUAL_ANIMATION, transitionMap2);
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
  progressBar.ValueChangedSignal().Connect( &OnProgressBarValueChanged );

  Stage::GetCurrent().Add(progressBar);
  application.SendNotification();
  application.Render();

  float val = progressBar.GetProperty<float>(ProgressBar::Property::PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.0f, TEST_LOCATION);

  // test to download a file of 100k in chunks
  float lowerBound = 0, upperBound = 100, progressValue = 0, secondaryProgressValue = 0, chunkValue = 0;

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

  val = progressBar.GetProperty<float>(ProgressBar::Property::SECONDARY_PROGRESS_VALUE);
  DALI_TEST_EQUALS(val, 0.0f, TEST_LOCATION);

  // test to download a file of 100k in chunks
  lowerBound = 0, upperBound = 100, secondaryProgressValue = 0, chunkValue = 0;

  while( chunkValue <= upperBound )
  {
    secondaryProgressValue = (chunkValue - lowerBound ) / ( upperBound - lowerBound );
    progressBar.SetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VALUE, secondaryProgressValue);
    val = progressBar.GetProperty<float>(ProgressBar::Property::SECONDARY_PROGRESS_VALUE);
    DALI_TEST_EQUALS(val, secondaryProgressValue, TEST_LOCATION);
    chunkValue = chunkValue + 10;
  }

  // test to download a file of 1000k in chunks
  lowerBound = 0, upperBound = 1000, secondaryProgressValue = 0, chunkValue = 0;

  while( chunkValue <= upperBound )
  {
    secondaryProgressValue = (chunkValue - lowerBound ) / ( upperBound - lowerBound );
    progressBar.SetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VALUE, secondaryProgressValue);
    val = progressBar.GetProperty<float>(ProgressBar::Property::SECONDARY_PROGRESS_VALUE);
    DALI_TEST_EQUALS(val, secondaryProgressValue, TEST_LOCATION);
    chunkValue = chunkValue + 100;
  }

  END_TEST;
}

int UtcDaliProgressBarSetPropertyP2(void)
{
  ToolkitTestApplication application;
  tet_infoline( "UtcDaliProgressBarSetPropertyP2" );

  ProgressBar progressBar = ProgressBar::New();
  progressBar.SetParentOrigin(ParentOrigin::TOP_LEFT);
  progressBar.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  progressBar.SetPosition( 0.0f, 0.0f );
  progressBar.SetProperty(ProgressBar::Property::LABEL_VISUAL, "test");
  progressBar.SetProperty(ProgressBar::Property::INDETERMINATE, true);
  progressBar.SetProperty(ProgressBar::Property::TRACK_VISUAL, trackImage);
  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VISUAL, progressImage);
  progressBar.SetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VISUAL, secondaryProgressImage);
  progressBar.SetProperty(ProgressBar::Property::INDETERMINATE_VISUAL, indeterminateImage);

  Property::Map transitionMap;
  transitionMap["target"] = "indeterminateVisual";
  transitionMap["property"] = "offset";
  transitionMap["initialValue"] = Vector2( 0.0f, 0.0f );
  transitionMap["targetValue"] = Vector2( 10.0f, 0.0f );
  transitionMap["animator"] = Property::Map().Add("alphaFunction", "EASE_IN_OUT_BACK")
                                             .Add("timePeriod", Property::Map().Add("delay", 0.5f).Add("duration", 1.0f));
  progressBar.SetProperty(ProgressBar::Property::INDETERMINATE_VISUAL_ANIMATION, transitionMap);
  progressBar.SetProperty(ProgressBar::Property::PROGRESS_VALUE, 0.2f);
  progressBar.SetProperty(ProgressBar::Property::SECONDARY_PROGRESS_VALUE, 0.3f);

  progressBar.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  progressBar.SetSize( Vector2( Stage::GetCurrent().GetSize().x, 20.0f ) );
  Stage::GetCurrent().Add(progressBar);
  application.SendNotification();
  application.Render();

  // Test get/set INDETERMINATE
  bool val = progressBar.GetProperty<bool>(ProgressBar::Property::INDETERMINATE);
  DALI_TEST_EQUALS(val, true, TEST_LOCATION);

  progressBar.SetProperty(ProgressBar::Property::INDETERMINATE, false);
  val = progressBar.GetProperty<bool>(ProgressBar::Property::INDETERMINATE);
  DALI_TEST_EQUALS(val, false, TEST_LOCATION);

  END_TEST;
}
