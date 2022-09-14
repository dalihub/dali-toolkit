#ifndef DALI_TOOLKIT_TEXT_FOREGROUND_COLOR_SPAN_H
#define DALI_TOOLKIT_TEXT_FOREGROUND_COLOR_SPAN_H

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
class ForegroundColorSpan;
}

/**
 * @brief ForegroundColorSpan is a handle to an object that specifies the foreground-color for range of characters.
 */
class DALI_TOOLKIT_API ForegroundColorSpan : public BaseSpan
{
public:
  /**
   * @brief Create an initialized ForegroundColorSpan.
   *
   * @param[in] color The foreground color.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static ForegroundColorSpan New(Vector4 color);

  /**
   * @brief Creates an uninitialized ForegroundColorSpan handle.
   *
   * Calling member functions with an uninitialized ForegroundColorSpan handle is not allowed.
   */
  ForegroundColorSpan();

  /**
   * @brief Copy constructor.
   * @param[in] rhs A reference to the copied handle
   */
  ForegroundColorSpan(const ForegroundColorSpan& rhs);

  /**
   * @brief Move constructor.
   * @param[in] rhs A reference to the handle to move
   */
  ForegroundColorSpan(ForegroundColorSpan&& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  ForegroundColorSpan& operator=(const ForegroundColorSpan& rhs);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  ForegroundColorSpan& operator=(ForegroundColorSpan&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~ForegroundColorSpan();

  /**
   * @brief Downcasts to a ForegroundColorSpan handle.
   * If handle is not a ForegroundColorSpan, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return ForegroundColorSpan handle or an uninitialized handle
   */
  static ForegroundColorSpan DownCast(BaseHandle handle);

public: //Methods
  /**
   * @brief Retrive the foreground-color.
   *
   * @return A foreground-color value.
   */
  const Vector4 GetForegroundColor() const;

  /**
   * @brief Retrieve whether the foreground-color is defined.
   *
   * @return The return is true if foreground-color is defined, otherwise false.
   */
  bool IsForegroundColorDefined() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized ForegroundColorSpan handle.
   *
   * @param[in] colorSpan Pointer to internal ForegroundColorSpan
   */
  explicit DALI_INTERNAL ForegroundColorSpan(Internal::ForegroundColorSpan* colorSpan);
  /// @endcond
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_FOREGROUND_COLOR_SPAN_H
