#ifndef DALI_TOOLKIT_TEXT_ABSTRACT_STYLE_CHARACTER_RUN_H
#define DALI_TOOLKIT_TEXT_ABSTRACT_STYLE_CHARACTER_RUN_H

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
#include <dali/public-api/common/type-traits.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-run.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Run of characters with the same style.
 */
struct AbstractStyleCharacterRun
{
  CharacterRun characterRun; ///< The initial character index and the number of characters of the run.

  // Constructors

protected:
  /**
   * @brief Constructor.
   * Default constructor to set the default values
   */
  AbstractStyleCharacterRun()
  : characterRun{}
  {
  }

  /**
   * @brief Constructor.
   * Default constructor to set the default values
   *
   * @param[in] characterRun the character run
   */
  AbstractStyleCharacterRun(const CharacterRun& characterRun)
  : characterRun{characterRun}
  {
  }

public:
  AbstractStyleCharacterRun(const AbstractStyleCharacterRun&) = default;

  AbstractStyleCharacterRun(AbstractStyleCharacterRun&&) noexcept = default;

  // Operators

  AbstractStyleCharacterRun& operator=(const AbstractStyleCharacterRun&) = default;

  AbstractStyleCharacterRun& operator=(AbstractStyleCharacterRun&&) noexcept = default;

  /**
   * @brief Destructor.
   */
  virtual ~AbstractStyleCharacterRun() = default;

  //Methods

  /**
  * @brief Retrive the first index in run.
  * @return the end character index in run.
  */
  CharacterIndex GetStartCharacterIndex() const;

  /**
  * @brief Retrive the number of characters in the run.
  * @return the the number of characters in run.
  */
  Length GetNumberOfCharacters() const;

  /**
  * @brief Calculate the end index in run.
  * @return the end character index in run.
  */
  CharacterIndex GetEndCharacterIndex() const;
};

} // namespace Text

} // namespace Toolkit

// Allow AbstractStyleCharacterRun to be treated as a POD type
template<>
struct TypeTraits<Dali::Toolkit::Text::AbstractStyleCharacterRun> : public Dali::BasicTypes<Dali::Toolkit::Text::AbstractStyleCharacterRun>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ABSTRACT_STYLE_CHARACTER_RUN_H
