#ifndef __DALI_TOOLKIT_INTERNAL_TOGGLE_BUTTON_H__
#define __DALI_TOOLKIT_INTERNAL_TOGGLE_BUTTON_H__

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property-array.h>


// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/buttons/toggle-button.h>
#include "button-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * ToggleButton implementation class.
 *
 * \sa Dali::Toolkit::ToggleButton
 */
class ToggleButton : public Button
{
public:

  /**
   * Create a new ToggleButton.
   * @return A smart-pointer to the newly allocated ToggleButton.
   */
  static Dali::Toolkit::ToggleButton New();

protected:

  /**
   * Construct a new ToggleButton.
   */
  ToggleButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ToggleButton();

public:

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

private:

  /**
   * Called to create all toggle visuals and save them to mToggleVisuals.
   * @param[in] states The array store toggle states.
   * @param[out] visuals The created state visual vector.
   */
  void CreateVisualsForAllStates( const Property::Array& states, std::vector<Toolkit::Visual::Base>& visuals );

  /**
   * Called to set toggle states when TOGGLE_STATES is set in SetProperty function.
   * @param[in] states The array store toggle states.
   */
  void SetToggleStates( const Property::Array& states );

  /**
   * Called to retrieve toggle states.
   * @return The toggle states array.
   */
  Property::Array GetToggleStates() const;

  /**
   * Called to set toggle tooltips when TOGGLE_TIPS is set in SetProperty function.
   * @param[in] tips The array store toggle tips.
   */
  void SetToggleTooltips( std::vector<std::string>& tips );

  /**
   * Called to retrieve toggle tips.
   * @return The toggle tips array.
   */
  const std::vector<std::string>& GetToggleTooltips() const;

  /**
   * Called to prepare visual for next state.
   * @param[in] index The property index to set.
   * @param[in] visual The visual to set.
   */
  void PrepareVisual(Property::Index index, Toolkit::Visual::Base& visual);

  /**
   * Called to relayout visual.
   * @param[in] index The index of visual to relayout.
   * @param[in] size The size of control.
   */
  void RelayoutVisual( Property::Index index, const Vector2& size );

private: // From Button

  /**
   * @copydoc Toolkit::Internal::Button::OnInitialize
   */
  virtual void OnInitialize();

  /**
   * @copydoc Toolkit::Internal::Button::OnRelayout
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * This method is called when the button is pressed.
   */
  virtual void OnPressed();

private:

  // Undefined
  ToggleButton( const ToggleButton& );

  // Undefined
  ToggleButton& operator=( const ToggleButton& );

private:

  Property::Array mToggleStates;                              ///< Toggle states, string or map.
  std::vector<Toolkit::Visual::Base> mToggleVisuals;          ///< Save all unselected visuals.
  std::vector<Toolkit::Visual::Base> mToggleSelectedVisuals;  ///< Save all selected visuals.
  std::vector<Toolkit::Visual::Base> mToggleDisabledVisuals;  ///< Save all disabled unselected visuals.
  std::vector<Toolkit::Visual::Base> mToggleDisabledSelectedVisuals;  ///< Save all disabled selected visuals.
  std::vector<std::string> mToggleTooltips;               ///< Toggle tooltips.
  unsigned int             mCurrentToggleIndex;       ///< The index of state.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::ToggleButton& GetImplementation( Toolkit::ToggleButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::ToggleButton&>( handle );
}

inline const Toolkit::Internal::ToggleButton& GetImplementation( const Toolkit::ToggleButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::ToggleButton&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TOGGLE_BUTTON_H__
