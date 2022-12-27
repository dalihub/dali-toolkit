#ifndef DALI_TOOLKIT_TEXT_BOLD_SPAN_H
#define DALI_TOOLKIT_TEXT_BOLD_SPAN_H

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
class BoldSpan;
}

/**
 * @brief BoldSpan is a handle to an object that specifies the Bold for range of characters.
 */
class DALI_TOOLKIT_API BoldSpan : public BaseSpan
{
public:
  /**
   * @brief Create an initialized BoldSpan.
   *
   * @return A handle to newly allocated Dali resource.
   */
  static BoldSpan New();

  /**
   * @brief Creates an uninitialized BoldSpan handle.
   *
   * Calling member functions with an uninitialized BoldSpan handle is not allowed.
   */
  BoldSpan();

  /**
   * @brief Copy constructor.
   * @param[in] rhs A refrence to the copied handle.
   */
  BoldSpan(const BoldSpan& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A refrence to the move handle.
   */
  BoldSpan(BoldSpan&& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] rhs A reference to the copied handle.
   * @return A refrence to this.
   */
  BoldSpan& operator=(const BoldSpan& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] rhs rhs A reference to the move handle.
   * @return A refrence to this.
   */
  BoldSpan& operator=(BoldSpan&& rhs);

  /**
   * @brief Non virtual destructor.
   *
   */
  ~BoldSpan();

  /**
   * @brief Downcasts to a BoldSpan handle.
   * If handle is not a BoldSpan, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object.
   * @return BoldSpan handle or an uninitialized handle
   */
  static BoldSpan DownCast(BaseHandle handle);

  // Not intended for application developers
  /**
   * @brief This constructor is used internally to create an initialized BoldSpan handle.
   * @param[in] boldSpan Pointer to internal BoldSpan.
   */
  explicit DALI_INTERNAL BoldSpan(Internal::BoldSpan* boldSpan);
};

}// namespace Text

}// namespace Toolkit

}// namespace Dali

#endif // DALI_TOOLKIT_TEXT_BOLDSPAN_H
