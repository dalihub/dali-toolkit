#ifndef __DALI_TOOLKIT_INTERNAL_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_VIEW_H__

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
#include <dali/dali.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/view/view.h>

namespace Dali
{

namespace Toolkit
{

class View;

namespace Internal
{

/**
 * View is a control to add layers and a background.
 * @see Dali::Toolkit::View for more details.
 */
class View : public ControlImpl
{
private:
  typedef std::map<unsigned int,Layer> LayerContainer;
  typedef std::map<unsigned int,Layer>::iterator LayerIt;
  typedef std::map<unsigned int,Layer>::const_iterator LayerConstIt;

  /**
   * Orientation declaration used internally to rotate the view.
   * The angles associated with each enum value could be changed with the SetOrientationFunction method.
   */
  enum Orientation
  {
    PORTRAIT,          ///< portrait orientation.
    LANDSCAPE,         ///< landscape orientation.
    PORTRAIT_INVERSE,  ///< portrait inverse orientation.
    LANDSCAPE_INVERSE  ///< landscape inverse orientation.
  };

public:

  /**
   * Create an initialized View.
   * @param fullscreen If true, the view's size is going to be set with the Dali::Stage size. Otherwise a size must be provided.
   * @return A handle to a newly allocated Dali resource.
   */
  static Toolkit::View New( bool fullscreen );

  /**
   * @copydoc Dali::Toolkit::View::GetContentLayer()
   */
  Layer GetContentLayer( unsigned int index ) const;

  /**
   * @copydoc Dali::Toolkit::View::AddContentLayer()
   */
  unsigned int AddContentLayer( Layer layer );

  /**
   * @copydoc Dali::Toolkit::View::RemoveContentLayer()
   */
  void RemoveContentLayer( Layer layer );

  /**
   * @copydoc Dali::Toolkit::View::GetBackgroundLayer()
   */
  Layer GetBackgroundLayer() const;

  /**
   * @copydoc Dali::Toolkit::View::SetBackground()
   */
  void SetBackground( ImageActor image );

  /**
   * @copydoc Dali::Toolkit::View::SetOrientationFunction()
   */
  void SetOrientationFunction( Degree portrait, Degree landscale, Degree portraitInverse, Degree landscapeInverse );

  /**
   * @copydoc Dali::Toolkit::View::OrientationChanged()
   *
   */
  void OrientationChanged( Dali::Orientation orientation );

  /**
   * @copydoc Dali::Toolkit::View::SetAutoRotate()
   *
   */
  void SetAutoRotate( bool enabled );

public:

  /**
   * @copydoc Dali::Toolkit::View::AnimationStartedSignalOrientation()
   */
  Toolkit::View::OrientationAnimationStartedSignalV2& OrientationAnimationStartedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

private: // From ControlImpl

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

private:


  /**
   * Constructor.
   * It initializes View members.
   * It initializes orientations as follows: portrait 0, landscape 90, portrait inverse 180, landscape inverse 270.
   * @param fullscreen If true, the view's size is going to be set with the Dali::Stage size. Otherwise a size must be provided.
   */
  View(bool fullscreen);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~View();

  /**
   * Return an orientation for the given angle in degrees.
   * @param degree angle in degrees.
   * @return An internal orientation.
   */
  View::Orientation DegreeToViewOrientation( Degree degree );

  /**
   * Find a layer in the layer container. Non const method
   */
  LayerIt FindLayer( Layer layer );

private:
  int            mOrientation;            ///< Stores the given orientation in degrees.
  bool           mFullScreen;             ///< Stores if the view is fullscreen or not.
  LayerContainer mContentLayers;          ///< Layer container.
  unsigned int   mNextLayerIndex;         ///< Next index to be used when a layer is added.
  Layer          mBackgroundLayer;        ///< The background layer.
  Animation      mRotateAnimation;        ///< The animation which rotates the view (and all layers added to it)
  float          mOrientationFunction[4]; ///< The orientation function used to transform from degrees to the internal orientation.
  bool           mAutoRotateEnabled;      ///< Whether the view rotates if the OrientationChanged method is called.

  Toolkit::View::OrientationAnimationStartedSignalV2 mOrientationAnimationStartedSignalV2;
};

} // namespace Internal


// Helpers for public-api forwarding methods

inline Toolkit::Internal::View& GetImpl( Toolkit::View& view )
{
  DALI_ASSERT_ALWAYS( view );

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<Toolkit::Internal::View&>( handle );
}

inline const Toolkit::Internal::View& GetImpl( const Toolkit::View& view )
{
  DALI_ASSERT_ALWAYS( view );

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const Toolkit::Internal::View&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_VIEW_H__
