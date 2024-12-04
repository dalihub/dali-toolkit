#ifndef DALI_SCENE3D_MODEL_COMPONENTS_MATERIAL_H
#define DALI_SCENE3D_MODEL_COMPONENTS_MATERIAL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali
{
namespace Scene3D
{
// Forward declarations.
namespace Internal
{
class Material;
} // namespace Internal

/**
 * @addtogroup dali_scene3d_material
 * @{
 */

/**
 * @brief Class for setting Material properties of 3D models
 *
 * @SINCE_2_2.22
 *
 * @note This Material class is for setting Material properties of 3D models.
 * This Material supports properties and textures for PBR.
 * Also, Material can be shared with multiple ModelPrimitives and if the value is modified,
 * the rendering results of all ModelPrimitives using this Material will be changed.
 *
 * @code
 * Material material = Material::New();
 * ModelPrimitive modelPrimitive = ModelPrimitive::New();
 * modelPrimitive.SetMaterial(material);
 * material.SetProperty(PropertyIndex, PropertyValue);
 * @endcode
 */
class DALI_SCENE3D_API Material : public Dali::BaseHandle
{
public:
  /**
   * @brief Enumeration for the start and end property ranges for material.
   * @SINCE_2_2.22
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX          = PROPERTY_REGISTRATION_START_INDEX,    ///< Start index is used by the property registration macro. @SINCE_2_2.22
    MATERIAL_PROPERTY_START_INDEX = PROPERTY_START_INDEX,                 ///< Start index of Control properties. @SINCE_2_2.22
    MATERIAL_PROPERTY_END_INDEX   = MATERIAL_PROPERTY_START_INDEX + 1000, ///< Reserving 1000 property indices. @SINCE_2_2.22

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,        ///< @SINCE_2_2.22
    ANIMATABLE_PROPERTY_END_INDEX   = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000, ///< Reserve animatable property indices, @SINCE_2_2.22
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the Material class.
   * @SINCE_2_2.22
   */
  struct Property
  {
    enum
    {
      /**
       * @brief Name of material.
       * @details type Property::STRING.
       * @SINCE_2_2.22
       */
      NAME = MATERIAL_PROPERTY_START_INDEX,

      /**
       * @brief Property for the URL of the base color texture.
       * @details Type Property::STRING.
       * @SINCE_2_2.22
       */
      BASE_COLOR_URL,

      /**
       * @brief Property for the base color factor of the material surface.
       * @details Type Property::VECTOR4.
       * @SINCE_2_2.22
       */
      BASE_COLOR_FACTOR,

      /**
       * @brief Property for the URL of the metallic-roughness texture.
       * @details Type Property::STRING.
       * @SINCE_2_2.22
       */
      METALLIC_ROUGHNESS_URL,

      /**
       * @brief Property for the metallic factor of the material surface.
       * @details Type Property::FLOAT.
       * @SINCE_2_2.22
       */
      METALLIC_FACTOR,

      /**
       * @brief Property for the roughness factor of the material surface.
       * @details Type Property::FLOAT.
       * @SINCE_2_2.22
       */
      ROUGHNESS_FACTOR,

      /**
       * @brief Property for the URL of the normal texture.
       * @details Type Property::STRING.
       * @SINCE_2_2.22
       */
      NORMAL_URL,

      /**
       * @brief Property for the scale factor applied to normal vectors.
       * @details Type Property::FLOAT.
       * @SINCE_2_2.22
       */
      NORMAL_SCALE,

      /**
       * @brief Property for the URL of the occlusion texture.
       * @details Type Property::STRING.
       * @SINCE_2_2.22
       */
      OCCLUSION_URL,

      /**
       * @brief Property for the occlusion strength of the material surface.
       * @details Type Property::FLOAT.
       * @SINCE_2_2.22
       */
      OCCLUSION_STRENGTH,

      /**
       * @brief Property for the URL of the emissive texture.
       * @details Type Property::STRING.
       * @SINCE_2_2.22
       */
      EMISSIVE_URL,

      /**
       * @brief Emissive factor Property.
       * @details type Property::VECTOR3.
       * @SINCE_2_2.22
       */
      EMISSIVE_FACTOR,

      /**
       * @brief Alpha mode Property.
       * @details type Property::INTEGER.
       * @SINCE_2_2.22
       */
      ALPHA_MODE,

      /**
       * @brief Alpha cutoff Property.
       * @details type Property::FLOAT.
       * @SINCE_2_2.22
       */
      ALPHA_CUTOFF,

      /**
       * @brief Double sided Property.
       * @details type Property::BOOLEAN.
       * @SINCE_2_2.22
       */
      DOUBLE_SIDED,

      /**
       * @brief Index of refraction (IOR) of the material surface
       * @details type Property::FLOAT
       * @SINCE_2_2.22
       */
      IOR,

      /**
       * @brief Property for the URL of the specular texture.
       * @details Type Property::STRING.
       * @SINCE_2_2.22
       */
      SPECULAR_URL,

      /**
       * @brief Property for the specular factor of the material surface.
       * @details Type Property::FLOAT.
       * @SINCE_2_2.22
       */
      SPECULAR_FACTOR,

      /**
       * @brief Property for the URL of the specular color texture.
       * @details Type Property::STRING.
       * @SINCE_2_2.22
       */
      SPECULAR_COLOR_URL,

      /**
       * @brief Property for the specular color factor of the material surface.
       * @details Type Property::VECTOR3.
       * @SINCE_2_2.22
       */
      SPECULAR_COLOR_FACTOR,

      /**
       * @brief Property to define rendering order.
       * @details Depth index is used to define rendering order. This property
       * is compatible with Dali::Renderer::Property::DepthIndex. Basically,
       * a Renderer that has smaller depth index is rendered earlier.
       * In the ordinary DALI UI components has 0 as depth index by default.
       * (For the case of Toolkit::Control, its renderer has depth index
       * value between [-20, 20] as fallowing the renderer's purpose)
       *
       * In the Scene3D cases, the rendering order of each Renderer may need
       * to be manually defined to match scene developer's intent.
       * Scene3D::DepthIndex::Ranges could be used to adjust rendering order
       * between 3D Scene content.
       * Or it also could be used to manage UI component in 3D Scene independently.
       *
       * Changing the depth index only affects the rendering order, and does not
       * mean that objects drawn later will be drawn on top. To compute final
       * rendering order, whether the object is opaque or non-opaque takes precedence
       * over the depth index. Changing the rendering order among translucent objects
       * has a significant impact on the rendering results.
       * @SINCE_2_3.3
       */
      DEPTH_INDEX,
    };
  };

  enum TextureType
  {
    /**
     * @brief Base Color Texture Property.
     * @SINCE_2_2.22
     * @note This texture represents the base color of the material. In most cases, this will be the diffuse color of the material.
     */
    BASE_COLOR,

    /**
     * @brief Metallic Roughness Texture Property.
     * @SINCE_2_2.22
     * @note This texture represents the metallicness and roughness of the material. This texture can be used to make the material look more metallic or rough.
     */
    METALLIC_ROUGHNESS,

    /**
     * @brief Normal Texture Property.
     * @SINCE_2_2.22
     * @note This texture represents the surface of the material. This texture can be used to make the surface of the material look smooth or rough.
     */
    NORMAL,

    /**
     * @brief Occlusion Texture Property.
     * @SINCE_2_2.22
     * @note This texture represents the depth of the material. This texture can be used to make the material look more three-dimensional.
     */
    OCCLUSION,

    /**
     * @brief Emissive Texture Property.
     * @SINCE_2_2.22
     * @note This texture makes the material look like it's emitting light. This texture can be used to make the material look like it's glowing.
     */
    EMISSIVE,

    /**
     * @brief Specular Texture Property.
     * @SINCE_2_2.22
     * @note This texture represents the specular reflection of the material. This texture can be used to make the material look more reflective.
     */
    SPECULAR,

    /**
     * @brief Specular Color Texture Property.
     * @SINCE_2_2.22
     * @note This texture represents the color of the specular reflection of the material. This texture can be used to set the color of the specular reflection of the material.
     */
    SPECULAR_COLOR,
  };

  enum AlphaModeType
  {
    /**
     * @brief This indicates that the material is fully opaque and that the alpha value should be ignored.
     * @SINCE_2_2.22
     */
    OPAQUE,

    /**
     * @brief This indicates that the material is either fully opaque or fully transparent depending on the alpha value. The alpha value is used to mask out areas of the material that should be transparent.
     * @SINCE_2_2.22
     */
    MASK,

    /**
     * @brief This indicates that the material is transparent and that the alpha value should be used to blend the material with the background.
     * @SINCE_2_2.22
     */
    BLEND,
  };

public: // Creation & Destruction
  /**
   * @brief Create an initialized Material.
   *
   * @SINCE_2_2.22
   * @return A handle to a newly allocated Dali resource
   */
  static Material New();

  /**
   * @brief Creates an uninitialized Material.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   *
   * @SINCE_2_2.22
   */
  Material();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   *
   * @SINCE_2_2.22
   */
  ~Material();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_2_2.22
   * @param[in] material Handle to an object
   */
  Material(const Material& material);

  /**
   * @brief Move constructor
   *
   * @SINCE_2_2.22
   * @param[in] rhs A reference to the moved handle
   */
  Material(Material&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_2_2.22
   * @param[in] material Handle to an object
   * @return reference to this
   */
  Material& operator=(const Material& material);

  /**
   * @brief Move assignment
   *
   * @SINCE_2_2.22
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  Material& operator=(Material&& rhs) noexcept;

  /**
   * @brief Downcasts an Object handle to Material.
   *
   * If handle points to a Material, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_2_2.22
   * @param[in] handle Handle to an object
   * @return Handle to a Material or an uninitialized handle
   */
  static Material DownCast(BaseHandle handle);

public: // Public Method
  /**
   * @brief Sets the value of an existing property.
   * @note BaseHandle is not subclass of Handle. So this API is not use Handle.SetProperty
   *
   * @SINCE_2_2.22
   * @param[in] index The index of the property
   * @param[in] propertyValue The new value of the property
   */
  void SetProperty(Dali::Property::Index index, Dali::Property::Value propertyValue);

  /**
   * @brief Retrieves a property value.
   * @note BaseHandle is not subclass of Handle. So this API is not use Handle.SetProperty
   *
   * @SINCE_2_2.22
   * @param[in] index The index of the property
   * @return The property value
   * @note This returns the value set by SetProperty() or the animation target value if it is being animated.
   */
  Dali::Property::Value GetProperty(Dali::Property::Index index) const;

  /**
   * @brief Convenience function for obtaining a property of a known type.
   *
   * @SINCE_2_2.22
   * @param[in] index The index of the property
   * @return The property value
   * @pre The property types match i.e. PropertyTypes::Get<T>() is equal to GetPropertyType(index).
   * @see GetProperty()
   */
  template<typename T>
  T GetProperty(Dali::Property::Index index) const
  {
    Dali::Property::Value value = GetProperty(index);

    return T(value.Get<T>());
  }

  /**
   * @brief Sets the texture for a given texture type.
   *
   * @SINCE_2_2.22
   * @param[in] index The texture type index
   * @param[in] texture The texture to set.
   */
  void SetTexture(Scene3D::Material::TextureType index, Dali::Texture texture);

  /**
   * @brief Gets the texture for a given texture type.
   *
   * @SINCE_2_2.22
   * @param[in] index The texture type index
   * @return The texture at the given index.
   */
  Dali::Texture GetTexture(Scene3D::Material::TextureType index);

  /**
   * @brief Sets the sampler for a given texture type.
   *
   * @SINCE_2_2.22
   * @param[in] index The texture type index
   * @param[in] sampler The sampler to use for this texture type
   */
  void SetSampler(Scene3D::Material::TextureType index, Dali::Sampler sampler);

  /**
   * @brief Gets the sampler for a given texture type.
   *
   * @SINCE_2_2.22
   * @param[in] index The texture type index
   * @return The sampler used for this texture type
   */
  Dali::Sampler GetSampler(Scene3D::Material::TextureType index);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Scene3D::Internal implementation.
   *
   * @param[in] implementation The Material implementation
   */
  DALI_INTERNAL Material(Dali::Scene3D::Internal::Material* implementation);
  /// @endcond
};

/**
 * @}
 */

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_COMPONENTS_MATERIAL_H
