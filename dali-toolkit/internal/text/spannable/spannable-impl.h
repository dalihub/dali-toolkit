#ifndef DALI_TOOLKIT_INTERNAL_TEXT_SPANNABLE_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_SPANNABLE_IMPL_H

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
#include <dali-toolkit/devel-api/text/spannable.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/spanned-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class Spannable;

typedef IntrusivePtr<Spannable> SpannablePtr;
typedef const Spannable*        SpannableConstPtr;

/**
 * @copydoc Dali::Toolkit::Text::Spannable
 */
class Spannable : public Spanned
{
public:
  /**
   * This constructor is only used by derived classes.
   */
  Spannable();

  /**
   * @brief Virtual destructor.
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Spannable() = 0;

  // Removed constructors and assignment operators
public:
  Spannable(const Spannable&) = delete;            ///< Deleted copy constructor
  Spannable(Spannable&&)      = delete;            ///< Deleted move constructor
  Spannable& operator=(const Spannable&) = delete; ///< Deleted copy assignment operator
  Spannable& operator=(Spannable&&) = delete;      ///< Deleted move assignment operator

public: //Methods
  /**
   * @copydoc Dali::Toolkit::Text::Spannable::AttachSpan()
   */
  virtual bool AttachSpan(const Dali::Toolkit::Text::BaseSpan& styleSpan, const Dali::Toolkit::Text::Range& range) = 0;

  /**
   * @copydoc Dali::Toolkit::Text::Spannable::DetachSpan()
   */
  virtual bool DetachSpan(const Dali::Toolkit::Text::BaseSpan& styleSpan) = 0;

}; // class Spannable

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::Spannable& GetImplementation(Dali::Toolkit::Text::Spannable& spannable)
{
  DALI_ASSERT_ALWAYS(spannable && "spannable handle is empty");

  BaseObject& handle = spannable.GetBaseObject();

  return static_cast<Internal::Spannable&>(handle);
}

inline const Internal::Spannable& GetImplementation(const Dali::Toolkit::Text::Spannable& spannable)
{
  DALI_ASSERT_ALWAYS(spannable && "spannable handle is empty");

  const BaseObject& handle = spannable.GetBaseObject();

  return static_cast<const Internal::Spannable&>(handle);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_SPANNABLE_IMPL_H
