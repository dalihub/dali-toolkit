#ifndef DALI_SCENE3D_MODEL_COMPONENTS_MATERIAL_MODIFY_OBSERVER_H
#define DALI_SCENE3D_MODEL_COMPONENTS_MATERIAL_MODIFY_OBSERVER_H

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
#include <dali/devel-api/common/bitwise-enum.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/model-components/material.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
/**
 * @brief MaterialModifyObserver is a class to notify that the material has changed.
 */
class MaterialModifyObserver
{
public:
  /**
   * @brief Flag which informations are changed.
   */
  enum ModifyFlag
  {
    NONE    = 0,
    TEXTURE = 1 << 0,
    SHADER  = 1 << 1,
    UNIFORM = 1 << 2,
  };

  /**
    * @brief Notify to observers that texture is changed.
    *
    * @param[in] material The handle material itself. TODO : Are we need this?
    * @param[in] flag Flag which informations are changed.
    */
  virtual void OnMaterialModified(Dali::Scene3D::Material material, ModifyFlag flag) = 0;
};

} // namespace Internal

} // namespace Scene3D

// specialization has to be done in the same namespace
template<>
struct EnableBitMaskOperators<Scene3D::Internal::MaterialModifyObserver::ModifyFlag>
{
  static const bool ENABLE = true;
};

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_COMPONENTS_MATERIAL_MODIFY_OBSERVER_H
