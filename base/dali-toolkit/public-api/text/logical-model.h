#ifndef __DALI_TOOLKIT_TEXT_LOGICAL_MODEL_H__
#define __DALI_TOOLKIT_TEXT_LOGICAL_MODEL_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/text/text-definitions.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

class LogicalModel;
typedef IntrusivePtr<LogicalModel> LogicalModelPtr;

/**
 * @brief A logical text model contains layout independent information.
 *
 * This includes:
 * - A series of UTF-32 characters in logical order
 */
class LogicalModel : public RefObject
{
public:

  /**
   * @brief Create a new instance of a LogicalModel.
   *
   * @return A pointer to a new LogicalModel.
   */
  static LogicalModelPtr New();

  // Text interface.

  /**
   * @brief Replaces any text previously set.
   *
   * @param[in] text An array of UTF-32 characters.
   * @param[in] length The length of the array.
   */
  void SetText( const Character* text,
                Length length );

  /**
   * @brief Retrieves the number of characters of the text.
   *
   * @return The number of characters.
   */
  Length GetNumberOfCharacters() const;

  /**
   * @brief Retrieves characters from the text in the given buffer.
   *
   * @pre The size of the @p text buffer needs to be big enough to copy the @p numberOfCharacters.
   * @param[in] characterIndex The index to the first character to copy.
   * @param[out] text Pointer to a buffer where the text is copied.
   * @param[in] numberOfCharacters The number of characters to be copied.
   */
  void GetText( CharacterIndex characterIndex,
                Character* text,
                Length numberOfCharacters ) const;

protected:

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~LogicalModel();

private:

  /**
   * @brief Private constructor.
   */
  LogicalModel();

  // Undefined
  LogicalModel( const LogicalModel& handle );

  // Undefined
  LogicalModel& operator=( const LogicalModel& handle );

private:

  struct Impl;
  Impl* mImpl;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_LOGICAL_MODEL_H__
