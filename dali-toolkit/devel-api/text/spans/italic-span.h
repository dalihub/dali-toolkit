#ifndef DALI_TOOLKIT_TEXT_ITALICSPAN_H
#define DALI_TOOLKIT_TEXT_ITALICSPAN_H

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
class ItalicSpan;
}

/**
 * @brief ItalicSpan is a handle to an object that specifies the Italic font for range of characters.
 */
class DALI_TOOLKIT_API ItalicSpan : public BaseSpan
{
public:
  /**
   * @brief Create an Initialized ItalicSpan.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static ItalicSpan New();

  /**
   * @brief Creates an uninitialized ItalicSpan handle.
   *
   * Calling member functions with an uninitialized ItalicSpan handle is not allowed.
   */
  ItalicSpan();

  /**
   * @brief Copy constructor.
   * @param[in] rhs A reference to the copied handle.
   */
  ItalicSpan(const ItalicSpan& rhs);

  /**
   * @brief Move constructor.
   * @param rhs A reference to the handle to move.
   */
  ItalicSpan(ItalicSpan&& rhs);

  /**
   * @brief Assignment operator.
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  ItalicSpan& operator=(const ItalicSpan& rhs);

   /**
   * @brief Move assignment operator.
   * @param[in] rhs A reference to the moved handle.
   * @return A reference to this.
   */
  ItalicSpan& operator=(ItalicSpan&& rhs);

  /**
   * @brief Non virtual destructor.
   */
  ~ItalicSpan();

  /**
   * @brief Downcasts to a ItalicSpan handle.
   * If handle is not a ItalicSpan, the returned handle is left uninitialized.
   *
   * @param[in] handle  Handle to an object.
   * @return ItalicSpan handle or an uninitialized handle.
   */
  static ItalicSpan DownCast(BaseHandle handle);

  // Not intended for application developers.
  /**
   * @brief This constructor is used internally to Create an initialized ItalicSpan handle.
   *
   * @param[in] italicSpan Pointer to internal ItalicSpan.
   */
  explicit DALI_INTERNAL ItalicSpan(Internal::ItalicSpan* italicSpan);

};

}// namespace Text

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_TEXT_ITALICSPAN_H