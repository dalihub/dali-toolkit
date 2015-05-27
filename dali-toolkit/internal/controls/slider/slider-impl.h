#ifndef __DALI_TOOLKIT_INTERNAL_SLIDER_H__
#define __DALI_TOOLKIT_INTERNAL_SLIDER_H__

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
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/object/property-array.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/slider/slider.h>
#include <dali-toolkit/public-api/controls/text-controls/text-label.h>

namespace Dali
{

namespace Toolkit
{

class Button;

namespace Internal
{

class Slider;

typedef Dali::IntrusivePtr< Slider > SliderPtr;

/**
 * @copydoc Toolkit::Slider
 */
class Slider : public Control
{
public:

  typedef Property::Array MarkList;

  /**
   * Create a new Slider.
   *
   * @return A public handle to the newly allocated Slider.
   */
  static Dali::Toolkit::Slider New();

public:

  // Properties

  /**
   * Set marks from a list
   *
   * @param[in] marks The list of marks to set
   */
  void SetMarks( const MarkList& marks );

  /**
   * Get the list of marks
   *
   * @return The marks list
   */
  const MarkList& GetMarks() const;

  /**
   * Set if should snap to marks or not
   *
   * @param[in] snap Flag to snap to marks or not
   */
  void SetSnapToMarks( bool snap );

  /**
   * Return if snap to marks is set or not
   *
   * @return If snap to marks is set
   */
  bool GetSnapToMarks() const;

  /**
   * Set the value of the slider
   *
   * @param[in] value The value to set. Will be clamped to [lowerBound .. upperBound]
   */
  void SetValue( float value );

  /**
   * Get the value of the slider
   *
   * @return The current value of the slider
   */
  float GetValue() const;

  /**
   * Set hit region
   *
   * @param[in] region The hit region
   */
  void SetHitRegion( const Vector2& region );

  /**
   * Get hit region
   *
   * @return The hit region
   */
  const Vector2& GetHitRegion() const;

  /**
   * Set backing region
   *
   * @param[in] region The backing region
   */
  void SetBackingRegion( const Vector2& region );

  /**
   * Get backing region
   *
   * @return The backing region
   */
  const Vector2& GetBackingRegion() const;

  /**
   * @brief Set the disable color.
   *
   * @param[in] color The disable color.
   */
  void SetDisableColor( const Vector4& color );

  /**
   * @brief Get disable color
   *
   * @return The disable color
   */
  Vector4 GetDisableColor() const;

  /**
   * Get popup text color
   *
   * @return The popup text color
   */
  Vector4 GetPopupTextColor() const;

  /**
   * Set the value precision to be used for numbers in the slider
   *
   * @param[in] precision The number of decimal places to use for printing numbers
   */
  void SetValuePrecision( int precision );

  /**
   * Get value precision
   *
   * @return The value precision
   */
  int GetValuePrecision() const;

  /**
   * Show the popup
   *
   * @param[in] showPopup The show popup flag
   */
  void SetShowPopup( bool showPopup );

  /**
   * Get show value in popup
   *
   * @return The show value flag
   */
  bool GetShowPopup() const;

  /**
   * Set show value on handle
   *
   * @param[in] showValue The show value flag
   */
  void SetShowValue( bool showValue );

  /**
   * Get show value on handle
   *
   * @return The show value flag
   */
  bool GetShowValue() const;

  /**
   * Set enabled
   *
   * param[in] enabled Set the enabled flag
   */
  void SetEnabled( bool enabled );

  /**
   * Return if enabled or not
   *
   * @return If enabled
   */
  bool IsEnabled() const;

  /**
   * @brief Set the mark tolerance
   *
   * The tolerance is the percentage of the slider width for which snapping to
   * marks occurs
   *
   * @param[in] tolerance The percentage of width for which to snap
   */
  void SetMarkTolerance( float tolerance );

  /**
   * Return the mark tolerance
   *
   * @return The tolerance set for snapping to marks
   */
  float GetMarkTolerance() const;

public:
  //Signals

  /**
   * @copydoc Toolkit::Slider::ValueChangedSignal()
   */
  Toolkit::Slider::ValueChangedSignalType& ValueChangedSignal();

  /**
   * copydoc Toolkit::Slider::SlidingFinishedSignal()
   */
  Toolkit::Slider::ValueChangedSignalType& SlidingFinishedSignal();

  /**
   * @copydoc Toolkit::Slider::MarkSignal()
   */
  Toolkit::Slider::MarkSignalType& MarkSignal();

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
   * Construct a new Slider.
   */
  Slider();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Slider();

  /**
   * @copydoc CustomActorImpl::OnSizeSet( const Vector3& size )
   */
  virtual void OnSizeSet( const Vector3& size );

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

  /**
   * Slider states
   */
  enum SliderState
  {
    NORMAL,
    DISABLED,
    PRESSED,
    FOCUSED
  };

private:

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * Hit region touch event
   *
   * @param[in] actor The actor the event is raised for
   * @param[in] event The touch event info
   * @return If the event is handled or not
   */
  bool OnTouchEvent( Actor actor, const TouchEvent& event );

