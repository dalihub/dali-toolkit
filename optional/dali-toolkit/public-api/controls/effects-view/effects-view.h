#ifndef __DALI_TOOLKIT_EFFECTS_VIEW_H__
#define __DALI_TOOLKIT_EFFECTS_VIEW_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{

class EffectsView;

} // namespace Internal

/**
 * EffectsView: Applies an effect to a tree of actors
 *
 * Example usage: Applying an emboss effect
 * ...
 * EffectsView effectsView = EffectsView::New();
 *
 * // set position and format
 * effectsView.SetParentOrigin( ParentOrigin::CENTER );
 * effectsView.SetSize( Vector2( width, height) );
 * effectsView.SetPixelFormat( Pixel::RGBA8888 );
 *
 * // set effect type and properties
 * effectsView.SetType( Toolkit::EffectsView::EMBOSS );
 * effectsView.SetProperty( effectsView.GetEffectSizePropertyIndex(), static_cast< float >( shadowSize ) );
 * effectsView.SetProperty( effectsView.GetEffectOffsetPropertyIndex(), Vector3( shadowDistance.x, shadowDistance.y, 0.0f ) );
 * effectsView.SetProperty( effectsView.GetEffectColorPropertyIndex(), shadowColor );
 *
 * // Render result to an offscreen
 * effectsView.SetOutputImage( image );
 *
 * // Render once
 * effectsView.SetRefreshOnDemand( true );
 *
 * // optionally set a clear color
 * effectsView.SetBackgroundColor( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );
 *
 * // start effect processing
 * effectsView.Enable();
 */
class EffectsView : public Control
{
public:

  enum EffectType
  {
    DROP_SHADOW,
    EMBOSS,
    INVALID_TYPE
  };

public:

  /**
   * Create an EffectsView object with default configuration
   */
  static EffectsView New();

  /**
   * Create an uninitialized EffectsView. Only derived versions can be instantiated.
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  EffectsView();

  /**
   * Copy constructor.
   */
  EffectsView( const EffectsView& handle );

  /**
   * Assignment operator.
   */
  EffectsView& operator=( const EffectsView& rhs );

  /**
   * Downcast an Object handle to EffectsView. If handle points to a EffectsView the
   * downcast produces a valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a EffectsView or an uninitialized handle
   */
  static EffectsView DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~EffectsView();

public:

  /**
   * Set the effect type
   * @param[in] type The type of effect to be performed by the EffectView.
   *                 A member of the EffectType enumeration.
   */
  void SetType( EffectType type );

  /**
   * Get the effect type
   * @return The type of effect performed by the EffectView. A member of the EffectType enumeration.
   */
  EffectType GetType() const;

  /**
   * Enable the effect
   */
  void Enable();

  /**
   * Disable the effect
   */
  void Disable();

  /**
   * Refresh/Redraw the effect
   */
  void Refresh();

  /**
   * Set refresh mode
   * @param[in] onDemand Set true to enable on demand rendering, call Refresh() whenever a render is required.
   *                     Set false to render each frame. (EffectsView refresh mode is set to continuous by default).
   */
  void SetRefreshOnDemand( bool onDemand );

   /**
    * Set the pixel format for the output
    * @param[in] pixelFormat The pixel format for the output
    */
   void SetPixelFormat( Pixel::Format pixelFormat );

   /**
    * Set the FrameBufferImage that will receive the final output of the EffectsView.
    * @param[in] image User supplied FrameBufferImage that will receive the final output of the EffectsView.
    */
   void SetOutputImage( FrameBufferImage image );

   /**
    * Get the FrameBufferImage that holds the final output of the EffectsView.
    * @return The FrameBufferImage that holds the final output of the EffectsView.
    */
   FrameBufferImage GetOutputImage();

   /**
    * Get the property index to the effect size
    * @return The property index to the effect size
    */
   Property::Index GetEffectSizePropertyIndex() const;

   /**
    * Get the property index to the effect strength
    * @return The property index to the effect strength
    */
   Property::Index GetEffectStrengthPropertyIndex() const;

   /**
    * Get the property index to the Vector3 specifying the effect offset (eg drop shadow offset)
    * @return The property index to the Vector3 specifying the effect offset
    */
   Property::Index GetEffectOffsetPropertyIndex() const;

   /**
    * Get the property index to the effect color (eg shadow color)
    * @return The property index to the effect color
    */
   Property::Index GetEffectColorPropertyIndex() const;

   /**
    * Set background color for the view. The background will be filled with this color.
    * @param[in] color The background color.
    */
   void SetBackgroundColor( const Vector4& color );

   /**
    * Get the background color.
    * @return The background color.
    */
   Vector4 GetBackgroundColor() const;

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  EffectsView( Internal::EffectsView& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  EffectsView( Dali::Internal::CustomActor* internal );

}; // class EffectsView

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_EFFECTS_VIEW_H__
