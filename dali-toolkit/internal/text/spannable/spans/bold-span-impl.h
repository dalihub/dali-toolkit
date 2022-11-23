#ifndef DALI_TOOLKIT_INTERNAL_TEXT_BOLDSPAN_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_BOLDSPAN_IMPL_H

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
#include <dali-toolkit/devel-api/text/spans/bold-span.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spans/base-span-impl.h>
#include <dali-toolkit/internal/text/logical-model-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class BoldSpan;
using BoldSpanPtr = IntrusivePtr<BoldSpan>;

/**
 * @copydoc Dali::Toolkit::Text::BoldSpan
 */
class BoldSpan : public BaseSpan
{
public:
  /**
   * @brief Creates a new BoldSpan object.
   *
   * @return Dali::Toolkit::Text::BoldSpan
   */
  static Dali::Toolkit::Text::BoldSpan New();

  /**
   * @brief Construct a new Bold Span object
   *
   */
  BoldSpan();

  ///< Deleted copy constructor
  BoldSpan(const BoldSpan& rhs) = delete;

  ///< Deleted move constructor
  BoldSpan(BoldSpan&& rhs) = delete;

  ///< Deleted copy assignment operator
  BoldSpan& operator=(const BoldSpan& rhs) = delete;

  ///< Deleted move assignment operator
  BoldSpan& operator=(BoldSpan&& rhs) = delete;

  /**
   * @brief Destructor
   * A reference counted object may only be deleted by calling Unreference().
   */
  ~BoldSpan() override;

  /**
   * @copydoc Dali::Toolkit::Text::BaseSpan::CreateStyleCharacterRun
   */
  void CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const override;

}; // class BoldSpan

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::BoldSpan& GetImplementation(Dali::Toolkit::Text::BoldSpan& boldSpan)
{
  DALI_ASSERT_ALWAYS(boldSpan && "boldSpan handle is empty");

  BaseObject& object = boldSpan.GetBaseObject();

  return static_cast<Internal::BoldSpan&>(object);
}

inline const Internal::BoldSpan& GetImplementation(const Dali::Toolkit::Text::BoldSpan& boldSpan)
{
  DALI_ASSERT_ALWAYS(boldSpan && "boldSpan handle is empty");

  const BaseObject& object = boldSpan.GetBaseObject();

  return static_cast<const Internal::BoldSpan&>(object);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif //DALI_TOOLKIT_INTERNAL_TEXT_BOLDSPAN_IMPL_H