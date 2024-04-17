#ifndef DALI_TOOLKIT_TEXT_RANGE_H
#define DALI_TOOLKIT_TEXT_RANGE_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
namespace Text
{
namespace Internal DALI_INTERNAL
{
class Range;
}

/**
 * @brief Base structure for different text-spans.
 * The application can attach and detach spans on range of text for text-controller.
 * Used to modify text style dynamically at runtime on character level.
 *
 * A text-span contains set of attributes of specific style for text.
 *
 * To receive a particular text-span, the application has to create span through its builder.
 *
 * @note An instance of this class cannot be created.
 *
 */
class DALI_TOOLKIT_API Range : public BaseHandle
{
public:
  /**
   * @brief Create an initialized range.
   *
   * @param[in] startIndex the start index of range (included)
   * @param[in] endIndex the end index of range (included)
   *
   * @return A handle to a newly allocated Dali resource
   */
  static Range New(u_int32_t startIndex, u_int32_t endIndex);

  /**
   * @brief Creates an uninitialized Range handle.
   *
   * Calling member functions with an uninitialized Range handle is not allowed.
   */
  Range();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs A reference to the copied handle
   */
  Range(const Range& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the handle to move
   */
  Range(Range&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle A reference to the copied handle
   * @return A reference to this
   */
  Range& operator=(const Range& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the handle to move
   * @return A reference to this handle
   */
  Range& operator=(Range&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~Range();

protected:
  /**
   * @brief Downcasts to a Range handle.
   * If handle is not a Range, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return Range handle or an uninitialized handle
   */
  static Range DownCast(BaseHandle handle);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Internal constructor.
   *
   * @param[in] range Pointer to internal Range
   */
  explicit DALI_INTERNAL Range(Internal::Range* range);
  /// @endcond

public: //Methods
        /**
  * @brief Retrive the start index of range
  * @return the 32-bit unsigned int.
  */
  u_int32_t GetStartIndex() const;

  /**
  * @brief Retrive the end index of range
  * @return the 32-bit unsigned int.
  */
  u_int32_t GetEndIndex() const;

  /**
  * @brief Retrive the number of indices of range (endIndex - startIndex +1)
  * @return the 32-bit unsigned int.
  */
  u_int32_t GetNumberOfIndices() const;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_RANGE_H
