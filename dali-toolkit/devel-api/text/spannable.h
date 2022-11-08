#ifndef DALI_TOOLKIT_TEXT_SPANNABLE_H
#define DALI_TOOLKIT_TEXT_SPANNABLE_H

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
#include <dali-toolkit/devel-api/text/range.h>
#include <dali-toolkit/devel-api/text/spanned.h>
#include <dali-toolkit/devel-api/text/spans/base-span.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class Spannable;
}

/**
 * @brief Interface for text to which spans objects can be attached and detached.
 * It provides uniform APIs for mutable formatted character sequences.
 *
 * @note An instance of this class cannot be created.
 *
 */
class DALI_TOOLKIT_API Spannable : public Spanned
{
public:
  /**
   * @brief Creates an uninitialized Spannable handle.
   *
   * Calling member functions with an uninitialized Spannable handle is not allowed.
   */
  Spannable();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs A reference to the copied handle
   */
  Spannable(const Spannable& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the handle to move
   */
  Spannable(Spannable&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle A reference to the copied handle
   * @return A reference to this
   */
  Spannable& operator=(const Spannable& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the handle to move
   * @return A reference to this handle
   */
  Spannable& operator=(Spannable&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~Spannable();

public:
  /**
   * @brief Add the given style span on the given range of text.
   *
   * @param[in] styleSpan The span of style to apply it on range @p range
   * @param[in] range The range.
   *
   * @return true if the @p range is valid on text. Otherwise false
   */
  bool AttachSpan(BaseSpan styleSpan, Range range);

  /**
   * @brief Remove the given style span on all ranges of text.
   * checks if @p styleSpan exists, and if it does, it's to be removed.
   *
   * @param[in] styleSpan The span of style to remove with ranges.
   *
   * @return true if the @p styleSpan was exist and removed. Otherwise false
   */
  bool DetachSpan(const BaseSpan& styleSpan);

protected:
  /**
   * @brief Downcasts to a Spannable handle.
   * If handle is not a Spannable, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Spannable handle or an uninitialized handle
   */
  static Spannable DownCast(BaseHandle handle);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Internal constructor.
   *
   * @param[in] spannable Pointer to internal Spannable
   */
  explicit DALI_INTERNAL Spannable(Internal::Spannable* spannable);
  /// @endcond
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_SPANNABLE_H
