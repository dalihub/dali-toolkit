#ifndef DALI_TOOLKIT_TEXT_STRIKETHROUGH_CHARACTER_RUN_H
#define DALI_TOOLKIT_TEXT_STRIKETHROUGH_CHARACTER_RUN_H

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
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/abstract-style-character-run.h>
#include <dali-toolkit/internal/text/strikethrough-style-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Run of strikethrough characters with same properties.
 */
struct StrikethroughCharacterRun : public AbstractStyleCharacterRun
{
  /**
   * Default constructor to set the default values of bitfields
   */
  StrikethroughCharacterRun()
  : AbstractStyleCharacterRun(),
    properties{}
  {
  }

  StrikethroughStyleProperties properties; /// The properties of strikethrough style
};

} // namespace Text

} // namespace Toolkit

// Allow StrikethroughCharacterRun to be treated as a POD type
template<>
struct TypeTraits<Dali::Toolkit::Text::StrikethroughCharacterRun> : public Dali::BasicTypes<Dali::Toolkit::Text::StrikethroughCharacterRun>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_STRIKETHROUGH_CHARACTER_RUN_H
