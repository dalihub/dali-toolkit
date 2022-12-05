#ifndef DALI_TOOLKIT_TEXT_BACKGROUND_COLOR_SPAN_H
#define DALI_TOOLKIT_TEXT_BACKGROUND_COLOR_SPAN_H

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
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class BackgroundColorSpan;
}

/**
 * @brief BackgroundColorSpan is a handle to an object that specifies the background-color for range of characters.
 */
class DALI_TOOLKIT_API BackgroundColorSpan : public BaseSpan
{
public:
  /**
   * @brief Create an initialized BackgroundColorSpan.
   *
   * @param[in] color The background color.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static BackgroundColorSpan New(const Vector4 &color);

  /**
   * @brief Creates an uninitialized BackgroundColorSpan handle.
   *
   * Calling member functions with an uninitialized BackgroundColorSpan handle is not allowed.
   */
  BackgroundColorSpan();

  /**
   * @brief Copy constructor.
   * @param[in] rhs A reference to the copied handle
   */
  BackgroundColorSpan(const BackgroundColorSpan& rhs);

  /**
   * @brief Move constructor.
   * @param[in] rhs A reference to the handle to move
   */
  BackgroundColorSpan(BackgroundColorSpan&& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  BackgroundColorSpan& operator=(const BackgroundColorSpan& rhs);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  BackgroundColorSpan& operator=(BackgroundColorSpan&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~BackgroundColorSpan();

  /**
   * @brief Downcasts to a BackgroundColorSpan handle.
   * If handle is not a BackgroundColorSpan, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return BackgroundColorSpan handle or an uninitialized handle
   */
  static BackgroundColorSpan DownCast(BaseHandle handle);

public: //Methods
  /**
   * @brief Retrive the background-color.
   *
   * @return A background-color value.
   */
  const Vector4 GetBackgroundColor() const;

  /**
   * @brief Retrieve whether the background-color is defined.
   *
   * @return The return is true if background-color is defined, otherwise false.
   */
  bool IsBackgroundColorDefined() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized BackgroundColorSpan handle.
   *
   * @param[in] colorSpan Pointer to internal BackgroundColorSpan
   */
  explicit DALI_INTERNAL BackgroundColorSpan(Internal::BackgroundColorSpan* colorSpan);
  /// @endcond
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_BACKGROUND_COLOR_SPAN_H
