#ifndef DALI_TOOLKIT_TEXT_SPANNABLE_STRING_H
#define DALI_TOOLKIT_TEXT_SPANNABLE_STRING_H

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
#include <dali-toolkit/devel-api/text/spannable.h>
#include <string>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class SpannableString;
}

/**
 * @brief Interface for text that has spans objects attached to ranges of it.
 * It provides uniform APIs for read-only access to many different kinds of formatted character sequences.
 *
 * @note An instance of this class cannot be created.
 *
 */
class DALI_TOOLKIT_API SpannableString : public Spannable
{
public:
  /**
   * @brief Create an initialized SpannableString.
   *
   * @param[in] text the text to be formatted
   *
   * @return A handle to a newly allocated Dali resource
   */
  static SpannableString New(const std::string& text);

  /**
   * @brief Creates an uninitialized SpannableString handle.
   *
   * Calling member functions with an uninitialized SpannableString handle is not allowed.
   */
  SpannableString();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs A reference to the copied handle
   */
  SpannableString(const SpannableString& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the handle to move
   */
  SpannableString(SpannableString&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle A reference to the copied handle
   * @return A reference to this
   */
  SpannableString& operator=(const SpannableString& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the handle to move
   * @return A reference to this handle
   */
  SpannableString& operator=(SpannableString&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~SpannableString();

protected:
  /**
   * @brief Downcasts to a SpannableString handle.
   * If handle is not a SpannableString, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return SpannableString handle or an uninitialized handle
   */
  static SpannableString DownCast(BaseHandle handle);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Internal constructor.
   *
   * @param[in] spannableString Pointer to internal SpannableString
   */
  explicit DALI_INTERNAL SpannableString(Internal::SpannableString* spannableString);
  /// @endcond
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_SPANNABLE_STRING_H
