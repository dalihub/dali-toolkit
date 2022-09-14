#ifndef DALI_TOOLKIT_INTERNAL_TEXT_SPANNABLE_STRING_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_SPANNABLE_STRING_IMPL_H

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
#include <dali-toolkit/devel-api/text/spannable-string.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/spannable/span-ranges-container-impl.h>
#include <dali-toolkit/internal/text/spannable/spannable-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class SpannableString;

typedef IntrusivePtr<SpannableString> SpannableStringPtr;
typedef const SpannableString*        SpannableStringConstPtr;

/**
 * @copydoc Dali::Toolkit::Text::SpannableString
 */
class SpannableString : public Spannable
{
public:
  /**
   * @brief Creates a new SpannableString.
   *
   * @return A public handle to the newly allocated SpannableString.
   */
  static Dali::Toolkit::Text::SpannableString New(const std::string& text);

  /**
   * @brief Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~SpannableString() override;

protected:
  /**
   * @brief Constructs a new SpannableString.
   */
  SpannableString(const std::string& text);

  // Removed constructors and assignment operators
public:
  SpannableString(const SpannableString&) = delete;            ///< Deleted copy constructor
  SpannableString(SpannableString&&)      = delete;            ///< Deleted move constructor
  SpannableString& operator=(const SpannableString&) = delete; ///< Deleted copy assignment operator
  SpannableString& operator=(SpannableString&&) = delete;      ///< Deleted move assignment operator

public:
  //Methods from CharacterSequence
  /**
   * @copydoc Dali::Toolkit::Text::CharacterSequence::GetCharacters()
   */
  Vector<uint32_t> GetCharacters() const override;

  /**
   * @copydoc Dali::Toolkit::Text::CharacterSequence::GetNumberOfCharacters()
   */
  uint32_t GetNumberOfCharacters() const override;

  /**
   * @copydoc Dali::Toolkit::Text::CharacterSequence::ToString()
   */
  std::string ToString() const override;

  //Methods from Spannable
  /**
   * @copydoc Dali::Toolkit::Text::Spannable::AttachSpan()
   */
  bool AttachSpan(const Dali::Toolkit::Text::BaseSpan& styleSpan, const Dali::Toolkit::Text::Range& range);

  /**
   * @copydoc Dali::Toolkit::Text::Spannable::DetachSpan()
   */
  bool DetachSpan(const Dali::Toolkit::Text::BaseSpan& styleSpan);

  //Methods from Spanned
  /**
   * @copydoc Dali::Toolkit::Text::Spanned::GetAllSpans()
   */
  std::vector<Dali::Toolkit::Text::BaseSpan> GetAllSpans() const;

  /**
   * @copydoc Dali::Toolkit::Text::Spanned::RetrieveAllSpansAndRanges()
   */
  void RetrieveAllSpansAndRanges(std::vector<Dali::Toolkit::Text::BaseSpan>& spans, std::vector<Dali::Toolkit::Text::Range>& ranges) const;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};

}; // class SpannableString

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::SpannableString& GetImplementation(Dali::Toolkit::Text::SpannableString& spannableString)
{
  DALI_ASSERT_ALWAYS(spannableString && "spannableString handle is empty");

  BaseObject& handle = spannableString.GetBaseObject();

  return static_cast<Internal::SpannableString&>(handle);
}

inline const Internal::SpannableString& GetImplementation(const Dali::Toolkit::Text::SpannableString& spannableString)
{
  DALI_ASSERT_ALWAYS(spannableString && "spannableString handle is empty");

  const BaseObject& handle = spannableString.GetBaseObject();

  return static_cast<const Internal::SpannableString&>(handle);
}

} // namespace Text

} // namespace Toolkit

// Allow SpannableString to be treated as a POD type
template<>
struct TypeTraits<Dali::Toolkit::Text::Internal::SpannableString> : public Dali::BasicTypes<Dali::Toolkit::Text::Internal::SpannableString>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_SPANNABLE_STRING_IMPL_H
