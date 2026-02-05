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

#include <dali-toolkit/internal/styling/default-theme.h>
#include <dali-toolkit/internal/styling/style-manager-impl.h>
#include <dali/devel-api/adaptor-framework/style-monitor.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
DefaultTheme& DefaultTheme::Get()
{
  static DefaultTheme instance;
  return instance;
}

DefaultTheme::DefaultTheme()
: mTextLabelStyle(),
  mTextFieldStyle(),
  mTextEditorStyle()
{
}

void DefaultTheme::Reset()
{
  mTextLabelStyle.Clear();
  mTextFieldStyle.Clear();
  mTextEditorStyle.Clear();
}

void DefaultTheme::ApplyDefaultStyle(Toolkit::TextLabel textLabel)
{
  Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
  if(!styleManager)
  {
    return;
  }

  // Only load if style map is empty (caching optimization)
  if(mTextLabelStyle.Count() == 0)
  {
    mTextLabelStyle = GetImpl(styleManager).GetStyleProperties("TextLabel", textLabel);
  }

  // Apply TextLabel style directly using Property::Index keys
  for(Property::Map::SizeType i = 0; i < mTextLabelStyle.Count(); ++i)
  {
    const auto& keyValuePair = mTextLabelStyle.GetKeyValue(i);
    const Property::Key& key = keyValuePair.first;
    const Property::Value& value = keyValuePair.second;
    textLabel.SetProperty(key.indexKey, value);
  }
}

void DefaultTheme::ApplyDefaultStyle(Toolkit::TextField textField)
{
  Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
  if(!styleManager)
  {
    return;
  }

  if(mTextFieldStyle.Count() == 0)
  {
    mTextFieldStyle = GetImpl(styleManager).GetStyleProperties("TextField", textField);
  }

  // Apply TextField style directly using Property::Index keys
  for(Property::Map::SizeType i = 0; i < mTextFieldStyle.Count(); ++i)
  {
    const auto& keyValuePair = mTextFieldStyle.GetKeyValue(i);
    const Property::Key& key = keyValuePair.first;
    const Property::Value& value = keyValuePair.second;
    textField.SetProperty(key.indexKey, value);
  }
}

void DefaultTheme::ApplyDefaultStyle(Toolkit::TextEditor textEditor)
{
  Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
  if(!styleManager)
  {
    return;
  }

  if(mTextEditorStyle.Count() == 0)
  {
    mTextEditorStyle = GetImpl(styleManager).GetStyleProperties("TextEditor", textEditor);
  }

  // Apply TextEditor style directly using Property::Index keys
  for(Property::Map::SizeType i = 0; i < mTextEditorStyle.Count(); ++i)
  {
    const auto& keyValuePair = mTextEditorStyle.GetKeyValue(i);
    const Property::Key& key = keyValuePair.first;
    const Property::Value& value = keyValuePair.second;
    textEditor.SetProperty(key.indexKey, value);
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
