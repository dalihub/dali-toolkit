#ifndef DALI_SCENE3D_MODEL_COMPONENTS_MODEL_PRIMITIVE_H
#define DALI_SCENE3D_MODEL_COMPONENTS_MODEL_PRIMITIVE_H

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/model-components/material.h>

namespace Dali
{
namespace Scene3D
{
//Forward declarations.
namespace Internal
{
class ModelPrimitive;
} // namespace Internal

/**
 * @addtogroup dali_scene3d_model_components_model_primitive
 * @{
 */

/**
 * @brief This ModelPrimitive class is required to draw the mesh geometry defined by the user.
 * Users can set Dali::Geometry and Material to ModelPrimitive using SetGeometry and SetMaterial methods respectively.
 * When ModelPrimitive added to ModelNode using ModelNode::AddModelPrimitive() method, the Geometry is rendered on the screen according to the Material settings.
 *
 * @SINCE_2_2.99
 *
 * If you load resources from 3D format files such as glTF using Model class, ModelPrimitive is also created internally.
 * In this case, blendShape morphing or skeletal animation defined in the format can be used.
 * However, for the custom ModelPrimitive that is created by user, blendShape morphing or skeletal animation is not supported.
 */
class DALI_SCENE3D_API ModelPrimitive : public Dali::BaseHandle
{
public: // Creation & Destruction
  /**
   * @brief Create an initialized ModelPrimitive.
   *
   * @SINCE_2_2.99
   * @return A handle to a newly allocated Dali resource
   */
  static ModelPrimitive New();

  /**
   * @brief Creates an uninitialized ModelPrimitive.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_2.99
   */
  ModelPrimitive();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_2.99
   */
  ~ModelPrimitive();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_2.99
   * @param[in] modelPrimtive Handle to an object
   */
  ModelPrimitive(const ModelPrimitive& modelPrimtive);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_2.99
   * @param[in] rhs A reference to the moved handle
   */
  ModelPrimitive(ModelPrimitive&& rhs);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.99
   * @param[in] modelPrimitive Handle to an object
   * @return reference to this
   */
  ModelPrimitive& operator=(const ModelPrimitive& modelPrimitive);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_2.99
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  ModelPrimitive& operator=(ModelPrimitive&& rhs);

  /**
   * @brief Downcasts an Object handle to ModelPrimitive.
   *
   * If handle points to a ModelPrimitive, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.99
   * @param[in] handle Handle to an object
   * @return Handle to a ModelPrimitive or an uninitialized handle
   */
  static ModelPrimitive DownCast(BaseHandle handle);

public: // Public Method
  /**
   * @brief Set the geometry for this renderer.
   *
   * @SINCE_2_2.99
   * @param[in] geometry The geometry to set.
   */
  void SetGeometry(Dali::Geometry geometry);

  /**
   * @brief Get the geometry for this renderer.
   *
   * @SINCE_2_2.99
   * @return The geometry for this renderer.
   */
  Dali::Geometry GetGeometry() const;

  /**
   * @brief Sets the material for this primitive.
   *
   * @SINCE_2_2.99
   * @param[in] material The material
   */
  void SetMaterial(Material material);

  /**
   * @brief Retrieves a material.
   *
   * @SINCE_2_2.99
   * @return The material handle
   */
  Material GetMaterial() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Scene3D::Internal implementation.
   *
   * @param[in] implementation The ModelPrimitive implementation
   */
  DALI_INTERNAL ModelPrimitive(Dali::Scene3D::Internal::ModelPrimitive* implementation);
  /// @endcond
};

/**
 * @}
 */

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_COMPONENTS_MODEL_PRIMITIVE_H
