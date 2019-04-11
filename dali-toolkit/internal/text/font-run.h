#ifndef DALI_TOOLKIT_TEXT_FONT_RUN_H
#define DALI_TOOLKIT_TEXT_FONT_RUN_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-run.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

/**
 * @brief Run of characters with the same font.
 */
struct FontRun
{
  CharacterRun characterRun;       ///< The initial character index and the number of characters of the run.
  FontId       fontId;             ///< Font id of the run.
  bool         isItalicRequired:1; ///< Whether the italic style is required.
  bool         isBoldRequired:1;   ///< Whether the bold style is required.
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_FONT_RUN_H
