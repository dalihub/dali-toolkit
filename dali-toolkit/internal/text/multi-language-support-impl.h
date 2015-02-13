#ifndef __DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_SUPPORT_IMPL_H__
#define __DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_SUPPORT_IMPL_H__

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
#include <dali-toolkit/public-api/text/multi-language-support.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace Internal
{

/**
 * @brief Stores valid font ids per script.
 */
struct ValidateFontsPerScript
{
  /**
   * Default constructor.
   */
  ValidateFontsPerScript()
  : mValidFonts()
  {}

  /**
   * Default destructor.
   */
  ~ValidateFontsPerScript()
  {}

  /**
   * @brief Whether the given @p fontId is in the vector of valid fonts.
   *
   * @param[in] fontId The font id.
   *
   * @return @e true if the font is in the vector of valid fonts.
   */
  bool FindValidFont( FontId fontId ) const;

  Vector<FontId> mValidFonts;
};

/**
 * @brief Multi-language support implementation. @see Text::MultilanguageSupport.
 */
class MultilanguageSupport : public BaseObject
{
public:

  /**
   * Constructor
   */
  MultilanguageSupport();

  /**
   * Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~MultilanguageSupport();

  /**
   * @copydoc Dali::MultilanguageSupport::Get()
   */
  static Text::MultilanguageSupport Get();

  /**
   * @copydoc Dali::MultilanguageSupport::SetScripts()
   */
  void SetScripts( LogicalModel& model );

  /**
   * @copydoc Dali::MultilanguageSupport::ValidateFonts()
   */
  void ValidateFonts( LogicalModel& model );

private:
  Vector<FontId>                  mDefaultFontPerScriptCache; ///< Caches the default font for a script.
  Vector<ValidateFontsPerScript*> mValidFontsPerScriptCache;  ///< Caches valid fonts for a script.
};

} // namespace Internal

inline static Internal::MultilanguageSupport& GetImplementation( MultilanguageSupport& multilanguageSupport )
{
  DALI_ASSERT_ALWAYS( multilanguageSupport && "multi-language handle is empty" );
  BaseObject& handle = multilanguageSupport.GetBaseObject();
  return static_cast<Internal::MultilanguageSupport&>( handle );
}

inline static const Internal::MultilanguageSupport& GetImplementation( const MultilanguageSupport& multilanguageSupport )
{
  DALI_ASSERT_ALWAYS( multilanguageSupport && "multi-language handle is empty" );
  const BaseObject& handle = multilanguageSupport.GetBaseObject();
  return static_cast<const Internal::MultilanguageSupport&>( handle );
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_SUPPORT_IMPL_H__
