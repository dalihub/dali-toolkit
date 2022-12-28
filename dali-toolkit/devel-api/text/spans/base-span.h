#ifndef DALI_TOOLKIT_TEXT_BASE_SPAN_H
#define DALI_TOOLKIT_TEXT_BASE_SPAN_H

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
#include <dali-toolkit/devel-api/text/spans/span-enumerations.h>
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
class BaseSpan;
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
class DALI_TOOLKIT_API BaseSpan : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized BaseSpan handle.
   *
   * Calling member functions with an uninitialized BaseSpan handle is not allowed.
   */
  BaseSpan();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs A reference to the copied handle
   */
  BaseSpan(const BaseSpan& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the handle to move
   */
  BaseSpan(BaseSpan&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle A reference to the copied handle
   * @return A reference to this
   */
  BaseSpan& operator=(const BaseSpan& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the handle to move
   * @return A reference to this handle
   */
  BaseSpan& operator=(BaseSpan&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~BaseSpan();

  /**
   * @brief Retrive the span type.
   *
   * @return A span type value.
   */
  const Dali::Toolkit::Text::SpanType::Value GetSpanType() const;

  /**
   * @brief Downcasts to a BaseSpan handle.
   * If handle is not a BaseSpan, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return BaseSpan handle or an uninitialized handle
   */
  static BaseSpan DownCast(BaseHandle handle);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Internal constructor.
   *
   * @param[in] baseSpan Pointer to internal BaseSpan
   */
  explicit DALI_INTERNAL BaseSpan(Internal::BaseSpan* baseSpan);
  /// @endcond
};

} // namespace Text

} // namespace Toolkit

// Allow BaseSpan to be treated as a POD type
template<>
struct TypeTraits<Dali::Toolkit::Text::BaseSpan> : public BasicTypes<Dali::Toolkit::Text::BaseSpan>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_BASE_SPAN_H
