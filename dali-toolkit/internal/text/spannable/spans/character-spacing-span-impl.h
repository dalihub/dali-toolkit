#ifndef DALI_TOOLKIT_INTERNAL_TEXT_CHARACTER_SPACING_SPAN_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_CHARACTER_SPACING_SPAN_IMPL_H

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
#include <dali-toolkit/devel-api/text/spans/character-spacing-span.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/logical-model-impl.h>
#include <dali-toolkit/internal/text/spannable/spans/base-span-impl.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class CharacterSpacingSpan;
using CharacterSpacingSpanPtr = IntrusivePtr<CharacterSpacingSpan>;

/**
 * @copydoc Dali::Toolkit::Text::CharacterSpacingSpan
 */
class CharacterSpacingSpan : public BaseSpan
{
public:
  /**
   * @brief Creates a new CharacterSpacingSpan object.
   */
  static Dali::Toolkit::Text::CharacterSpacingSpan New(const float& value);

  /**
   * Default Constructor
   */
  CharacterSpacingSpan();

  CharacterSpacingSpan(const CharacterSpacingSpan&) = delete;            ///< Deleted copy constructor
  CharacterSpacingSpan(CharacterSpacingSpan&&)      = delete;            ///< Deleted move constructor
  CharacterSpacingSpan& operator=(const CharacterSpacingSpan&) = delete; ///< Deleted copy assignment operator
  CharacterSpacingSpan& operator=(CharacterSpacingSpan&&) = delete;      ///< Deleted move assignment operator

  /**
   * @brief Destructor
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~CharacterSpacingSpan() override;

public: //Methods
  /**
   * @copydoc Dali::Toolkit::Text::CharacterSpacingSpan::GetCharacterSpacing()
   */
  const float GetCharacterSpacing() const;

  /**
   * @copydoc Dali::Toolkit::Text::CharacterSpacingSpan::IsCharacterSpacingDefined()
   */
  bool IsCharacterSpacingDefined() const;

public: //Methods. Not intended for application developers
  /**
   * @brief Set the CharacterSpacing.
   *
   * @param[in] value The CharacterSpacing.
   */
  void SetCharacterSpacing(const float& value);

public: //Methods for internal only
  /**
   * @copydoc Dali::Toolkit::Text::BaseSpan::CreateStyleCharacterRun
   */
  void CreateStyleCharacterRun(IntrusivePtr<LogicalModel>& logicalModel, const Dali::Toolkit::Text::Range& range) const override;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl{nullptr};

}; // class CharacterSpacingSpan

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::CharacterSpacingSpan& GetImplementation(Dali::Toolkit::Text::CharacterSpacingSpan& characterSpacingSpan)
{
  DALI_ASSERT_ALWAYS(characterSpacingSpan && "CharacterSpacingSpan handle is empty");

  BaseObject& object = characterSpacingSpan.GetBaseObject();

  return static_cast<Internal::CharacterSpacingSpan&>(object);
}

inline const Internal::CharacterSpacingSpan& GetImplementation(const Dali::Toolkit::Text::CharacterSpacingSpan& CharacterSpacingSpan)
{
  DALI_ASSERT_ALWAYS(CharacterSpacingSpan && "CharacterSpacingSpan handle is empty");

  const BaseObject& object = CharacterSpacingSpan.GetBaseObject();

  return static_cast<const Internal::CharacterSpacingSpan&>(object);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_CHARACTER_SPACING_SPAN_IMPL_H