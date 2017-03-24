/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <fstream>
#include <stdlib.h>
#include <locale.h>
#include <libintl.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-popup.h>

using namespace Dali;
using namespace Toolkit;

namespace
{

const char* DEFAULT_LOCALE_DIR = "/tmp/locale/";
const std::string DEFAULT_EN_LOCALE_DIR = "/tmp/locale/en/LC_MESSAGES/";

}

void dali_textselectionpopupmirroringrtl_startup(void)
{
  // Cheat! Copy the .mo file
  std::ifstream  src( std::string( DEFAULT_EN_LOCALE_DIR + "dali-toolkit.mo.ar" ).c_str(), std::ifstream::binary );
  std::ofstream  dst( std::string( DEFAULT_EN_LOCALE_DIR + "dali-toolkit.mo" ).c_str(), std::ofstream::binary );
  dst << src.rdbuf();

  test_return_value = TET_UNDEF;
}

void dali_textselectionpopupmirroringrtl_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliToolkitTextSelectionPopupMirroringRTL(void)
{
  // Test the popup mirroring.
  const std::string CUT( "optionCut" );
  const std::string COPY( "optionCopy" );
  const std::string PASTE( "optionPaste" );

  ToolkitTestApplication application;

  setlocale( LC_ALL, "en_GB.UTF-8" );
  textdomain("dali-toolkit");
  bindtextdomain("dali-toolkit", DEFAULT_LOCALE_DIR );

  TextSelectionPopup textSelectionPopup = TextSelectionPopup::New( NULL );

  // Enable some buttons.
  TextSelectionPopup::Buttons buttons = static_cast<TextSelectionPopup::Buttons>( TextSelectionPopup::COPY | TextSelectionPopup::CUT | TextSelectionPopup::PASTE );
  textSelectionPopup.EnableButtons( buttons );

  // Show the popup.
  textSelectionPopup.ShowPopup();

  Actor cutActor = textSelectionPopup.FindChildByName( CUT );
  if( !cutActor )
  {
    tet_result(TET_FAIL);
  }

  Actor tableOfButtons = cutActor.GetParent();
  if( !tableOfButtons )
  {
    tet_result(TET_FAIL);
  }

  // The order should be PASTE, CUT, COPY
  DALI_TEST_EQUALS( PASTE, tableOfButtons.GetChildAt( 0 ).GetName(), TEST_LOCATION );
  DALI_TEST_EQUALS( CUT, tableOfButtons.GetChildAt( 2 ).GetName(), TEST_LOCATION );
  DALI_TEST_EQUALS( COPY, tableOfButtons.GetChildAt( 4 ).GetName(), TEST_LOCATION );

  tet_result(TET_PASS);
  END_TEST;
}
