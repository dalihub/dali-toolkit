#ifndef DALI_SCENE3D_MODEL_COMPONENTS_MODEL_PRIMITIVE_MODIFY_OBSERVER_H
#define DALI_SCENE3D_MODEL_COMPONENTS_MODEL_PRIMITIVE_MODIFY_OBSERVER_H

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
#include <dali-scene3d/public-api/model-components/model-primitive.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
/**
 * @brief Observer class that notifies ModelNode when a Renderer of ModelPrimitive is created.
 */
class ModelPrimitiveModifyObserver
{
public:
  /**
   * @brief Called when a Renderer of ModelPrimitive is created.
   *
   * @param[in] renderer The Renderer that is created.
   */
  virtual void OnRendererCreated(Renderer renderer) = 0;
};

} // namespace Internal

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_COMPONENTS_MODEL_PRIMITIVE_MODIFY_OBSERVER_H
