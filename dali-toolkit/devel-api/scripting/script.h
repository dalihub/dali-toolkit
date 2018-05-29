#ifndef __DALI_TOOLKIT_SCRIPT_H__
#define __DALI_TOOLKIT_SCRIPT_H__

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

// EXTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Script;
}

/**
 * Script
 * This class provides the ability to execute script in a supported language.
 * Specific script language support is provided through plugins.
 *
 * For Example.
 *
 * @code
 *
 * Script script = Script::New();
 *
 * script.ExecuteFile("bump-map.js");
 *
 * @endcode
 *
 */
class DALI_TOOLKIT_API Script : public BaseHandle
{

public:

  /**
   * Create an Script handle; this can be initialised with Script::New()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  Script();

  /**
   * Creates an Script object.
   * @return A handle to the Script control.
   */
  static Script New();

  /**
   * Virtual destructor.
   */
  ~Script();

  /**
   * Executes the contents of filename in a scripted environment.
   * @pre A Dali Application object exists
   * @param filename A filename of a script file to execute
   * @return true on success, false on failure
   *
   */
  bool ExecuteFile( const std::string& filename );

private:

  Script(Internal::Script *impl);

}; // class Script

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SCRIPT_H__
