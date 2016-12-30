#ifndef DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_TOOLBAR_H
#define DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_TOOLBAR_H

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
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
#include <dali-toolkit/public-api/controls/table-view/table-view.h>
#include <dali-toolkit/devel-api/controls/text-controls/text-selection-toolbar.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/layer.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class TextSelectionToolbar : public Control
{
public:

  /**
   * @copydoc Dali::Toollkit::TextSelectionToolbar::New()
   */
  static Toolkit::TextSelectionToolbar New();

  // Properties

  /**
   * @brief Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index index );

  /**
   *  @copydoc Toolkit::TextSelectionToolbar::AddOption()
   */
  void AddOption( Actor& option );

  /**
   *  @copydoc Toolkit::TextSelectionToolbar::AddDivider()
   */
  void AddDivider( Actor& divider );

  /**
   * @copydoc Toolkit::TextSelectionToolbar::ResizeDividers()
   */
  void ResizeDividers( Size& size );

  /**
   * @copydoc Toolkit::TextSelectionToolbar::RaiseAbove()
   */
  void RaiseAbove( Layer target );

private: // From Control

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
  * @copydoc Control::OnRelayout()
  */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * @brief Set max size of Popup
   * @param[in] maxSize Size (Vector2)
   */
  void SetPopupMaxSize( const Size& maxSize );

  /**
   * @brief Get Max size of Popup
   * @return Vector2 the max size of the Popup
   */
  const Dali::Vector2& GetPopupMaxSize() const;

private: // Implementation

  /**
   * @copydoc Toolkit::TextSelectionToolbar::ConfigureScrollview()
   */
  void ConfigureScrollview( const Property::Map& properties );

  /**
   * @brief Set up scrollview to scroll Toolbar horizontally
   */
  void SetUpScrollView();

  /**
   * @brief Set up the parts that make the Toolbar
   */
  void SetUp();

  /**
   * Toolbar has started to scroll
   * @param[in] position current scroll view position
   */
  void OnScrollStarted( const Vector2& position );

  /**
   * Toolbar has stopped scrolling
   * @param[in] position current scroll view position
   */
  void OnScrollCompleted( const Vector2& position );

  /**
   * Construct a new TextField.
   */
  TextSelectionToolbar();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~TextSelectionToolbar();

private:

  // Undefined copy constructor and assignment operators
  TextSelectionToolbar(const TextSelectionToolbar&);
  TextSelectionToolbar& operator=(const TextSelectionToolbar& rhs);

private: // Data

  Layer mToolbarLayer;                                ///< The layer used to house the toolbar.
  Toolkit::TableView mTableOfButtons;                 ///< Actor which holds all the buttons, sensitivity can be set on buttons via this actor
  Toolkit::ScrollView mScrollView;                    ///< Provides scrolling of Toolbar when content does not fit.
  RulerPtr mRulerX;                                   ///< Ruler to clamp horizontal scrolling. Updates on Relayout
  Size mMaxSize;                                      ///< Max size of the Toolbar
  unsigned int mIndexInTable;                         ///< Index in table to add option
  Dali::Vector< unsigned int > mDividerIndexes;       ///< Vector of indexes in the Toolbar that contain dividers.

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::TextSelectionToolbar& GetImpl( Toolkit::TextSelectionToolbar& textSelectionToolbar )
{
  DALI_ASSERT_ALWAYS( textSelectionToolbar );

  Dali::RefObject& handle = textSelectionToolbar.GetImplementation();

  return static_cast<Toolkit::Internal::TextSelectionToolbar&>(handle);
}

inline const Toolkit::Internal::TextSelectionToolbar& GetImpl( const Toolkit::TextSelectionToolbar& textSelectionToolbar )
{
  DALI_ASSERT_ALWAYS( textSelectionToolbar );

  const Dali::RefObject& handle = textSelectionToolbar.GetImplementation();

  return static_cast<const Toolkit::Internal::TextSelectionToolbar&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_TEXT_SELECTION_TOOLBAR_H
