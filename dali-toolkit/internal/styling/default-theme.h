#ifndef DALI_TOOLKIT_INTERNAL_DEFAULT_THEME_H
#define DALI_TOOLKIT_INTERNAL_DEFAULT_THEME_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>
#include <dali-toolkit/public-api/controls/text-controls/text-field.h>
#include <dali-toolkit/public-api/controls/text-controls/text-editor.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class DefaultTheme
{
public:
  static DefaultTheme& Get();
  void ApplyDefaultStyle(Toolkit::TextLabel textLabel);
  void ApplyDefaultStyle(Toolkit::TextField textField);
  void ApplyDefaultStyle(Toolkit::TextEditor textEditor);
  void Reset(); // Reload theme from StyleManager

private:
  DefaultTheme();
  ~DefaultTheme() = default;
  DefaultTheme(const DefaultTheme&) = delete;
  DefaultTheme& operator=(const DefaultTheme&) = delete;

private:
  Property::Map mTextLabelStyle;
  Property::Map mTextFieldStyle;
  Property::Map mTextEditorStyle;
};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_DEFAULT_THEME_H