  /**
   * Pan gesture event
   *
   * @param[in] actor The actor the event is raised for
   * @param[in] gesture The pan event info
   */
  void OnPan( Actor actor, const PanGesture& gesture );

  /**
   * Map a position onto a domain and return the result as a percentage
   *
   * @param[in] point The point to map onto the domain
   * @return The result as a percentage [0..1]
   */
  float MapPercentage( const Vector2& point );

  /**
   * Map a value in the range to a percentage
   *
   * @param[in] point The value in range [lowerBound..upperBound]
   * @return The result as a percentage [0..1]
   */
  float MapValuePercentage( float value );

  /**
   * Convert a point in local hit space into domain space
   *
   * @param[in] x The x position to convert
   * @return The x position in domain space
   */
  float HitSpaceToDomain( float x );

  /**
   * Map a percentage onto the slider's bounds
   *
   * @param[in] percent The current value of slider in percent
   * @param[in] lowerBound The lower bound to map onto
   * @param[in] upperBound The upper bound to map onto
   * @return The value of percent mapped from lowerBound to upperBound
   */
  float MapBounds( float percent, float lowerBound, float upperBound );

  /**
   * Get the range of the valid values the slider handle can move between
   *
   * @param[in] currentSize The current size of the slider
   * @return The range as a domain pair
   */
  Domain CalcDomain( const Vector2& currentSize );

  /**
   * Create the hit region
   *
   * @return The hit region actor
   */
  Actor CreateHitRegion();

  /**
   * Create the backing for the slider
   *
   * @return The backing actor
   */
  ImageActor CreateBacking();

  /**
   * Create the progress backing for the slider
   *
   * @return The backing actor
   */
  ImageActor CreateProgress();

  /**
   * Create the handle for the slider
   *
   * @return The created image handle
   */
  ImageActor CreateHandle();

  /**
   * Create the popup arrow
   *
   * @return The created image handle
   */
  ImageActor CreatePopupArrow();

  /**
   * Create the popup
   *
   * @return The created image handle
   */
  ImageActor CreatePopup();

  /**
   * Create the textview for the popup
   *
   * @return The textview created for the popup
   */
  Toolkit::TextLabel CreatePopupText();

  /**
   * Create the value display for the slider
   *
   * @return The created root actor of the display
   */
  Actor CreateValueDisplay();

  /**
   * Set the skin based on the current state
   */
  void UpdateSkin();

  /**
   * Create all the children
   */
  void CreateChildren();

  /**
   * Create value popup
   */
  void AddPopup();

  /**
   * Remove value popup
   */
  void RemovePopup();

  /**
   * Display the popup for a set duration with the given value
   *
   * @param[in] value The value to display in the popup
   */
  void DisplayPopup( float value );

  /**
   * If there are marks present, filter the incoming percent based on snapping to any nearby marks
   *
   * @param[in] value The incoming value on the slider to filter
   * @return The filtered percentage snapped to any nearby marks
   */
  float MarkFilter( float value );

  /**
   * If there are marks present, snap the incoming percent to the nearest mark
   *
   * @param[in] value The incoming value on the slider to snap
   * @return The filtered percentage snapped to the nearest mark
   */
  float SnapToMark( float value );

  /**
   * Search for if a mark has been reached
   *
   * @param[in] value The value to search against
   * @param[out] outIndex The index of the mark if found
   * @return If a mark has been found to match percent
   */
  bool MarkReached( float value, int& outIndex );

  /**
   * Handler for when the value view needs to be hidden
   *
   * @return If handled or not
   */
  bool HideValueView();

  /**
   * Set value choosing whether to fire signals or not
   *
   * @paramp[in] value The value to set
   * @param[in] raiseSignals Configure signals to be raised or not.
   */
  void DisplayValue( float value, bool raiseSignals );

  /**
   * Create the image for the backing
   *
   * @param[in] imageName The name of the image to load and set
   */
  void SetBackingImageName( const std::string& imageName );

  /**
   * @brief Return the backing image file name.
   *
   * @return The backing image file name.
   */
  std::string GetBackingImageName();

  /**
   * Create the image for the progress bar
   *
   * @param[in] imageName The name of the image to load and set
   */
  void SetProgressImageName( const std::string& imageName );

  /**
   * @brief Return the progress image name.
   *
   * @return The progress image name if it exists.
   */
  std::string GetProgressImageName();

  /**
   * @brief Create the image for the popup
   *
   * @param[in] imageName The name of the image to load and set
   */
  void CreatePopupImage( const std::string& imageName );

  /**
   * @brief Set the popup name
   *
   * @param[in] imageName The name of the image to set
   */
  void SetPopupImageName( const std::string& imageName );

  /**
   * @brief Return the popup image name.
   *
   * @return The name of the popup image if it exists.
   */
  std::string GetPopupImageName();

  /**
   * @brief Set the popup arrow image name
   *
   * @param[in] imageName The name of the image to set
   */
  void SetPopupArrowImageName( const std::string& imageName );

