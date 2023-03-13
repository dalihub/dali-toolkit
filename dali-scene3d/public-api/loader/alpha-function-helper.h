#ifndef DALI_SCENE3D_LOADER_ALPHA_FUNCTION_HELPER_H_
#define DALI_SCENE3D_LOADER_ALPHA_FUNCTION_HELPER_H_
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

// EXTERNAL INCLUDES
#include <dali/public-api/animation/alpha-function.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
/**
 * @return Given a name, provide a AlphaFunction; if the name was not
 *  recognised, get the default one.
 */
AlphaFunction DALI_SCENE3D_API GetAlphaFunction(const std::string& name, bool* found = nullptr);

/**
 * @brief Registers an alpha function only if one with the same @a name has
 *  not yet been registered. Throws Exception the name isn't unique.
 */
void DALI_SCENE3D_API RegisterAlphaFunction(const std::string& name, AlphaFunction alphaFn) noexcept(false);

} // namespace Dali::Scene3D::Loader

#endif //DALI_SCENE3D_LOADER_ALPHA_FUNCTION_HELPER_H_
