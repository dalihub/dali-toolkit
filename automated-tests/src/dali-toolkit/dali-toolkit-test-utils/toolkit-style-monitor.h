#ifndef DALI_TOOLKIT_TOOLKIT_STYLE_MONITOR_H
#define DALI_TOOLKIT_TOOLKIT_STYLE_MONITOR_H

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/style-monitor.h>
#include <dali/public-api/adaptor-framework/style-change.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Test
{
namespace StyleMonitor
{
void SetThemeFileOutput(const std::string& name, const std::string& output);
void SetDefaultFontFamily(const std::string& family);
void SetDefaultFontStyle(const std::string& style);
void SetDefaultFontSize(float size);
void SetThemeChangedBeforeAdaptorInit(bool changed);

} // namespace StyleMonitor
} // namespace Test

#endif // DALI_TOOLKIT_TOOLKIT_STYLE_MONITOR_H
