#ifndef DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_SUPPORT_IMPL_H
#define DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_SUPPORT_IMPL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/text-abstraction/icu.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/multi-language-support.h>

namespace Dali
{
namespace TextAbstraction
{
//Forward declaration
class FontClient;
} // namespace TextAbstraction

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
  {
  }

  /**
   * Default destructor.
   */
  ~ValidateFontsPerScript()
  {
  }

  /**
   * @brief Whether the given @p fontId is in the vector of valid fonts.
   *
   * @param[in] fontId The font id.
   *
   * @return @e true if the font is in the vector of valid fonts.
   */
  bool IsValidFont(FontId fontId) const;

  /**
   * @brief Cache the given @p fontId in the vector of valid fonts.
   * @note If cache size is big enough, we might remove some caches.
   *
   * @param[in] fontId The font id.
   */
  void Cache(FontId fontId);

  Vector<FontId> mValidFonts;
};

/**
 * @brief Stores default font ids per script. It can be different sizes for a default font family.
 */
struct DefaultFonts
{
  struct CacheItem
  {
    TextAbstraction::FontDescription description;
    FontId                           fontId;
  };

  /**
   * Default constructor.
   */
  DefaultFonts()
  : mFonts()
  {
  }

  /**
   * Default destructor.
   */
  ~DefaultFonts()
  {
  }

  /**
   * @brief Finds a default font for the given @p size.
   *
   * @param[in] fontClient The font client.
   * @param[in] description The font's description.
   * @param[in] size The given size.
   *
   * @return The font id of a default font for the given @p size. If there isn't any font cached it returns 0.
   */
  FontId FindFont(TextAbstraction::FontClient&            fontClient,
                  const TextAbstraction::FontDescription& description,
                  PointSize26Dot6                         size,
                  Character                               character) const;

  /**
   * @brief Cache a default font for the given @p size.
   * @note If cache size is big enough, we might remove some caches.
   *
   * @param[in] description The font's description.
   * @param[in] fontId The font id.
   */
  void Cache(const TextAbstraction::FontDescription& description, FontId fontId);

  std::vector<CacheItem> mFonts;
};

/**
 * @brief Multi-language support implementation. @see Text::MultilanguageSupport.
 */
class MultilanguageSupport : public BaseObject, public ConnectionTracker
{
public:
  /**
   * Constructor
   */
  MultilanguageSupport(bool connectLocaleChangedSignal = true);

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
  void SetScripts(const Vector<Character>& text,
                  CharacterIndex           startIndex,
                  Length                   numberOfCharacters,
                  Vector<ScriptRun>&       scripts);

  /**
   * @copydoc Dali::MultilanguageSupport::ValidateFonts()
   */
  void ValidateFonts(TextAbstraction::FontClient&            fontClient,
                     const Vector<Character>&                text,
                     const Vector<ScriptRun>&                scripts,
                     const Vector<FontDescriptionRun>&       fontDescriptions,
                     const TextAbstraction::FontDescription& defaultFontDescription,
                     TextAbstraction::PointSize26Dot6        defaultFontPointSize,
                     float                                   fontSizeScale,
                     CharacterIndex                          startIndex,
                     Length                                  numberOfCharacters,
                     Vector<FontRun>&                        fonts,
                     Property::Map*                          variationsMapPtr);

  /**
   * @brief Callback function for when the locale is changed.
   * @param[in] locale The new system locale.
   */
  void OnLocaleChanged(std::string locale);

  /**
   * @copydoc Dali::MultilanguageSupport::GetLocale()
   */
  const std::string& GetLocale();

  /**
   * @copydoc Dali::MultilanguageSupport::SetLocale()
   */
  void SetLocale(const std::string& locale);

  /**
   * @copydoc Dali::MultilanguageSupport::ClearCache()
   */
  void ClearCache();

  /**
   * @copydoc Dali::MultilanguageSupport::IsICULineBreakNeeded()
   */
  bool IsICULineBreakNeeded();

  /**
   * @copydoc Dali::MultilanguageSupport::UpdateICULineBreak()
   */
  void UpdateICULineBreak(const std::string&              text,
                          TextAbstraction::Length         numberOfCharacters,
                          TextAbstraction::LineBreakInfo* breakInfo);

private:
  TextAbstraction::ICU            mICU;                       ///< Handle to the dali ICU.
  Vector<DefaultFonts*>           mDefaultFontPerScriptCache; ///< Caches default fonts for a script.
  Vector<ValidateFontsPerScript*> mValidFontsPerScriptCache;  ///< Caches valid fonts for a script.

  std::string mLocale;
  bool        mIsICUEnabled : 1;
  bool        mIsICULineBreakNeededForLocale : 1;

  //Methods

  /**
   * @brief Add the current script to scripts and create new script.
   *
   * @param[in] requestedScript The script of the new script run.
   * @param[in] isRightToLeft The direction of the new script run.
   * @param[in] addScriptCharactersToNewScript Whether to add the pending characters to the new script run or to the current script run.
   * @param[inout] currentScriptRun The current character script run and it will be updated it to the new script run.
   * @param[inout] numberOfAllScriptCharacters The pending characters.
   * @param[inout] scripts The list of scripts.
   * @param[inout] scriptIndex The current index of scripts.
   *
   */
  void AddCurrentScriptAndCreatNewScript(const Script       requestedScript,
                                         const bool         isRightToLeft,
                                         const bool         addScriptCharactersToNewScript,
                                         ScriptRun&         currentScriptRun,
                                         Length&            numberOfAllScriptCharacters,
                                         Vector<ScriptRun>& scripts,
                                         ScriptRunIndex&    scriptIndex);

  /**
   * @brief Checks if an ICU-based line break update is required for the current locale.
   * @note Use only when there is a change in locale to avoid useless calculations.
   * @return If true, icu-based line breaks are required or possible.
   */
  bool IsICULineBreakNeededForLocale();
};

} // namespace Internal

inline static Internal::MultilanguageSupport& GetImplementation(MultilanguageSupport& multilanguageSupport)
{
  DALI_ASSERT_ALWAYS(multilanguageSupport && "multi-language handle is empty");
  BaseObject& handle = multilanguageSupport.GetBaseObject();
  return static_cast<Internal::MultilanguageSupport&>(handle);
}

inline static const Internal::MultilanguageSupport& GetImplementation(const MultilanguageSupport& multilanguageSupport)
{
  DALI_ASSERT_ALWAYS(multilanguageSupport && "multi-language handle is empty");
  const BaseObject& handle = multilanguageSupport.GetBaseObject();
  return static_cast<const Internal::MultilanguageSupport&>(handle);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_MULTI_LANGUAGE_SUPPORT_IMPL_H
