#ifndef DALI_TOOLKIT_TEXT_STRIKETHROUGH_SPAN_H
#define DALI_TOOLKIT_TEXT_STRIKETHROUGH_SPAN_H

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
#include <dali-toolkit/public-api/text/text-enumerations.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class StrikethroughSpan;
}

/**
 * @brief StrikethroughSpan is a handle to an object that specifies the strikethrough properties for range of characters.
 */

class DALI_TOOLKIT_API StrikethroughSpan : public BaseSpan
{
public:
  /**
   * @brief Create an initialized StrikethroughSpan.
   *
   * @return A handle to a newly allocated Dali resource
   */

  static StrikethroughSpan New();
  /**
   * @brief Create an initialized StrikethroughSpan.
   *
   * @param[in] color The color of line.
   * @param[in] height The height of line.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static StrikethroughSpan New(Vector4 color, float height);

  /**
   * @brief Creates an uninitialized StrikethroughSpan handle.
   *
   * Calling member functions with an uninitialized StrikethroughSpan handle is not allowed.
   */
  StrikethroughSpan();

  /**
   * @brief Copy constructor.
   * @param[in] rhs A reference to the copied handle
   */
  StrikethroughSpan(const StrikethroughSpan& rhs);

  /**
   * @brief Move constructor.
   * @param[in] rhs A reference to the handle to move
   */
  StrikethroughSpan(StrikethroughSpan&& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  StrikethroughSpan& operator=(const StrikethroughSpan& rhs);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  StrikethroughSpan& operator=(StrikethroughSpan&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~StrikethroughSpan();

  /**
   * @brief Downcasts to a StrikethroughSpan handle.
   * If handle is not a StrikethroughSpan, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return StrikethroughSpan handle or an uninitialized handle
   */
  static StrikethroughSpan DownCast(BaseHandle handle);

public: //Methods
  /**
   * @brief Retrive the color of line.
   *
   * @return A color value.
   */
  Vector4 GetColor() const;

  /**
   * @brief Retrieve whether the color is defined.
   *
   * @return The return is true if color is defined, otherwise false.
   */
  bool IsColorDefined() const;

  /**
   * @brief Retrive the height of line.
   *
   * @return A height value.
   */
  float GetHeight() const;

  /**
   * @brief Retrieve whether the height is defined.
   *
   * @return The return is true if height is defined, otherwise false.
   */
  bool IsHeightDefined() const;

public: // Not intended for application developers
  /// @cond internal

  /**
   * @brief This constructor is used internally to Create an initialized StrikethroughSpan handle.
   *
   * @param[in] strikethroughSpan Pointer to internal StrikethroughSpan
   */
  explicit DALI_INTERNAL StrikethroughSpan(Internal::StrikethroughSpan* strikethroughSpan);

  /// @endcond
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_STRIKETHROUGH_SPAN_H