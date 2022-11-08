#ifndef DALI_TOOLKIT_TEXT_CHARACTER_SEQUENCE_H
#define DALI_TOOLKIT_TEXT_CHARACTER_SEQUENCE_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/text-abstraction/text-abstraction-definitions.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class CharacterSequence;
}

/**
 * @brief Interface for readable character sequence.
 * It provides uniform APIs for read-only access to many different kinds of character sequences.
 *
 * @note An instance of this class cannot be created.
 *
 */
class DALI_TOOLKIT_API CharacterSequence : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized CharacterSequence handle.
   *
   * Calling member functions with an uninitialized CharacterSequence handle is not allowed.
   */
  CharacterSequence();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs A reference to the copied handle
   */
  CharacterSequence(const CharacterSequence& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the handle to move
   */
  CharacterSequence(CharacterSequence&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle A reference to the copied handle
   * @return A reference to this
   */
  CharacterSequence& operator=(const CharacterSequence& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the handle to move
   * @return A reference to this handle
   */
  CharacterSequence& operator=(CharacterSequence&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~CharacterSequence();

protected:
  /**
   * @brief Downcasts to a CharacterSequence handle.
   * If handle is not a CharacterSequence, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return CharacterSequence handle or an uninitialized handle
   */
  static CharacterSequence DownCast(BaseHandle handle);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Internal constructor.
   *
   * @param[in] characterSequence Pointer to internal CharacterSequence
   */
  explicit DALI_INTERNAL CharacterSequence(Internal::CharacterSequence* characterSequence);
  /// @endcond

public: //Methods
  /**
  * @brief Retrive the characters
  *
  * @return the utf32 characters.
  */
  Vector<uint32_t> GetCharacters() const;

  /**
  * @brief Retrive number of characters in container
  *
  * @return the number of characters.
  */
  uint32_t GetNumberOfCharacters() const;

  /**
  * @brief Retrieve constructed string form Character Sequence.
  * Copy the characters from container to string
  *
  * @return the std string.
  */
  std::string ToString() const;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_CHARACTER_SEQUENCE_H
