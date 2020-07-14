#ifndef DALI_TOOLKIT_RENDERING_BACKEND_DEVEL_H
#define DALI_TOOLKIT_RENDERING_BACKEND_DEVEL_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

namespace Dali
{

namespace Toolkit
{
/**
 * @addtogroup dali_toolkit_controls_text_controls
 * @{
 */

namespace DevelText
{

/**
 * @brief Enumeration for the type of text renderer required.
 */
enum RenderingType
{
  RENDERING_SHARED_ATLAS, ///< A bitmap-based solution where renderers can share a texture atlas
  RENDERING_VECTOR_BASED  ///< A solution where glyphs are stored as vectors (scalable). Requires highp shader support. @SINCE_1_1.31
};

const unsigned int DEFAULT_RENDERING_BACKEND = RENDERING_SHARED_ATLAS;

} // namespace DevelText

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_RENDERING_BACKEND_DEVEL_H
