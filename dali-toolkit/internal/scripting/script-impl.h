#ifndef __DALI_TOOLKIT_INTERNAL_SCRIPT_H__
#define __DALI_TOOLKIT_INTERNAL_SCRIPT_H__

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
// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/scripting/script.h>
#include <dali-toolkit/devel-api/scripting/script-plugin.h>


namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class Script;

/**
 * @copydoc Toolkit::Script
 */
class Script : public Dali::BaseObject
{
public:

  /**
   * @brief Constructor
   */
  Script();

  /**
   * @copydoc Toolkit::Script::ExecuteFile
   */
  void ExecuteFile( const std::string& filename );

protected:

  /**
   *  @brief virtual destructor
   */
  virtual ~Script();

private:

  /**
   * @brief Undefined copy  constructor
   */
  Script(const Script&);

  /**
   * @brief Undefined assignment operator
   */
  Script& operator=(const Script& rhs);

private: // data

  ScriptPlugin* mPlugin;    ///< plugin

};

} // namespace Internal

inline Internal::Script& GetImpl(Dali::Toolkit::Script& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::Script&>(handle);
}

inline const Internal::Script& GetImpl(const Dali::Toolkit::Script& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::Script&>(handle);
}


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SCRIPT_H__
