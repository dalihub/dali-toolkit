#ifndef DALI_SCENE3D_LOADER_LOAD_SCENE_METADATA_H_
#define DALI_SCENE3D_LOADER_LOAD_SCENE_METADATA_H_
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

// EXTERNAL INCLUDES
#include <dali/public-api/images/image-operations.h>
#include <string>
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Defines the matadata for an image.
 */
struct ImageMetadata
{
  ImageDimensions    mMinSize{};                                   ///< The minimum required image size for rendering
  SamplingMode::Type mSamplingMode{SamplingMode::BOX_THEN_LINEAR}; ///< The sampling mode used to resize the image
};

/**
 * @brief Stores the metadata for a scene.
 *
 * @note The supported format is the following:
 *  {
 *    "images": (optional) [ {
 *      "uri": name of the image file,
 *      "minWidth": minimum width required for rendering,
 *      "minHeight": minimum height required for rendering.
 *      "samplingMode": sampling mode used to resize the image (@see Dali::SamplingMode). (optional)
 *    } ]
 *  }
 */
struct SceneMetadata
{
  std::unordered_map<std::string, ImageMetadata> mImageMetadata{}; ///< The container of image metadata (i.e. the pairs of image URI and image metadata)
};

/**
 * @brief Attempts to load and process a json file specifying scene metadata
 * for pre-processing the scene resources: e.g. the minimum size required for rendering images, etc.
 * @note Will fail quietly if the file is not present.
 */
DALI_SCENE3D_API void LoadSceneMetadata(const std::string& url, SceneMetadata& sceneMetadata);

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_LOAD_SCENE_METADATA_H_
