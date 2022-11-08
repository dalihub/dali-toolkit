#ifndef DALI_TOOLKIT_INTERNAL_TEXT_SPANNED_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_SPANNED_IMPL_H

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
#include <dali-toolkit/devel-api/text/spanned.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/string-text/character-sequence-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class CharacterSequence;
class Spanned;

typedef IntrusivePtr<Spanned> SpannedPtr;
typedef const Spanned*        SpannedConstPtr;

/**
 * @copydoc Dali::Toolkit::Text::Spanned
 */
class Spanned : public CharacterSequence
{
public:
  /**
   * This constructor is only used by derived classes.
   */
  Spanned();

  /**
   * @brief Virtual destructor.
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Spanned() = 0;

  // Removed constructors and assignment operators
public:                                        // Constructors
  Spanned(const Spanned&) = delete;            ///< Deleted copy constructor
  Spanned(Spanned&&)      = delete;            ///< Deleted move constructor
  Spanned& operator=(const Spanned&) = delete; ///< Deleted copy assignment operator
  Spanned& operator=(Spanned&&) = delete;      ///< Deleted move assignment operator

public: //Methods
  /**
   * @copydoc Dali::Toolkit::Text::Spanned::GetAllSpans()
   */
  virtual std::vector<Dali::Toolkit::Text::BaseSpan> GetAllSpans() const = 0;

  /**
   * @copydoc Dali::Toolkit::Text::Spanned::RetrieveAllSpansAndRanges()
   */
  virtual void RetrieveAllSpansAndRanges(std::vector<Dali::Toolkit::Text::BaseSpan>& spans, std::vector<Dali::Toolkit::Text::Range>& ranges) const = 0;

}; // class Spanned

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::Spanned& GetImplementation(Dali::Toolkit::Text::Spanned& spanned)
{
  DALI_ASSERT_ALWAYS(spanned && "spanned handle is empty");

  BaseObject& handle = spanned.GetBaseObject();

  return static_cast<Internal::Spanned&>(handle);
}

inline const Internal::Spanned& GetImplementation(const Dali::Toolkit::Text::Spanned& spanned)
{
  DALI_ASSERT_ALWAYS(spanned && "spanned handle is empty");

  const BaseObject& handle = spanned.GetBaseObject();

  return static_cast<const Internal::Spanned&>(handle);
}

} // namespace Text

} // namespace Toolkit

// Allow Spanned to be treated as a POD type
template<>
struct TypeTraits<Dali::Toolkit::Text::Internal::Spanned> : public Dali::BasicTypes<Dali::Toolkit::Text::Internal::Spanned>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_SPANNED_IMPL_H
