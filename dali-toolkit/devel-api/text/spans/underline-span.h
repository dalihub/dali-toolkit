#ifndef DALI_TOOLKIT_TEXT_UNDERLINE_SPAN_H
#define DALI_TOOLKIT_TEXT_UNDERLINE_SPAN_H

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
class UnderlineSpan;
}

/**
 * @brief UnderlineSpan is a handle to an object that specifies the underline properties for range of characters.
 */
class DALI_TOOLKIT_API UnderlineSpan : public BaseSpan
{
public:
  /**
   * @brief Create an initialized UnderlineSpan.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static UnderlineSpan New();

  /**
   * @brief Create an initialized UnderlineSpan.
   *
   * @param[in] color The color of line.
   * @param[in] height The height of line.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static UnderlineSpan NewSolid(Vector4 color, float height);

  /**
   * @brief Create an initialized UnderlineSpan.
   *
   * @param[in] color The color of line.
   * @param[in] height The height of line.
   * @param[in] dashGap The dash-gap of line.
   * @param[in] dashWidth The dash-width of line.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static UnderlineSpan NewDashed(Vector4 color, float height, float dashGap, float dashWidth);

  /**
   * @brief Create an initialized UnderlineSpan.
   *
   * @param[in] color The color of line.
   * @param[in] height The height of line.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static UnderlineSpan NewDouble(Vector4 color, float height);

  /**
   * @brief Creates an uninitialized UnderlineSpan handle.
   *
   * Calling member functions with an uninitialized UnderlineSpan handle is not allowed.
   */
  UnderlineSpan();

  /**
   * @brief Copy constructor.
   * @param[in] rhs A reference to the copied handle
   */
  UnderlineSpan(const UnderlineSpan& rhs);

  /**
   * @brief Move constructor.
   * @param[in] rhs A reference to the handle to move
   */
  UnderlineSpan(UnderlineSpan&& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  UnderlineSpan& operator=(const UnderlineSpan& rhs);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  UnderlineSpan& operator=(UnderlineSpan&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~UnderlineSpan();

  /**
   * @brief Downcasts to a UnderlineSpan handle.
   * If handle is not a UnderlineSpan, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return UnderlineSpan handle or an uninitialized handle
   */
  static UnderlineSpan DownCast(BaseHandle handle);

public: //Methods
  /**
   * @brief Retrive the type of line.
   *
   * @return A type value.
   */
  Text::Underline::Type GetType() const;

  /**
   * @brief Retrieve whether the type is defined.
   *
   * @return The return is true if type is defined, otherwise false.
   */
  bool IsTypeDefined() const;

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

  /**
   * @brief Retrive the dash-gap of line.
   *
   * @return A dash-gap value.
   */
  float GetDashGap() const;

  /**
   * @brief Retrieve whether the dash-gap is defined.
   *
   * @return The return is true if dash-gap is defined, otherwise false.
   */
  bool IsDashGapDefined() const;

  /**
   * @brief Retrive the dash-width of line.
   *
   * @return A dash-width value.
   */
  float GetDashWidth() const;

  /**
   * @brief Retrieve whether the dash-width is defined.
   *
   * @return The return is true if dash-width is defined, otherwise false.
   */
  bool IsDashWidthDefined() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized UnderlineSpan handle.
   *
   * @param[in] underlineSpan Pointer to internal UnderlineSpan
   */
  explicit DALI_INTERNAL UnderlineSpan(Internal::UnderlineSpan* underlineSpan);
  /// @endcond
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_UNDERLINE_SPAN_H
