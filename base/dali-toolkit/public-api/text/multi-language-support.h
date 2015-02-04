#ifndef __DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_SUPPORT_H__
#define __DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_SUPPORT_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/text/text-definitions.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace Internal DALI_INTERNAL
{

class MultilanguageSupport;

} // Internal

class LogicalModel;

/**
 * @brief Sets the character's scripts to the model and validates the fonts set by the user or assigns default ones.
 */
class MultilanguageSupport : public BaseHandle
{
public:

  /**
   * @brief Create an uninitialized MultilanguageSupport handle.
   */
  MultilanguageSupport();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~MultilanguageSupport();

  /**
   * @brief This constructor is used by MultilanguageSupport::Get().
   *
   * @param[in] implementation A pointer to the internal multi-language support object.
   */
  explicit DALI_INTERNAL MultilanguageSupport( Internal::MultilanguageSupport* implementation );

  /**
   * @brief Retrieve a handle to the MultilanguageSupport instance.
   *
   * @return A handle to the MultilanguageSupport.
   */
  static MultilanguageSupport Get();

  /**
   * @brief Sets the scripts of the whole text.
   *
   * Any script info previously set is removed.
   *
   * Scripts are used to validate and set default fonts and to shape the text in further steps.
   *
   * @pre The @p model needs to have a text set.
   *
   * @param[in,out] model The text's logical model.
   */
  void SetScripts( LogicalModel& model );

  /**
   * @brief Validates the character's font of the whole text.
   *
   * It may update fonts set by application developers.
   *
   * This method ensures all characters are going to be rendered using an appropriate font. Provided a valid font
   * exists in the platform.
   *
   * For those characters with no font set, it sets a default one.
   *
   * If a font has been set by the application developer, this method checks if the font supports the character.
   * If it doesn't, this method replaces it by a default one.
   *
   * @pre The @p model needs to have a text set.
   * @pre The @p model needs to have the scripts set.
   *
   * @param[in,out] model The text's logical model.
   */
  void ValidateFonts( LogicalModel& model );
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_SUPPORT_H__
