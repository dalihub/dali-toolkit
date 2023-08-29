#ifndef DALI_TOOLKIT_NPATCH_HELPER_H
#define DALI_TOOLKIT_NPATCH_HELPER_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/uint-16-pair.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <memory> ///< for std::shared_ptr

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/dali-toolkit-common.h>
#include <dali-toolkit/internal/visuals/npatch-data.h>

namespace Dali
{
namespace Toolkit
{
namespace NPatchHelper
{
/**
 * The list that includes stretch pixel ranges
 */
using StretchRanges = Dali::Vector<Uint16Pair>;

/**
 * @brief Creates a Npatch Geometry object
 *
 * @param[in] gridSize The gridSize for creating a geometry
 * @return The Geometry for NPatch
 */
DALI_TOOLKIT_API Geometry CreateGridGeometry(Uint16Pair gridSize);

/**
 * @brief Creates a geometry with the border only for the grid size to be used by this visuals' shaders
 * e.g. a 5x4 grid would create a geometry that would look like:
 *
 *   ---------------------
 *   |  /|  /|  /|  /|  /|
 *   |/  |/  |/  |/  |/  |
 *   ---------------------
 *   |  /|           |  /|
 *   |/  |           |/  |
 *   -----           -----
 *   |  /|           |  /|
 *   |/  |           |/  |
 *   ---------------------
 *   |  /|  /|  /|  /|  /|
 *   |/  |/  |/  |/  |/  |
 *   ---------------------
 *
 * @param[in] gridSize The grid size of the solid geometry to create
 * @return Returns the created geometry for the grid size
 */
DALI_TOOLKIT_API Geometry CreateBorderGeometry(Uint16Pair gridSize);


/**
 * @brief Registers a properties for Stretch Ranges
 *
 * @param[in,out] renderer The renderer for broken image
 * @param[in] uniformName The name of the uniform
 * @param[in] stretchPixels The stretchable pixels in the cropped image space
 * @param[in] imageExtent The imageExtent
 */
void RegisterStretchProperties(Renderer& renderer, const char* uniformName, const NPatchUtility::StretchRanges& stretchPixels, uint16_t imageExtent);

/**
 * @brief Apply a texture and uniforms
 *
 * @param[in,out] renderer The renderer for broken image
 * @param[in] data The pointer of npatch-data
 */
void ApplyTextureAndUniforms(Renderer& renderer, const std::shared_ptr<const Internal::NPatchData> data);

} // namespace NPatchUtility

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_NPATCH_HELPER_H
