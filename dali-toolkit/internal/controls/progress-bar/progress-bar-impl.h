#ifndef DALI_TOOLKIT_INTERNAL_PROGRESS_BAR_H
#define DALI_TOOLKIT_INTERNAL_PROGRESS_BAR_H

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
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/progress-bar/progress-bar.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ProgressBar;

typedef Dali::IntrusivePtr< ProgressBar > ProgressBarPtr;

/**
 * @copydoc Toolkit::ProgressBar
 */
class ProgressBar : public Control
{
public:

  /**
   * Create a new ProgressBar.
   *
   * @return A public handle to the newly allocated ProgressBar.
   */
  static Dali::Toolkit::ProgressBar New();

public:

  // Properties

  /**
   * Set the value of the ProgressBar
   *
   * @param[in] value The value to set. Will be clamped to [lowerBound .. upperBound]
   */

  void SetProgressValue( float value );

  /**
   * Get the value of the ProgressBar
   *
   * @return The current value of the ProgressBar
   */
  float GetProgressValue() const;

public:
  //Signals

  /**
   * @copydoc Toolkit::ProgressBar::ValueChangedSignal()
   */
  Toolkit::ProgressBar::ValueChangedSignalType& ValueChangedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName,
                               FunctorDelegate* functor );

  // Properties

  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

protected:

  /**
   * Construct a new ProgressBar.
   */
  ProgressBar();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ProgressBar();

  /**
   * @copydoc CustomActorImpl::OnRelayout()
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * @copydoc CustomActorImpl::GetNaturalSize()
   */
  virtual Vector3 GetNaturalSize();

private:

  /**
   * Domain is a from/to pair
   */
  struct Domain
  {
    Vector2 from;
    Vector2 to;

    Domain()
    {
    }
    Domain( Vector2 fromVal, Vector2 toVal )
        : from( fromVal ), to( toVal )
    {
    }
  };

private:

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * Get the range of the valid values the ProgressBar handle can move between
   *
   * @param[in] currentSize The current size of the ProgressBar
   * @return The range as a domain pair
   */
  Domain CalcDomain( const Vector2& currentSize );

  /**
   * Create the track for the ProgressBar
   *
   * @return The track actor
   */
  Toolkit::ImageView CreateTrack();

  /**
   * Create the progress track for the ProgressBar
   *
   * @return The track actor
   */
  Toolkit::ImageView CreateProgress();

  /**
   * Create all the children
   */
  void CreateChildren();

  /**
   * Set value choosing whether to fire signals or not
   *
   * @paramp[in] value The value to set
   * @param[in] raiseSignals Configure signals to be raised or not.
   */
  void DisplayValue( float value, bool raiseSignals );

  /**
   * Create the image for the track
   *
   * @param[in] filename The track image
   */
  void SetTrackVisual( const std::string& filename );

  /**
   * @brief Set the track visual from an Dali::Property::Map
   *
   * @param[in] map The Dali::Property::Map to use for to display
   */
  void SetTrackVisual( Dali::Property::Map map );

  /**
   * @brief Return the track image.
   *
   * @return The track image.
   */
  std::string GetTrackVisual();

  /**
   * Create the image for the progress bar
   *
   * @param[in] filename The progress bar image
   */
  void SetProgressVisual( const std::string& filename );

  /**
   * @brief Set the progress visual from an Dali::Property::Map
   *
   * @param[in] map The Dali::Property::Map to use for to display
   */
  void SetProgressVisual( Dali::Property::Map map );

  /**
   * @brief Return the progress bar image.
   *
   * @return The progress bar image if it exists.
   */
  std::string GetProgressVisual();

private:

  // Undefined
  ProgressBar( const ProgressBar& );

  // Undefined
  ProgressBar& operator=( const ProgressBar& rhs );

private:

  Domain mDomain;                           ///< Current domain of the handle

  Toolkit::ImageView mTrack;                ///< Track image
  Toolkit::ImageView mProgress;             ///< Progress bar
  Toolkit::TextLabel mValueTextLabel;       ///< Text value to show progress percentage
  Toolkit::ProgressBar::ValueChangedSignalType mValueChangedSignal;       ///< Signal emitted when the value is changed

  std::string mTrackVisual;           ///< Image for track image
  std::string mProgressVisual;        ///< Image for progress bar image

  Property::Map mTrackMap;         ///< the Property::Map if the image came from a Property::Map, empty otherwise
  Property::Map mProgressMap;      ///< the Property::Map if the image came from a Property::Map, empty otherwise

  Vector2 mTrackVisualSize;      ///< Size of the track image used
  Vector2 mProgressVisualSize;   ///< Size of progress image used

  float mValue;             ///< Current value of ProgressBar
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::ProgressBar& GetImpl( Toolkit::ProgressBar& pub )
{
  DALI_ASSERT_ALWAYS( pub );

  Dali::RefObject& handle = pub.GetImplementation();

  return static_cast< Toolkit::Internal::ProgressBar& >( handle );
}

inline const Toolkit::Internal::ProgressBar& GetImpl( const Toolkit::ProgressBar& pub )
{
  DALI_ASSERT_ALWAYS( pub );

  const Dali::RefObject& handle = pub.GetImplementation();

  return static_cast< const Toolkit::Internal::ProgressBar& >( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_PROGRESS_BAR_H
