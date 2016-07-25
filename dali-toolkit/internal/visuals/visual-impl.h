#ifndef DALI_TOOLKIT_INTERNAL_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_VISUAL_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/devel-api/visual-factory/visual.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * Base class for all Control rendering logic. A control may have multiple visuals.
 *
 * Note: The visual responds to the the Actor::COLOR by blending it with the 'Multiply' operator.
 *
 * The following properties are optional
 *
 * | %Property Name          | Type             |
 * |-------------------------|------------------|
 * | customShader            | MAP              |
 *
 * where custom-shader is a map with the following properties:
 * | %Property Name          | Type             |
 * |-------------------------|------------------|
 * | vertexShader            | STRING           |
 * | fragmentShader          | STRING           |
 * | subdivideGridX          | INT              |
 * | subdivideGridY          | INT              |
 * | shaderHints             | INT              |
 */
class Visual : public BaseObject
{
public:

  /**
   * Initialisation of the visual, this API should only called by the VisualFactory:
   *  request the geometry and shader from the cache, if not available, create and save to the cache for sharing;
   *  record the property values.
   *
   * @param[in] actor The Actor the visual is applied to if, empty if the visual has not been applied to any Actor
   * @param[in] propertyMap The properties for the requested Visual object.
   */
  void Initialize( Actor& actor, const Property::Map& propertyMap );

  /**
   * @copydoc Toolkit::Visual::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc Toolkit::Visual::GetSize
   */
  const Vector2& GetSize() const;

  /**
   * @copydoc Toolkit::Visual::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * ToDo: Add this function to Toolkit::Visual when it is fully implemented.
   *
   * Set the clip rectangular of this visual.
   * The contents of the visual will not be visible outside this rectangular.
   *
   * @param [in] clipRect The clipping rectangular.
   */
  virtual void SetClipRect( const Rect<int>& clipRect );

  /**
   *ToDo: Add this function to Toolkit::Visual when it is fully implemented.
   *
   * Reposition this visual with a 2D offset.
   *
   * @param[in] offset The offset to reposition the visual.
   */
  virtual void SetOffset( const Vector2& offset );

  /**
   * @copydoc Toolkit::Visual::SetDepthIndex
   */
  void SetDepthIndex( float index );

  /**
   * @copydoc Toolkit::Visual::GetDepthIndex
   */
  float GetDepthIndex() const;

  /**
   * @copydoc Toolkit::Visual::SetOnStage
   * @pre Impl->mGeometry must be created before this method is called
   */
  void SetOnStage( Actor& actor );

  /**
   * @copydoc Toolkit::Visual::SetOffStage
   */
  void SetOffStage( Actor& actor );

  /**
   * @copydoc Toolkit::Visual::CreatePropertyMap
   */
  void CreatePropertyMap( Property::Map& map ) const;

  /**
   * @brief Set whether the Pre-multiplied Alpha Blending is required
   *
   * @param[in] preMultipled whether alpha is pre-multiplied.
   */
  void EnablePreMultipliedAlpha(  bool preMultipled );

  /**
   * @brief Query whether alpha is pre-multiplied.
   *
   * @return True is alpha is pre-multiplied, false otherwise.
   */
  bool IsPreMultipliedAlphaEnabled() const;

  /**
   * @brief Sets properties of custom shader
   * @param[in] propertyMap Property map containing the custom shader data
   */
  void SetCustomShader( const Property::Map& propertyMap );

protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  Visual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Visual();

protected:
  /**
   * @brief Called by CreatePropertyMap() allowing sub classes to respond to the CreatePropertyMap event
   *
   * @param[out] map The visual property map.
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const = 0;

  /**
   * @brief Called by Initialize() allowing sub classes to respond to the Initialize event
   *
   * @param[in] actor The Actor the visual is applied to if, empty if the visual has not been applied to any Actor
   * @param[in] propertyMap The properties for the requested Visual object.
   */
  virtual void DoInitialize( Actor& actor, const Property::Map& propertyMap ) {};

protected:

  /**
   * @brief Called by SetOnStage() allowing sub classes to respond to the SetOnStage event
   *
   * @param[in] actor The actor applying this visual.
   */
  virtual void DoSetOnStage( Actor& actor );

  /**
   * @brief Called by SetOffStage() allowing sub classes to respond to the SetOffStage event
   *
   * @param[in] actor The actor applying this visual.
   */
  virtual void DoSetOffStage( Actor& actor );

protected:
  /**
   * @brief Gets the on stage state for this Visual
   *
   * @return Returns true if this Visual is on stage, false if it is off the stage
   */
  bool GetIsOnStage() const;

  /**
   * @brief Gets whether the Dali::Renderer is from a shared cache (and therefore any modifications will affect other users of that renderer)
   *
   * @return Returns true if the renderer is from shared cache, false otherwise
   */
  bool GetIsFromCache() const;

private:

  // Undefined
  Visual( const Visual& visual );

  // Undefined
  Visual& operator=( const Visual& visual );

protected:
  struct Impl;
  Impl* mImpl;
  VisualFactoryCache& mFactoryCache;
};

} // namespace Internal

inline const Internal::Visual& GetImplementation(const Toolkit::Visual& visual)
{
  DALI_ASSERT_ALWAYS( visual && "Visual handle is empty" );

  const BaseObject& handle = visual.GetBaseObject();

  return static_cast<const Internal::Visual&>(handle);
}

inline Internal::Visual& GetImplementation(Toolkit::Visual& visual)
{
  DALI_ASSERT_ALWAYS( visual && "Visual handle is empty" );

  BaseObject& handle = visual.GetBaseObject();

  return static_cast<Internal::Visual&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_VISUAL_H
