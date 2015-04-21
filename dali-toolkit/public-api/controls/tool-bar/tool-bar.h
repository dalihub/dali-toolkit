#ifndef __DALI_TOOLKIT_TOOL_BAR_H__
#define __DALI_TOOLKIT_TOOL_BAR_H__

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
#include <string>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/alignment/alignment.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
// Forward declarations
class ToolBar;
}

/**
 * Provides a tool bar where other controls (Dali::Actor) could be placed.
 * controls could be added into three different groups: on the left, center or right.
 * AddControl() and RemoveControl() methods should be used to add and remove controls. The use of Actor's Dali::Actor::Add() method
 * is not forbidden, it adds controls on the left group with a size of 10% of the total tool bar size.
 * Dali::Actor::Remove() method does nothing.
 */
class DALI_IMPORT_API ToolBar : public Control
{
public:
  static const Toolkit::Alignment::Padding DEFAULT_PADDING; ///< Default padding space between controls. By default all values are set to 0.

public:
  /**
   * Create a ToolBar handle; this can be initialised with ToolBar::New()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  ToolBar();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   * @param handle to copy from
   */
  ToolBar( const ToolBar& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  ToolBar& operator=( const ToolBar& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ToolBar();

  /**
   * Create an initialized ToolBar.
   * @return A handle to a newly allocated Dali resource.
   */
  static ToolBar New();

  /**
   * Downcast an Object handle to ToolBar. If handle points to a ToolBar the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ToolBar or an uninitialized handle
   */
  static ToolBar DownCast( BaseHandle handle );

  /**
   * Sets a background image.
   * @param background Actor with the tool bar background.
   */
  void SetBackground( Actor background );

  /**
   * Adds an additional control to the tool bar.
   * @pre The tool bar needs to be initialized.
   * @pre The alignment needs to be horizontal.
   * @param control An Actor with the additional control.
   * @param relativeSize Control's size as a percentage of the tool bar width.
   * @param alignment Where to insert controls. Possible values are Toolkit::Alignment::Left, Toolkit::Alignment::Center or Toolkit::Alignment::Right.
   * @param padding Padding values used for the added control (left, right, top, bottom). By default, no padding is added.
   */
  void AddControl( Actor control, float relativeSize, Toolkit::Alignment::Type alignment, const Toolkit::Alignment::Padding& padding = DEFAULT_PADDING );

  /**
   * Removes a control from the tool bar.
   * @pre control must have been added before to this tool bar.
   * @param control The control to be removed.
   */
  void RemoveControl( Actor control );

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL ToolBar( Internal::ToolBar& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL ToolBar( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TOOL_BAR_H__
