#ifndef __DALI_TOOLKIT_VIEW_H__
#define __DALI_TOOLKIT_VIEW_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
// Forward declarations
class CustomActor;
}

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
// Forward declarations
class View;
}

/**
 * View provides a container where different Dali::Layer instances and a background could be added. It
 * also provides a View::OrientationChanged() method which could be connected to the Dali::Orientation::SignalChange() signal.
 * This method rotates all layers accordingly with the given orientation, it emits an OrientationAnimationStartsSignal signal just before the rotation animation starts.
 *
 * By default view anchor point and parent origin are centered, the size is full screen and is got directly from the Dali::Stage. However, by passing \e false to the
 * Toolkit::View::New() method a custom size could be specified, and after initialization, anchor point and parent origin could be updated.
 *
 * If a background is set, a background layer will be created and dropped to the bottom.
 *
 * Use example (application is a Dali::Application object):
 * \code{.cpp}
 * Stage stage = Stage::GetCurrent();
 *
 * // Create default View. By default it gets the stage size.
 * Toolkit::View view = Toolkit::View::New();
 *
 * // Add the view to the stage before setting the background.
 * stage.Add( view );
 *
 * // Set background image. BACKGROUND_IMAGE is a string with the background image file path.
 * Image backgroundImage = Image::New( BACKGROUND_IMAGE );
 * ImageActor backgroundImageActor = ImageActor::New( backgroundImage );
 * mView.SetBackground( backgroundImageActor );
 *
 * // Connects the orientation signal with the View::OrientationChanged method.
 * application.GetOrientation().ChangedSignal().Connect( &view, &Toolkit::View::OrientationChanged );
 *
 * // Create a content layer.
 * Layer contentLayer = Layer::New();
 * contentLayer.SetAnchorPoint( AnchorPoint::CENTER );
 * contentLayer.SetParentOrigin( ParentOrigin::CENTER );
 * contentLayer.ApplyConstraint( ParentConstraint::Size::New( ParentSize() ) );
 * view.AddContentLayer( contentLayer );
 * \endcode
 */
class View : public Control
{
public:
  //Signal Names
  static const char* const SIGNAL_ORIENTATION_ANIMATION_START;

public:

  /**
   * Create a View handle; this can be initialised with View::New()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  View();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   * @param handle to copy from
   */
  View( const View& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  View& operator=( const View& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~View();

  /**
   * Create an initialized View.
   * @param fullscreen If true, the view's size is going to be set with the Dali::Stage size. Otherwise a size must be provided. By default fullscreen is set to true.
   * @return A handle to a newly allocated Dali resource.
   */
  static View New( bool fullscreen = true );

  /**
   * Downcast an Object handle to View. If handle points to a View the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a View or an uninitialized handle
   */
  static View DownCast( BaseHandle handle );

  /**
   * Returns a content layer.
   * @param index to the layer container.
   * @return A Layer handle if it exists, otherwise it returns an uninitialized handle.
   */
  Layer GetContentLayer( unsigned int index ) const;

  /**
   * Adds a new layer in the view.
   * @pre layer must be initialized.
   * @param layer A Layer handle.
   * @return the an index that can be used to access the layer stores in the view.
   */
  unsigned int AddContentLayer( Layer layer );

  /**
   * Removes a layer from the view.
   * @param layer The layer to be removed.
   */
  void RemoveContentLayer( Layer layer );

  /**
   * Returns the background layer.
   * @return the background layer or an empty handle if any background has been set before.
   */
  Layer GetBackgroundLayer() const;

  /**
   * Sets a background image.
   *
   * It creates a background layer the first time this method is called and it is dropped to the bottom.
   * Any previous set background will be replaced by the new one.
   *
   * @pre View must be on stage before calling SetBackground.
   * @param image An image actor.
   */
  void SetBackground( ImageActor image );

  /**
   * Sets the angle values for portrait, landscape, portrait inverse and landscape inverse.
   *
   * These angles are used to rotate the views.
   * By default, orientation angles are initialized as follows: portrait 0, landscape 90, portrait inverse 180, landscape inverse 270.
   *
   * @param portrait angle in degrees.
   * @param landscale angle in degrees.
   * @param portraitInverse angle in degrees.
   * @param landscapeInverse angle in degrees.
   */
  void SetOrientationFunction( Degree portrait, Degree landscale, Degree portraitInverse, Degree landscapeInverse );

  /**
   * It rotates all layers to the new given orientation.
   *
   * @param orientation The new orientation.
   */
  void OrientationChanged( Orientation orientation );

  /**
   * Enables or disables the view's rotation when device orientation changes.
   * @param enabled Whether auto-rotate should be enabled or disabled. By default is enabled.
   */
  void SetAutoRotate( bool enabled );

public: //Signals

  // Orientation change animation starts.
  typedef SignalV2< void ( View, Animation&, const Orientation& ) > OrientationAnimationStartedSignalV2;

  /**
   * Signal emitted just before the rotate animation starts when the device orientation changes.
   */
  OrientationAnimationStartedSignalV2& OrientationAnimationStartedSignal();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  View( Internal::View& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  View( Dali::Internal::CustomActor* internal );
};


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_VIEW_H__
