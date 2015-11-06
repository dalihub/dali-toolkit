#ifndef __DALI_TOOLKIT_TEXT_SELECTION_TOOLBAR_H__
#define __DALI_TOOLKIT_TEXT_SELECTION_TOOLBAR_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class TextSelectionToolbar;
}

/**
 * @brief A control which provides a Popup with a number of buttons
 *
 * The style of the pop can be set through style sheets, this includes the images for the buttons
 * A Show and Hide API is provided.
 *
 * If the buttons exceed the size constraints of the popup then it will offer scrolling.
 *
 *
 */
class DALI_IMPORT_API TextSelectionToolbar : public Control
{
public:

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the TextSelectionToolbar class.
   */
  struct Property
  {
    enum
    {
      MAX_SIZE =  PROPERTY_START_INDEX, ///< name "maxSize",                 The maximum size the Popup can be,              type VECTOR2
      ENABLE_OVERSHOOT,                 ///< name "enableOvershoot",         Whether the overshoot image is enabled,         type BOOLEAN
      SCROLL_VIEW,                      ///< name "scrollView",              Properties to set on scroll view                type Property::Map
    };
  };

  /**
   * Create the TextSelectionToolbar control.
   * @return A handle to the TextSelectionToolbar control.
   */
  static TextSelectionToolbar New();

  /**
   * @brief Creates an empty handle.
   */
  TextSelectionToolbar();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle The handle to copy from.
   */
  TextSelectionToolbar( const TextSelectionToolbar& handle );

  /**
   * @brief Assignment operator.
   *
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  TextSelectionToolbar& operator=( const TextSelectionToolbar& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~TextSelectionToolbar();


  /**
   * @brief Add a option to the the Tool bar
   * @param[in] option Option actor to add
   */
  void AddOption( Actor& option );

  /**
   * @brief Add a divider to the the Tool bar
   * @param[in] divider Actor to be used as divider
   */
  void AddDivider( Actor& divider );

  /**
   *  @brief ResizeDividers
   *  @param[in] size New size of dividers, provide 0 for height to automatically set height
   */
  void ResizeDividers( Size& size );

  /**
   * @brief Raises the layer above the given @e target layer.
   *
   * @param[in] target The layer to get above of.
   */
  void RaiseAbove( Layer target );

  /**
   * @brief Downcast a handle to TextSelectionToolbar.
   *
   * If the BaseHandle points is a TextSelectionToolbar the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @param[in] handle Handle to an object
   * @return handle to a TextSelectionToolbar or an empty handle
   */
  static TextSelectionToolbar DownCast( BaseHandle handle );

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in] implementation The Control implementation.
   */
  DALI_INTERNAL TextSelectionToolbar( Internal::TextSelectionToolbar& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL TextSelectionToolbar( Dali::Internal::CustomActor* internal );

}; // Class TextSelectionToolbar

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_TEXT_SELECTION_TOOLBAR_H__
