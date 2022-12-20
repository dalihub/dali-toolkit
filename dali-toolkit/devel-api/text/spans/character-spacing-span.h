#ifndef DALI_TOOLKIT_TEXT_CHARACTER_SPACING_SPAN_H
#define DALI_TOOLKIT_TEXT_CHARACTER_SPACING_SPAN_H

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

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/text/spans/base-span.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class CharacterSpacingSpan;
}

/**
 * @brief CharacterSpacingSpan is a handle to an object that specifies the CharacterSpacing for range of characters.
 */
class DALI_TOOLKIT_API CharacterSpacingSpan : public BaseSpan
{
public:
  /**
   * @brief Create an initialized CharacterSpacingSpan.
   *
   * @param[in] value The CharacterSpacing value.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static CharacterSpacingSpan New(const float value);

  /**
   * @brief Creates an uninitialized CharacterSpacingSpan handle.
   *
   * Calling member functions with an uninitialized CharacterSpacingSpan handle is not allowed.
   */
  CharacterSpacingSpan();

  /**
   * @brief Copy constructor.
   * @param[in] rhs A reference to the copied handle
   */
  CharacterSpacingSpan(const CharacterSpacingSpan& rhs);

  /**
   * @brief Move constructor.
   * @param[in] rhs A reference to the handle to move
   */
  CharacterSpacingSpan(CharacterSpacingSpan&& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  CharacterSpacingSpan& operator=(const CharacterSpacingSpan& rhs);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  CharacterSpacingSpan& operator=(CharacterSpacingSpan&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~CharacterSpacingSpan();

  /**
   * @brief Downcasts to a CharacterSpacingSpan handle.
   * If handle is not a CharacterSpacingSpan, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return CharacterSpacingSpan handle or an uninitialized handle
   */
  static CharacterSpacingSpan DownCast(BaseHandle handle);

public: //Methods
  /**
   * @brief Retrive the CharacterSpacing.
   *
   * @return A CharacterSpacing value.
   */
  const float GetCharacterSpacing() const;

  /**
   * @brief Retrieve whether the CharacterSpacing is defined.
   *
   * @return The return is true if CharacterSpacing is defined, otherwise false.
   */
  bool IsCharacterSpacingDefined() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized CharacterSpacingSpan handle.
   *
   * @param[in] charSpacingSpan Pointer to internal CharacterSpacingSpan
   */
  explicit DALI_INTERNAL CharacterSpacingSpan(Internal::CharacterSpacingSpan* charSpacingSpan);
  /// @endcond
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_CHARACTER_SPACING_SPAN_H
