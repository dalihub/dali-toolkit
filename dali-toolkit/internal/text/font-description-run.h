#ifndef DALI_TOOLKIT_TEXT_FONT_DESCRIPTION_RUN_H
#define DALI_TOOLKIT_TEXT_FONT_DESCRIPTION_RUN_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/text-abstraction/font-list.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-run.h>
#include <dali-toolkit/internal/text/text-definitions.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
/**
 * @brief Run of characters with the same font.
 */
struct FontDescriptionRun
{
  /**
   * Default constructor to set the default values of bitfields
   */
  FontDescriptionRun()
  : characterRun{},
    familyName{nullptr},
    familyLength{0u},
    weight{FontWeight::NONE},
    width{FontWidth::NONE},
    slant{FontSlant::NONE},
    size{0u},
    familyDefined{false},
    weightDefined{false},
    widthDefined{false},
    slantDefined{false},
    sizeDefined{false}
  {
  }

  FontDescriptionRun(const CharacterRun& characterRun,
                     char*               familyName,
                     Length              familyLength,
                     FontWeight          weight,
                     FontWidth           width,
                     FontSlant           slant,
                     PointSize26Dot6     size,
                     bool                familyDefined,
                     bool                weightDefined,
                     bool                widthDefined,
                     bool                slantDefined,
                     bool                sizeDefined)
  : characterRun{characterRun},
    familyName{familyName},
    familyLength{familyLength},
    weight{weight},
    width{width},
    slant{slant},
    size{size},
    familyDefined{familyDefined},
    weightDefined{weightDefined},
    widthDefined{widthDefined},
    slantDefined{slantDefined},
    sizeDefined{sizeDefined}
  {
  }

  CharacterRun    characterRun; ///< The initial character index and the number of characters of the run.
  char*           familyName;   ///< The font's family name.
  Length          familyLength; ///< The length of the font's family name.
  FontWeight      weight;       ///< The font's weight.
  FontWidth       width;        ///< The font's width.
  FontSlant       slant;        ///< The font's slant.
  PointSize26Dot6 size;         ///< The font's size.

  bool familyDefined : 1; ///< Whether the font's family is defined.
  bool weightDefined : 1; ///< Whether the font's weight is defined.
  bool widthDefined : 1;  ///< Whether the font's width is defined.
  bool slantDefined : 1;  ///< Whether the font's slant is defined.
  bool sizeDefined : 1;   ///< Whether the font's size is defined.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_FONT_DESCRIPTION_RUN_H