  /**
   * @brief Return the popup arrow image name.
   *
   * @return The name of the popup image if it exists.
   */
  std::string GetPopupArrowImageName();

  /**
   * Create the image for the popup arrow
   *
   * @param[in] imageName The name of the image to load and set
   */
  void CreatePopupArrowImage( const std::string& imageName );

  /**
   * Set the size of the progress bar region
   *
   * @param[in] region The size of the region to set
   */
  void ResizeProgressRegion( const Vector2& region );

  /**
   * Create the image for the handle
   *
   * @param[in] imageName The name of the image to load and set
   */
  void SetHandleImageName( const std::string& imageName );

  /**
   * @brief Return the handle image name.
   *
   * @return The name of the image handle if it exists.
   */
  std::string GetHandleImageName();

  /**
   * Set the size of the handle region
   *
   * @param[in] region The size of the region to set
   */
  void ResizeHandleRegion( const Vector2& region );

  /**
   * Create and display the value on the handle
   */
  void CreateHandleValueDisplay();

  /**
   * Remove and destroy the handle value display
   */
  void DestroyHandleValueDisplay();

  /**
   * Update the color of the popup text
   *
   * @param[in] color The new color
   */
  void SetPopupTextColor( const Vector4& color );

  /**
   * Set handle region
   *
   * @param[in] region The handle region
   */
  void SetHandleRegion( const Vector2& region );

  /**
   * Get handle region
   *
   * @return The handle region
   */
  const Vector2& GetHandleRegion() const;

  /**
   * Set the lower bound of the slider's value
   *
   * @param[in] bound The value to set for the lower bound
   */
  void SetLowerBound( float bound );

  /**
   * Get the lower bound of the slider's value
   *
   * @return The lower bound value
   */
  float GetLowerBound() const;

  /**
   * Set the upper bound of the slider's value
   *
   * @param[in] bound The value to set for the upper bound
   */
  void SetUpperBound( float bound );

  /**
   * Get the upper bound of the slider's value
   *
   * @return The upper bound value
   */
  float GetUpperBound() const;

private:

  // Undefined
  Slider( const Slider& );

  // Undefined
  Slider& operator=( const Slider& rhs );

private:

  Domain mDomain;                           ///< Current domain of the handle

  Actor mHitArea;                           ///< The input handler
  ImageActor mBacking;                      ///< Backing image
  ImageActor mHandle;                       ///< Slider handle
  ImageActor mProgress;                     ///< Progress backing
  Actor mValueDisplay;                      ///< Display of the value
  ImageActor mPopup;                        ///< Popup backing
  ImageActor mPopupArrow;                   ///< Popup arrow backing

  Toolkit::TextLabel mValueTextLabel;         //< The text value in popup
  Toolkit::TextLabel mHandleValueTextLabel;   ///< The text value on handle
  Vector2 mHandleLastTouchPoint;            ///< The last touch point for the handle
  Timer mValueTimer;                        ///< Timer used to hide value view

  Toolkit::Slider::ValueChangedSignalType mValueChangedSignal;    ///< Signal emitted when the value is changed
  Toolkit::Slider::ValueChangedSignalType mSlidingFinishedSignal;    ///< Signal emitted when a sliding is finished
  Toolkit::Slider::MarkSignalType mMarkSignal;                    ///< Signal emitted when a mark is reached

  SliderState mState;                 ///< The state of the slider

  PanGestureDetector mPanDetector;    ///< Hit region pan detector

  MarkList mMarks;                    ///< List of discreet marks

  std::string mPopupImageName;      ///< Image name for popup image
  std::string mPopupArrowImageName; ///< Image name for popup arrow

  Vector4 mDisableColor;    ///< The color to tint the slider when disabled
  Vector4 mPopupTextColor;  ///< The color of the popup text

  Vector2 mHitRegion;     ///< Size of hit region
  Vector2 mBackingRegion; ///< Size of backing region
  Vector2 mHandleRegionSize;  ///< Size of the handle region

  float mLowerBound;        ///< Lower bound on value
  float mUpperBound;        ///< Upper bound on value
  float mValue;             ///< Current value of slider

  float mMarkTolerance;     ///< Tolerance in percentage of slider width for which to snap to marks

  int mValuePrecision;      ///< The precision to use for outputting the value

  bool mShowPopup   : 1,      ///< Show the popup or not
       mShowValue   : 1,      ///< Whether to display the value number or not on the handle
       mSnapToMarks : 1;      ///< Turn on or off snapping to marks
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::Slider& GetImpl( Toolkit::Slider& pub )
{
  DALI_ASSERT_ALWAYS( pub );

  Dali::RefObject& handle = pub.GetImplementation();

  return static_cast< Toolkit::Internal::Slider& >( handle );
}

inline const Toolkit::Internal::Slider& GetImpl( const Toolkit::Slider& pub )
{
  DALI_ASSERT_ALWAYS( pub );

  const Dali::RefObject& handle = pub.GetImplementation();

  return static_cast< const Toolkit::Internal::Slider& >( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_SLIDER_H__
