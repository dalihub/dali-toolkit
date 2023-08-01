#ifndef DALI_TOOLKIT_TEST_SUITE_UTILS_H
#define DALI_TOOLKIT_TEST_SUITE_UTILS_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/visuals/visual-properties.h>

// Put any toolkit specific operators for DALI_TEST_CHECK template here, before dali-test-suite-utils.h.

std::ostream& operator<<( std::ostream& ostream, Dali::Toolkit::Visual::ResourceStatus status );

// INTERNAL INCLUDES

#include <dali-test-suite-utils.h>
#include "toolkit-test-application.h"
#include "toolkit-application.h"
#include "toolkit-input-method-context.h"

#endif // DALI_TOOLKIT_TEST_SUITE_UTILS_H
