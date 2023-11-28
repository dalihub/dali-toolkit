#ifndef DALI_TOOLKIT_IMAGE_VISUAL_SHADER_DEBUG_H
#define DALI_TOOLKIT_IMAGE_VISUAL_SHADER_DEBUG_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <string>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace ImageVisualShaderDebug
{
/**
 * @brief Check whether we need to use debug option for image visual.
 *
 * @return True if ImageVisualShader relative environment on. False otherwise.
 */
bool DebugImageVisualShaderEnabled();

/**
 * @brief Apply fragment shader use debug script.
 *
 * @param[in, out] fragmentShader Shader code to apply debug script.
 */
void ApplyImageVisualShaderDebugScriptCode(std::string& fragmentShader);

} // namespace ImageVisualShaderDebug

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_IMAGE_VISUAL_SHADER_DEBUG_H
