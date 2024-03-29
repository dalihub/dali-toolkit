#ifndef DALI_SCENE3D_LOADER_KTX_LOADER_H
#define DALI_SCENE3D_LOADER_KTX_LOADER_H
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
 *
 */

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/environment-map-data.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Loads cube map data texture from a ktx file.
 *
 * @SINCE_2_0.7
 * @param[in] path The file path.
 * @param[out] environmentMapData The data structure with all pixel data objects.
 * @return bool True if the loading is succeded.
 */
bool LoadKtxData(const std::string& path, EnvironmentMapData& environmentMapData);

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_KTX_LOADER_H
