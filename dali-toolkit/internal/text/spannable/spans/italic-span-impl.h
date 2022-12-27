#ifndef DALI_TOOLKIT_INTERNAL_TEXT_ITALIC_SPAN_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_ITALIC_SPAN_IMPL_H

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
#include <dali-toolkit/devel-api/text/spans/italic-span.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/spannable/spans/base-span-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class ItalicSpan;
using ItalicSpanPtr = IntrusivePtr<ItalicSpan>;

/**
 * @copydoc Dali::Toolkit::Text::ItalicSpan
 */
class ItalicSpan : public BaseSpan
{
public:
  /**
   * @brief Creates a new ItalicSpan object.
   */
  static Dali::Toolkit::Text::ItalicSpan New();

  /**
   * Default Constructor
   */
  ItalicSpan();

  ///< Deleted copy constructor
  ItalicSpan(const ItalicSpan&) = delete;

  ///< Deleted move constructor
  ItalicSpan(ItalicSpan&&) = delete;

  ///< Deleted copy assignment operator
  ItalicSpan& operator=(const ItalicSpan&) = delete;

  ///< Deleted move assignment operator
  ItalicSpan& operator=(ItalicSpan&&) = delete;

  /**
   * @brief Destructor.
   *
   * A reference counted object may only be deleted by calling Unreference().
   */
  ~ItalicSpan() override;

  //Methods for internal only
  /**
   * @copydoc Dali::Toolkit::Text::BaseSpan::CreateStyleCharacterRun
   */
  void CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const override;

}; // class ItalicSpan

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ItalicSpan& GetImplementation(Dali::Toolkit::Text::ItalicSpan& italicSpan)
{
  DALI_ASSERT_ALWAYS(italicSpan && "italicSpan handle is empty");

  BaseObject& object = italicSpan.GetBaseObject();

  return static_cast<Internal::ItalicSpan&>(object);
}

inline const Internal::ItalicSpan& GetImplementation(const Dali::Toolkit::Text::ItalicSpan& italicSpan)
{
  DALI_ASSERT_ALWAYS(italicSpan && "italicSpan handle is empty");

  const BaseObject& object = italicSpan.GetBaseObject();

  return static_cast<const Internal::ItalicSpan&>(object);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali


#endif // DALI_TOOLKIT_INTERNAL_TEXT_ITALIC_SPAN_IMPL_H