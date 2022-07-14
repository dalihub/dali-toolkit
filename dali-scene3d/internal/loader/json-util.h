#ifndef DALI_SCENE3D_LOADER_JSON_UTIL_H_
#define DALI_SCENE3D_LOADER_JSON_UTIL_H_
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "dali-scene3d/public-api/loader/index.h"

// EXTERNAL INCLUDES
#include "dali-toolkit/devel-api/builder/tree-node.h"
#include "dali/public-api/animation/time-period.h"
#include "dali/public-api/common/vector-wrapper.h"
#include "dali/public-api/math/vector4.h"
#include "dali/public-api/object/property.h"

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
bool ReadBool(const Toolkit::TreeNode* node, bool& num);

bool ReadInt(const Toolkit::TreeNode* node, int& num);

bool ReadFloat(const Toolkit::TreeNode* node, float& num);

bool ReadIndex(const Toolkit::TreeNode* node, Index& num);

bool ReadBlob(const Toolkit::TreeNode* node, unsigned int& offset, unsigned int& length);

/**
 * @brief Gets the number of numerical element of a JSON array;
 * @return 0 if not an array, otherwise the number of float or integer elements
 *  at the front of the array.
 */
size_t GetNumericalArraySize(const Toolkit::TreeNode* node);

bool ReadVector(const Toolkit::TreeNode* node, float* num, unsigned int size);

bool ReadVector(const Toolkit::TreeNode* node, int* num, unsigned int size);

/**
 * @brief Reads a color.
 * 
 * The node contents could be a vector of 4 floats [r,g,b,a].
 * 
 * @param[in] node The tree node with the color.
 * @param[out] color The RGBA color.
 * 
 * @return true if succedded to read the color.
 */
bool ReadColor(const Toolkit::TreeNode* node, Vector4& color);

bool ReadTimePeriod(const Toolkit::TreeNode* node, TimePeriod& timePeriod);

bool ReadString(const Toolkit::TreeNode* node, std::string& strValue);

bool ReadStringVector(const Toolkit::TreeNode* node, std::vector<std::string>& strvector);

/**
 * @brief Attempts to read a property of the given type from the given JSON node.
 * @return The property value that it could interpret. If unsuccessful, its type will be NONE.
 * @note Currently only numerical types are supported (including boolean).
 */
Property::Value ReadPropertyValue(const Property::Type& propType, const Toolkit::TreeNode& tn);

/**
 * @brief Attempts to read a property, whose type it will attempt to determine from the given
 *  JSON node.
 * @return The property value that it could interpret. If unsuccessful, its type will be NONE.
 * @note Currently only numerical types are supported (including boolean).
 * @note Supports a disambiguation syntax, whereby the type can be specified explicitly:
 *  { "type": "rotation", "value": [...] } .
 * @note: rotation / rectangle / extents type properties must be disambiguated in all circumstances.
 */
Property::Value ReadPropertyValue(const Toolkit::TreeNode& tn);

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif //DALI_SCENE3D_LOADER_JSON_UTIL_H_
