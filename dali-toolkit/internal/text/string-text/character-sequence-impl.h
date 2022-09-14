#ifndef DALI_TOOLKIT_INTERNAL_TEXT_CHARACTER_SEQUENCE_IMPL_H
#define DALI_TOOLKIT_INTERNAL_TEXT_CHARACTER_SEQUENCE_IMPL_H

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
#include <dali-toolkit/devel-api/text/character-sequence.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/text-definitions.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal
{
class CharacterSequence;

typedef IntrusivePtr<CharacterSequence> CharacterSequencePtr;
typedef const CharacterSequence*        CharacterSequenceConstPtr;

/**
 * @copydoc Dali::Toolkit::Text::CharacterSequence
 */
class CharacterSequence : public BaseObject
{
public:
  /**
   * This constructor is only used by derived classes.
   */
  CharacterSequence();

  /**
   * @brief Virtual destructor.
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~CharacterSequence() = 0;

  // Removed constructors and assignment operators
public:
  CharacterSequence(const CharacterSequence&) = delete;            ///< Deleted copy constructor
  CharacterSequence(CharacterSequence&&)      = delete;            ///< Deleted move constructor
  CharacterSequence& operator=(const CharacterSequence&) = delete; ///< Deleted copy assignment operator
  CharacterSequence& operator=(CharacterSequence&&) = delete;      ///< Deleted move assignment operator

public: //Methods
  /**
   * @copydoc Dali::Toolkit::Text::CharacterSequence::GetCharacters()
   */
  virtual Vector<uint32_t> GetCharacters() const = 0;

  /**
   * @copydoc Dali::Toolkit::Text::CharacterSequence::GetNumberOfCharacters()
   */
  virtual uint32_t GetNumberOfCharacters() const = 0;

  /**
   * @copydoc Dali::Toolkit::Text::CharacterSequence::ToString()
   */
  virtual std::string ToString() const = 0;

}; // class CharacterSequence

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::CharacterSequence& GetImplementation(Dali::Toolkit::Text::CharacterSequence& characterSequence)
{
  DALI_ASSERT_ALWAYS(characterSequence && "characterSequence handle is empty");

  BaseObject& handle = characterSequence.GetBaseObject();

  return static_cast<Internal::CharacterSequence&>(handle);
}

inline const Internal::CharacterSequence& GetImplementation(const Dali::Toolkit::Text::CharacterSequence& characterSequence)
{
  DALI_ASSERT_ALWAYS(characterSequence && "characterSequence handle is empty");

  const BaseObject& handle = characterSequence.GetBaseObject();

  return static_cast<const Internal::CharacterSequence&>(handle);
}

} // namespace Text

} // namespace Toolkit

// Allow AbstractStyleSpan to be treated as a POD type
template<>
struct TypeTraits<Dali::Toolkit::Text::Internal::CharacterSequence> : public Dali::BasicTypes<Dali::Toolkit::Text::Internal::CharacterSequence>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_CHARACTER_SEQUENCE_IMPL_H
