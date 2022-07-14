#ifndef DALI_SCENE3D_LOADER_FACIAL_ANIMATION_LOADER_H
#define DALI_SCENE3D_LOADER_FACIAL_ANIMATION_LOADER_H

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
#include <dali-scene3d/public-api/loader/animation-definition.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Loads a facial animation encoded in a json file in the given @p url.
 *
 * Throws a DaliException on error.
 *
 * @param[in] url The url of the file containing the facial animation.
 *
 * @return An animation definition.
 */
DALI_SCENE3D_API AnimationDefinition LoadFacialAnimation(const std::string& url);
} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_FACIAL_ANIMATION_LOADER_H
