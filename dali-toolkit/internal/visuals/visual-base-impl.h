#ifndef DALI_TOOLKIT_INTERNAL_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_VISUAL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/internal/visuals/transition-data-impl.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/devel-api/direction-enums.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace Visual
{

class ResourceObserver;

using FittingMode = DevelVisual::FittingMode;

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
   * Setting the properties of the visual, this API should only called by the VisualFactory
   * @param[in] propertyMap The properties for the requested Visual object.
   */
  void SetProperties( const Property::Map& propertyMap );

  /**
   * @copydoc Toolkit::Visual::Base::SetName
   */
  void SetName( const std::string& name );

  /**
   * @copydoc Toolkit::Visual::Base::GetName
   */
  const std::string& GetName() const;

  /**
   * @copydoc Toolkit::Visual::Base::SetSize
   */
  void SetTransformAndSize( const Property::Map& transform, Size controlSize );

  /**
   * @brief Performs an action on the visual with the given action name and attributes.
   *
   * @param[in] actionName The name of the action to perform this API only takes an Index
   * @param[in] attributes The list of attributes for the action. ( optional for this data structure to have content )
   */
  void DoAction( const Dali::Property::Index actionName, const Dali::Property::Value attributes );

  /**
   * @copydoc Toolkit::Visual::Base::GetHeightForWidth
   */
  virtual float GetHeightForWidth( float width );

  /**
   * @copydoc Toolkit::Visual::Base::GetWidthForHeight
   */
  virtual float GetWidthForHeight( float height );

  /**
   * @copydoc Toolkit::Visual::Base::GetNaturalSize
   */
  virtual void GetNaturalSize( Vector2& naturalSize );

  /**
   * @copydoc Toolkit::Visual::Base::SetDepthIndex
   */
  void SetDepthIndex( int index );

  /**
   * @copydoc Toolkit::Visual::Base::GetDepthIndex
   */
  int GetDepthIndex() const;

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
   * @brief Create a property map containing per-instance visual properties.
   *
   * This will enable creation of new visuals on control state change with
   * any alternative style properties and the relevant instance properties
   * (e.g. for image visual, the desired size, and for text visual, the actual text).
   * @param[in] map The property map into which to write
   */
  void CreateInstancePropertyMap( Property::Map& map ) const;

  /**
   * @brief Set whether the Pre-multiplied Alpha Blending is required
   *
   * @param[in] preMultiplied whether alpha is pre-multiplied.
   */
  void EnablePreMultipliedAlpha( bool preMultiplied );

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

  /**
   * Gets currently staged renderer, or an empty handle if not staged
   */
  Renderer GetRenderer();

  /**
   * Sets the mix color ( including opacity )  of the visual.
   * @param[in] mixColor The new mix color
   */
  void SetMixColor( const Vector4& color );

  /**
   * Sets the mix color of the visual.
   * @param[in] mixColor The new mix color
   */
  void SetMixColor( const Vector3& color );

  /**
   * Gets the mix color of the visual.
   * @return The mix color
   */
  const Vector4& GetMixColor() const;

  /**
   * Animate the property if it exists in the visual or renderer.
   *
   * If it's a visual property such as mix color or a transform property,
   * saves the target value to the local data.
   *
   * If the visual isn't staged (i.e. it doesn't have a renderer),
   * then this will not add an animation.
   *
   * If the animator is valid and the transition handle is empty - it will
   * be created.
   *
   * @param[in] transition The animation to create or attach to
   * @param[in] animator The animation parameters of the property.
   */
  void AnimateProperty( Dali::Animation& transition,
                        Internal::TransitionData::Animator& animator );

  /**
   * @brief Add an observer to watch for when the Visuals resources are loaded.
   * Currently only supports a single observer
   *
   */
  void AddResourceObserver( Visual::ResourceObserver& observer );

  /**
   * @brief Remove an observer
   */
  void RemoveResourceObserver( Visual::ResourceObserver& observer );

  /**
   * @brief Called when the visuals resources are loaded / ready
   */
  void ResourceReady( Toolkit::Visual::ResourceStatus resourceStatus );

  /**
   * @brief Called when the visuals resources are loaded / ready
   * @return true if ready, false otherwise
   */
  virtual bool IsResourceReady() const;

  /**
   * @brief Get the loading state of the visual resource
   * @return Return the loading status (PREPARING, READY and FAILED) of visual resource
   */
  Toolkit::Visual::ResourceStatus GetResourceStatus() const;

  /**
   * @brief Get the fitting mode for the visual
   */
  FittingMode GetFittingMode() const;

 protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  Base( VisualFactoryCache& factoryCache, FittingMode fittingMode );

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
   * @brief Called by CreateInstancePropertyMap() allowing derived
   * classes to store instanced data (separate to styled data) that
   * needs copying between visuals on state change.
   *
   * @param[out] map The visual property map
   */
  virtual void DoCreateInstancePropertyMap( Property::Map& map ) const = 0;

  /**
   * @brief Called by SetProperties() allowing sub classes to set their properties
   *
   * @param[in] propertyMap The properties for the requested Visual object.
   */
  virtual void DoSetProperties( const Property::Map& propertyMap ) = 0;

  /**
   * @brief Called when transform or control size changes
   * ( Of use to SVG and Text visuals )
   */
  virtual void OnSetTransform() = 0;

  /**
   * @brief Called by SetOnStage() allowing sub classes to respond to the SetOnStage event
   *
   * @note The derived class is required to create the renderer, and add it to the actor when all the resources are in place.
   *
   * @param[in] actor The actor applying this visual.
   */
  virtual void DoSetOnStage( Actor& actor ) = 0;

  /**
   * @brief Called by SetOffStage() allowing sub classes to respond to the SetOffStage event
   *
   * @param[in] actor The actor applying this visual.
   */
  virtual void DoSetOffStage( Actor& actor );

  /**
   * @brief Called by DoAction() allowing sub classes to do the given action.
   *
   * @param[in] actionId The action to perform
   * @param[in] attributes The list of attributes for the action. ( optional for this data structure to have content )
   */
  virtual void OnDoAction( const Property::Index actionId, const Property::Value& attributes );

protected:

  /**
   * @brief Gets the on stage state for this Visual
   *
   * @return Returns true if this Visual is on stage, false if it is off the stage
   */
  bool IsOnStage() const;

private:

  /**
   * Register the mix color uniform on the Renderer and store the property index.
   * Note, this is not used by Color or Primitive Visuals, which will use their
   * own property index.
   */
  void RegisterMixColor();

  /**
   * Find the matching property on the renderer or shader. If it's a shader
   * property, register it on the renderer in order to animate it for this
   * visual independently.
   * @param[in] key The key to match.
   * @return the matching index, or INVALID_INDEX if it's not found
   */
  Property::Index GetPropertyIndex( Property::Key key );

  /**
   * Set up the transition. If no animation is required, then
   * transition will be untouched.
   *
   * @param[in] transition The transition to use or set up.
   * @param[in] animator The animation data to use
   * @param[in] index The property index on the renderer to animate
   * @param[in] initialValue The optional initial value
   * @param[in] targetValue The target value to use
   */
  void SetupTransition( Dali::Animation& transition,
                        Internal::TransitionData::Animator& animator,
                        Property::Index index,
                        Property::Value& initialValue,
                        Property::Value& targetValue );

  /**
   * Animate the opacity property - Special handling to
   * ensure that the blend mode is set to ON whilst animating,
   * and set back to AUTO if it's opaque at the end of the
   * animation.
   *
   * @param[in] transition The transition to use or set up.
   * @param[in] animator The animation data to use
   */
  void AnimateOpacityProperty( Dali::Animation& transition,
                               Internal::TransitionData::Animator& animator );

  /**
   * Animate the renderer property - no special handling
   *
   * @param[in] transition The transition to use or set up.
   * @param[in] animator The animation data to use
   */
  void AnimateRendererProperty( Dali::Animation& transition,
                                Internal::TransitionData::Animator& animator );

  /**
   * Animate the mix color property.
   *
   * If the animator is a vec3, then it only animates the color
   * channels without animating the opacity.  If it's a vec4, then it
   * runs 2 animators, one for the the vec3 mixColor, and one for the
   * opacity. (They are separate uniforms in the shader )
   *
   * @param[in] transition The transition to use or set up.
   * @param[in] animator The animation data to use
   */
  void AnimateMixColorProperty( Dali::Animation& transition,
                                Internal::TransitionData::Animator& animator );

  /**
   * Set up the right blend mode if the opacity is being animated.
   * Also ensure that when the animation finishes, the blend mode is
   * set to the appropriate value. It also uses the target value as
   * set into mMixColor.
   *
   * @param[in] transition The transition to listen to
   * @param[in] isInitialOpaque Whether the initial value is opaque
   * @param[in] animating If the transition animates the value.
   */
  void SetupBlendMode( Dali::Animation& transition,
                       bool isInitialOpaque, bool animating );

  /**
   * When a mix color animation has finished, ensure the blend mode is set back
   * to the right value for the target opacity.
   */
  void OnMixColorFinished( Animation& animation );

  // Undefined
  Base( const Visual::Base& visual );

  // Undefined
  Base& operator=( const Visual::Base& visual );

protected:
  struct Impl;
  Impl* mImpl;
  VisualFactoryCache& mFactoryCache;
};

typedef IntrusivePtr<Base> BasePtr;

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
