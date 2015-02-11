#ifndef __DALI_TOOLKIT_INTERNAL_TOOL_BAR_H__
#define __DALI_TOOLKIT_INTERNAL_TOOL_BAR_H__

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
#include <dali/public-api/common/map-wrapper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/tool-bar/tool-bar.h>
#include <dali-toolkit/public-api/controls/table-view/table-view.h>

namespace Dali
{

namespace Toolkit
{

class ToolBar;

namespace Internal
{

/**
 * ToolBar is a control to create a tool bar.
 * @see Dali::Toolkit::ToolBar for more details.
 */
class ToolBar : public Control
{
public:

  /**
   * Create an initialized ToolBar.
   * @return A handle to a newly allocated Dali resource.
   */
  static Toolkit::ToolBar New();

  /**
   * @copydoc Dali::Toolkit::View::SetBackground()
   */
  void SetBackground( Actor background );

  /**
   * @copydoc Dali::Toolkit::View::AddControl()
   */
  void AddControl( Dali::Actor control, float relativeSize, Toolkit::Alignment::Type alignment, const Toolkit::Alignment::Padding& padding );

  /**
   * @copydoc Dali::Toolkit::View::RemoveControl()
   */
  void RemoveControl( Dali::Actor control );

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * Adds a control using some default values (the control uses 10% of the tool bar space and is placed on the left group).
   * @param child The control to be added.
   *
   * @see Control::OnControlChildAdd()
   */
  virtual void OnControlChildAdd(Actor& child);

  /**
   *
   * @copydoc Toolkit::Control::OnControlSizeSet( const Vector3& targetSize )
   */
  virtual void OnControlSizeSet( const Vector3& targetSize );

private:
  /**
   */
  class Lock
  {
  public:
    /**
     * Constructor, sets the lock boolean
     */
    Lock( bool& lock )
    : mLock( lock )
    {
      mLock = true;
    }

    /**
     * Destructor, releases lock boolean
     */
    ~Lock()
    {
      mLock = false;
    }
  private:
    bool& mLock;
  };

  /**
   * Constructor.
   * It initializes ToolBar members.
   */
  ToolBar();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ToolBar();

private:
  Toolkit::TableView mLayout;                   ///< TableView used to place controls.
  unsigned int       mLeftOffset;               ///< Offset index where the next control is going to be added in the left group.
  unsigned int       mCenterBase;               ///< Base index where the first control of the center group is placed.
  unsigned int       mCenterOffset;             ///< Offset index where the next control is going to be added in the center group.
  unsigned int       mRightBase;                ///< Base index where the first control of the right group is placed.
  unsigned int       mRightOffset;              ///< Offset index where the next control is going to be added in the right group.
  float              mLeftRelativeSpace;        ///< Relative space between left and center groups of controls.
  float              mRightRelativeSpace;       ///< Relative space between center and right groups of controls.
  float              mAccumulatedRelativeSpace; ///< Stores the total percentage space used by controls.
  bool               mInitializing;             ///< Allows the use of Actor's API to add controls.
  Vector3            mToolBarSize;              ///< The size of tool bar
  Actor              mBackground;               ///< The background of the tool bar

  std::map<Actor/*control*/,Toolkit::Alignment> mControls; ///< Stores a relationship between controls and their alignments used to place them inside the table view.
};

} // namespace Internal


// Helpers for public-api forwarding methods

inline Toolkit::Internal::ToolBar& GetImpl( Toolkit::ToolBar& toolBar )
{
  DALI_ASSERT_ALWAYS( toolBar );

  Dali::RefObject& handle = toolBar.GetImplementation();

  return static_cast<Toolkit::Internal::ToolBar&>( handle );
}

inline const Toolkit::Internal::ToolBar& GetImpl( const Toolkit::ToolBar& toolBar )
{
  DALI_ASSERT_ALWAYS( toolBar );

  const Dali::RefObject& handle = toolBar.GetImplementation();

  return static_cast<const Toolkit::Internal::ToolBar&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TOOL_BAR_H__
