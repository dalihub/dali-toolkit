#ifndef __DALI_TOOLKIT_TOGGLE_BUTTON_H__
#define __DALI_TOOLKIT_TOGGLE_BUTTON_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/buttons/button.h>

namespace Dali
{

namespace Toolkit
{

// Forward declarations

namespace Internal DALI_INTERNAL
{
// Forward declarations

class ToggleButton;
}
/**
 * @addtogroup dali_toolkit_controls_buttons
 * @{
 */

/**
 * @brief A ToggleButton allows the user to change a setting between two or more states.
 *
 * By default a ToggleButton emits a Button::StateChangedSignal() signal when it's clicked.
 *
 * Usage example: -
 *
 * @code
 *
 * void ToggleButtonExample::Create( Application& application )
 * {
 *   ToggleButton button = ToggleButton::New();
 *   button.SetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS, Property::Array()
 *                       .Add( "A.png" )
 *                       .Add( "B.png" )
 *                       .Add( "C.png" ));
 *  or
 *
 *  Property::Map propertyMap1;
 *  Vector4 testColor1( 1.f, 0.5f, 0.3f, 0.2f );
 *  propertyMap1.Insert(Visual::Property::TYPE,  Visual::COLOR);
 *  propertyMap1.Insert(ColorVisual::Property::MIX_COLOR,  testColor1);
 *
 *  Property::Map propertyMap2;
 *  Vector4 testColor2( 0.5f, 1.f, 0.3f, 0.2f );
 *  propertyMap2.Insert(Visual::Property::TYPE,  Visual::COLOR);
 *  propertyMap2.Insert(ColorVisual::Property::MIX_COLOR,  testColor2);
 *
 *  Property::Map propertyMap3;
 *  Vector4 testColor3( 1.f, 0.5f, 1.f, 0.2f );
 *  propertyMap3.Insert(Visual::Property::TYPE,  Visual::COLOR);
 *  propertyMap3.Insert(ColorVisual::Property::MIX_COLOR,  testColor3);
 *
 *  button.SetProperty( Toolkit::ToggleButton::Property::STATE_VISUALS, Property::Array()
 *                       .Add( propertyMap1 )
 *                       .Add( propertyMap2 )
 *                       .Add( propertyMap3 ));
 *
 * button.SetProperty( Toolkit::ToggleButton::Property::TOOLTIPS, Property::Array()
 *                       .Add( "STATE A" )
 *                       .Add( "STATE B" )
 *                       .Add( "STATE C" ));
 *
 *   Stage::GetCurrent().Add( button );
 *
 *   // Connect to button signals emitted by the button
 *   button.ClickedSignal().Connect( this, &ToggleButtonExample::OnButtonClicked );
 * }
 *
 * bool ToggleButtonExample::OnButtonClicked( Button button )
 * {
 *   // Do something when the button is clicked
 *   return true;
 * }
 * @endcode
 *
 * See Button for more detail on signals and modifying states via properties.
 */
class DALI_IMPORT_API ToggleButton : public Button
{
public:
  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Button::PROPERTY_END_INDEX + 1,    ///< Toggle button start index
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000        ///< Reserving 1000 property indices
  };

  /**
   * @brief An enumeration of properties belonging to the ToggleButton class.
   */
  struct Property
  {
    enum
    {
      /**
       * @brief The state visual array of toggle button.
       * @details Name "stateVisuals",  type Property::Array.
       * It's a property array of property-maps or a property array of strings,
       * property map expects a description of visual and
       * string represents an image url.
       * @note Mandatory
       */
      STATE_VISUALS = PROPERTY_START_INDEX,

      /**
       * @brief The tooltips of toggle button.
       * @details Name "tooltips",  type Property::Array.
       * It's an array of toggle state tooltip strings.
       * Each tooltip string should match a toggle state strictly.
       * @note Mandatory
       */
      TOOLTIPS,

      /**
       * @brief The current state index of toggle button.
       * @details Name "currentStateIndex",  type integer.
       * It just provides a property to get current state index.
       * @note Optional
       * @note The index is automatically changed when toggle button is clicked.
       */
      CURRENT_STATE_INDEX
    };
  };

public:

  /**
   * @brief Create an uninitialized ToggleButton; this can be initialized with ToggleButton::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ToggleButton();

  /**
   * @brief Copy constructor.
   * @param[in] toggleButton Handle to an object
   */
  ToggleButton( const ToggleButton& toggleButton );

  /**
   * @brief Assignment operator.
   * @param[in] toggleButton Handle to an object
   * @return A reference to this
   */
  ToggleButton& operator=( const ToggleButton& toggleButton );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ToggleButton();

  /**
   * @brief Create an initialized ToggleButton.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static ToggleButton New();

  /**
   * @brief Downcast a handle to ToggleButton handle.
   *
   * If handle points to a ToggleButton the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return handle to a ToggleButton or an uninitialized handle
   */
  static ToggleButton DownCast( BaseHandle handle );

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL ToggleButton( Internal::ToggleButton& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  DALI_INTERNAL ToggleButton( Dali::Internal::CustomActor* internal );
  /// @endcond
};

/**
 * @}
 */
} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TOGGLE_BUTTON_H__
