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


// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali/public-api/animation/animation.h>
#include <dali-toolkit/devel-api/controls/progress-bar/progress-bar.h>
#include <dali-toolkit/devel-api/visual-factory/transition-data.h>

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

  /**
   * Set the secondary progress value of the ProgressBar
   *
   * @param[in] value The secondary progress value to set. Will be clamped to [lowerBound .. upperBound]
   */
  void SetSecondaryProgressValue( float value );

  /**
   * Get the secondary progress value of the ProgressBar
   *
   * @return The current secondary progress value of the ProgressBar
   */
  float GetSecondaryProgressValue() const;

  /**
   * Set the indeterminate state of the ProgressBar
   *
   * @param[in] value The value to set.
   */
  void SetIndeterminate( bool value );

  /**
   * Get the indeterminate state value of the ProgressBar
   *
   * @return The current determined state of the ProgressBar
   */
  bool GetIndeterminate() const;

  /**
   * Set the indeterminate visual transition of the ProgressBar
   *
   * @param[in] Transition data map to set.
   */
  void SetIndeterminateVisualTransition( Property::Map transtion );

  /**
   * Get the indeterminate visual transition data map of the ProgressBar
   *
   * @return The current indeterminate visual transition data map of the ProgressBar
   */
  Property::Map GetIndeterminateVisualTransition() const;

  /**
   * Play the indeterminate visual transition
   */
  void PlayIndeterminateVisualTransition();

public:
  //Signals

  /**
   * @copydoc toolkit::progressbar::valuechangedsignal()
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
   * Set indeterminate visual transition animation
   */
  void SetIndeterminateVisualTransition( Toolkit::TransitionData transtion );

  /**
   * Convert value to transition data
   */
  Toolkit::TransitionData ConvertPropertyToTransition( const Property::Value& value );

  /**
   * Update progress bar label when progress value is changed
   */
  void CreateVisualsForComponent( Property::Index index, const Property::Value& value, const float visualDepth );

  /**
   * Update progress bar label when progress value is changed
   */
  bool GetPropertyMapForVisual( Property::Index visualIndex, Property::Map& retreivedMap ) const;

  /**
   * Check if we should start animating
   */
  virtual void OnStageConnection( int depth );

private:

  // Undefined
  ProgressBar( const ProgressBar& );

  // Undefined
  ProgressBar& operator=( const ProgressBar& rhs );

private:

  Domain mDomain;                                                     ///< Current domain of the handle

  Animation mIndeterminateVisualAni;                                  ///< Animation for indetrminate visual. Transition animation.
  Toolkit::ProgressBar::ValueChangedSignalType mValueChangedSignal;   ///< Signal emitted when the value is changed

  Toolkit::TransitionData mIndeterminateVisualTransition;             ///< Transition data map for mIndeterminateVisualAni
  float mProgressValue;                                               ///< Current value of ProgressBar
  float mSecondaryProgressValue;                                      ///< Current loading value of ProgressBar
  bool mIndeterminate;                                                ///< Whether the progress state is determined or not
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
