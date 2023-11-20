#ifndef DALI_SCENE3D_LOADER_FACIAL_ANIMATION_LOADER_H
#define DALI_SCENE3D_LOADER_FACIAL_ANIMATION_LOADER_H

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
#include <dali-scene3d/public-api/loader/animation-definition.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Loads a facial animation encoded in a json file in the given @p url.
 *
 * Throws a DaliException on error.
 *
 * @SINCE_2_1.30
 * @param[in] url The url of the file containing the facial animation.
 *
 * @return An animation definition.
 */
DALI_SCENE3D_API AnimationDefinition LoadFacialAnimation(const std::string& url);

/**
 * @brief Loads a facial animation encoded in a json file in the given data stream.
 *
 * Throws a DaliException on error.
 *
 * @SINCE_2_2.23
 * @param[in] rawBuffer The raw buffer containing the facial animation.
 * @param[in] rawBufferLength The length of raw buffer.
 *
 * @return An animation definition.
 */
DALI_SCENE3D_API AnimationDefinition LoadFacialAnimationFromBuffer(const uint8_t* rawBuffer, int rawBufferLength);
} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_FACIAL_ANIMATION_LOADER_H
