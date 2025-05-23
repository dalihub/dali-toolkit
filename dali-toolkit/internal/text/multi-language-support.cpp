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

// FILE HEADER
#include <dali-toolkit/internal/text/multi-language-support.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/multi-language-support-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
MultilanguageSupport::MultilanguageSupport()
{
}

MultilanguageSupport::~MultilanguageSupport()
{
}

MultilanguageSupport::MultilanguageSupport(Internal::MultilanguageSupport* implementation)
: BaseHandle(implementation)
{
}

MultilanguageSupport MultilanguageSupport::Get()
{
  return Internal::MultilanguageSupport::Get();
}

MultilanguageSupport MultilanguageSupport::New(bool connectLocaleChangedSignal)
{
  auto multilanguageSupportImpl = new Internal::MultilanguageSupport(connectLocaleChangedSignal);

  return MultilanguageSupport(multilanguageSupportImpl);
}

void MultilanguageSupport::SetScripts(const Vector<Character>& text,
                                      CharacterIndex           startIndex,
                                      Length                   numberOfCharacters,
                                      Vector<ScriptRun>&       scripts)
{
  GetImplementation(*this).SetScripts(text,
                                      startIndex,
                                      numberOfCharacters,
                                      scripts);
}

void MultilanguageSupport::ValidateFonts(TextAbstraction::FontClient&            fontClient,
                                         const Vector<Character>&                text,
                                         const Vector<ScriptRun>&                scripts,
                                         const Vector<FontDescriptionRun>&       fontDescriptions,
                                         const TextAbstraction::FontDescription& defaultFontDescription,
                                         TextAbstraction::PointSize26Dot6        defaultFontPointSize,
                                         float                                   fontSizeScale,
                                         CharacterIndex                          startIndex,
                                         Length                                  numberOfCharacters,
                                         Vector<FontRun>&                        fonts,
                                         Property::Map*                          variationsMapPtr)
{
  GetImplementation(*this).ValidateFonts(fontClient,
                                         text,
                                         scripts,
                                         fontDescriptions,
                                         defaultFontDescription,
                                         defaultFontPointSize,
                                         fontSizeScale,
                                         startIndex,
                                         numberOfCharacters,
                                         fonts,
                                         variationsMapPtr);
}

const std::string& MultilanguageSupport::GetLocale()
{
  return GetImplementation(*this).GetLocale();
}

void MultilanguageSupport::SetLocale(const std::string& locale)
{
  GetImplementation(*this).SetLocale(locale);
}

void MultilanguageSupport::ClearCache()
{
  GetImplementation(*this).ClearCache();
}

bool MultilanguageSupport::IsICULineBreakNeeded()
{
  return GetImplementation(*this).IsICULineBreakNeeded();
}

void MultilanguageSupport::UpdateICULineBreak(const std::string&              text,
                                              TextAbstraction::Length         numberOfCharacters,
                                              TextAbstraction::LineBreakInfo* breakInfo)
{
  GetImplementation(*this).UpdateICULineBreak(text, numberOfCharacters, breakInfo);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
