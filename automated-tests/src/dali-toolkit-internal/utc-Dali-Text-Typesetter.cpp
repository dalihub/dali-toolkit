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

#include <iostream>

#include <stdlib.h>
#include <limits>

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/text/rendering/text-typesetter.h>
#include <dali-toolkit/internal/text/rendering/view-model.h>
#include <dali-toolkit/internal/text/text-controller.h>

using namespace Dali;
using namespace Toolkit;
using namespace Text;

int UtcDaliTextTypesetter(void)
{
  tet_infoline(" UtcDaliTextTypesetter");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New( controller->GetTextModel() );
  DALI_TEST_CHECK( typesetter );

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliTextTypesetterGetViewModel(void)
{
  tet_infoline(" UtcDaliTextTypesetter");
  ToolkitTestApplication application;

  // Creates a text controller.
  ControllerPtr controller = Controller::New();

  // Tests the rendering controller has been created.
  TypesetterPtr typesetter = Typesetter::New( controller->GetTextModel() );
  DALI_TEST_CHECK( typesetter );

  // Tests the view model has been created.
  ViewModel* model = typesetter->GetViewModel();
  DALI_TEST_CHECK( NULL != model );

  tet_result(TET_PASS);
  END_TEST;
}
