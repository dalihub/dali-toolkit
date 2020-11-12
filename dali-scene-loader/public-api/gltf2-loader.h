#ifndef DALI_SCENE_LOADER_GLTF2_LOADER_H
#define DALI_SCENE_LOADER_GLTF2_LOADER_H
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

 // INTERNAL INCLUDES
#include "dali-scene-loader/public-api/api.h"

// EXTERNAL INCLUDES
#include <string>

namespace Dali
{
namespace SceneLoader
{
struct CameraParameters;
struct LoadResult;
class ShaderDefinitionFactory;

/**
 * @brief Loads the scene from the glTF file located at @a url, storing the results in @a params.
 * @note Will throw std::runtime_error for JSON entities with types mismatching expectations, carrying
 *  invalid values, or I/O errors.
 */
DALI_SCENE_LOADER_API void LoadGltfScene(const std::string& url, ShaderDefinitionFactory& shaderFactory, LoadResult& params);

}
}

#endif //DALI_SCENE_LOADER_GLTF2_LOADER_H
