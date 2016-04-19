#ifndef __DALI_TOOLKIT_EFFECTS_VIEW_H__
#define __DALI_TOOLKIT_EFFECTS_VIEW_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/frame-buffer-image.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
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
 * EffectsView effectsView = EffectsView::New( Toolkit::EffectsView::EMBOSS );
 *
 * // set position and format
 * effectsView.SetParentOrigin( ParentOrigin::CENTER );
 * effectsView.SetSize( Vector2( width, height) );
 * effectsView.SetPixelFormat( Pixel::RGBA8888 );
 *
 * // set effect type and properties
 * effectsView.SetProperty( effectsView.GetEffectSizePropertyIndex(), static_cast< float >( shadowSize ) );
 * effectsView.SetProperty( effectsView.GetEffectOffsetPropertyIndex(), Vector3( shadowDistance.x, shadowDistance.y, 0.0f ) );
 * effectsView.SetProperty( effectsView.GetEffectColorPropertyIndex(), shadowColor );
 *
 * // Render once
 * effectsView.SetRefreshOnDemand( true );
 *
 * // optionally set a clear color
 * effectsView.SetBackgroundColor( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) );
 */
class DALI_IMPORT_API EffectsView : public Control
{
public:

  enum EffectType
  {
    DROP_SHADOW,
    EMBOSS,
    INVALID_TYPE
  };

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,  ///< @SINCE_1_0.0
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,              ///< Reserve property indices @SINCE_1_0.0

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,        ///< @SINCE_1_1.18
    ANIMATABLE_PROPERTY_END_INDEX =   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000  ///< Reserve animatable property indices, @SINCE_1_1.18
  };

  /**
   * @brief An enumeration of properties belonging to the EffectsView class.
   */
  struct Property
  {
    enum
    {
      // Event side properties
      EFFECT_SIZE = PROPERTY_START_INDEX,              ///< name "effectSize", type INTEGER

      // Animatable properties
      EFFECT_OFFSET = ANIMATABLE_PROPERTY_START_INDEX, ///< name "effectOffset", type VECTOR3
      EFFECT_COLOR,                                    ///< name "effectColor", type VECTOR4
    };
  };

public:

  /**
   * Create an EffectsView object with default configuration
   * @param[in] type The type of effect to be performed by the EffectView.
   *                 A member of the EffectType enumeration.
   */
  static EffectsView New( EffectType type );

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
   * Get the effect type
   * @return The type of effect performed by the EffectView. A member of the EffectType enumeration.
   */
  EffectType GetType() const;

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
  DALI_INTERNAL EffectsView( Internal::EffectsView& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL EffectsView( Dali::Internal::CustomActor* internal );

}; // class EffectsView

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_EFFECTS_VIEW_H__
