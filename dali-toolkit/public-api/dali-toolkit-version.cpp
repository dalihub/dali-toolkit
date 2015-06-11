/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// HEADER
#include <dali-toolkit/public-api/dali-toolkit-version.h>

// EXTERNAL INCLUDES
#ifdef DEBUG_ENABLED
#include <iostream>
#endif

namespace Dali
{

namespace Toolkit
{

const unsigned int TOOLKIT_MAJOR_VERSION = 1;
const unsigned int TOOLKIT_MINOR_VERSION = 0;
const unsigned int TOOLKIT_MICRO_VERSION = 44;
const char * const TOOLKIT_BUILD_DATE    = __DATE__ " " __TIME__;

#ifdef DEBUG_ENABLED
namespace
{
/// Allows the printing of the version number ONLY when debug is enabled
struct PrintVersion
{
  PrintVersion()
  {
    std::cout << "DALi Toolkit:   " << TOOLKIT_MAJOR_VERSION << "." << TOOLKIT_MINOR_VERSION << "." << TOOLKIT_MICRO_VERSION << " (" << TOOLKIT_BUILD_DATE << ")" << std::endl;
  }
};
PrintVersion TOOLKIT_VERSION;
} // unnamed namespace
#endif

} // namespace Toolkit

} // namespace Dali
