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
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace Visual
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
class Base : public BaseObject
{
public:

  /**
   *  Initialisation of the visual, this API should only called by the VisualFactory:
   *  request the geometry and shader from the cache, if not available, create and save to the cache for sharing;
   *  record the property values.
   *
   * @param[in] actor The Actor the visual is applied to if, empty if the visual has not been applied to any Actor
   * @param[in] propertyMap The properties for the requested Visual object.
   */
  void Initialize( Actor& actor, const Property::Map& propertyMap );

  /**
   * @copydoc Toolkit::Visual::Base::SetName
   */
  void SetName( const std::string& name );

  /**
   * @copydoc Toolkit::Visual::Base::GetName
   */
  const std::string& GetName();

  /**
   * @copydoc Toolkit::Visual::Base::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc Toolkit::Visual::Base::GetSize
   */
  const Vector2& GetSize() const;

  /**
   * @copydoc Toolkit::Visual::Base::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize ) const;

  /**
   * @copydoc Toolkit::Visual::Base::SetDepthIndex
   */
  void SetDepthIndex( float index );

  /**
   * @copydoc Toolkit::Visual::Base::GetDepthIndex
   */
  float GetDepthIndex() const;

  /**
   * @copydoc Toolkit::Visual::Base::SetOnStage
   * @pre Impl->mGeometry must be created before this method is called
   */
  void SetOnStage( Actor& actor );

  /**
   * @copydoc Toolkit::Visual::Base::SetOffStage
   */
  void SetOffStage( Actor& actor );

  /**
   * @copydoc Toolkit::Visual::Base::CreatePropertyMap
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

  /**
   * @copydoc Toolkit::Visual::Base::SetProperty
   */
  void SetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue );

  /**
   * @copydoc Toolkit::Visual::Base::GetProperty
   */
  Dali::Property::Value GetProperty( Dali::Property::Index index );

protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  Base( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~Base();

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
  virtual void DoInitialize( Actor& actor, const Property::Map& propertyMap ) {}

protected:

  /**
   * @brief Called by SetOnStage() allowing sub classes to respond to the SetOnStage event
   *
   * @note The derived class is required to create the renderer, and add it to the actor when all the resources are in place.
   *
   * @param[in] actor The actor applying this visual.
   */
  virtual void DoSetOnStage( Actor& actor )=0;

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

protected:
  /**
   * @brief Called by SetProperty(). To be overriden by derived clases in order to set properties.
   *
   * @param [in] index The index of the property.
   * @param [in] propertyValue The new value of the property.
   */
  virtual void DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue ) = 0;

  /**
   * @brief Called by GetProperty(). To be overriden by derived classes in order to retrieve properties.
   *
   * @param [in] index The index of the property.
   *
   * @return The property value.
   */
  virtual Dali::Property::Value DoGetProperty( Dali::Property::Index index ) = 0;

private:

  // Undefined
  Base( const Visual::Base& visual );

  // Undefined
  Base& operator=( const Visual::Base& visual );

protected:
  struct Impl;
  Impl* mImpl;
  VisualFactoryCache& mFactoryCache;
};

} // namspace Visual

} // namespace Internal

inline const Internal::Visual::Base& GetImplementation(const Toolkit::Visual::Base& visualBase )
{
  DALI_ASSERT_ALWAYS( visualBase && "visual base handle is empty" );

  const BaseObject& handle = visualBase.GetBaseObject();

  return static_cast<const Internal::Visual::Base&>(handle);
}

inline Internal::Visual::Base& GetImplementation(Toolkit::Visual::Base& visualBase)
{
  DALI_ASSERT_ALWAYS( visualBase && "visual base handle is empty" );

  BaseObject& handle = visualBase.GetBaseObject();

  return static_cast<Internal::Visual::Base&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_VISUAL_H
