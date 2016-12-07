#ifndef DALI_INTERNAL_TOOLTIP_H
#define DALI_INTERNAL_TOOLTIP_H

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
#include <string>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/devel-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <dali-toolkit/devel-api/controls/tooltip/tooltip-properties.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class Tooltip;
typedef IntrusivePtr< Tooltip > TooltipPtr;

/**
 * @brief Handles all the required tooltip related functionality for a control.
 *
 * Connects to the Hovered signal of the control.
 * Styling is achieved by merging the properties set so that new properties override previous but existing properties are still kept.
 */
class Tooltip : public RefObject, public ConnectionTracker
{
public:

  /**
   * @brief Creates an instance of the Tooltip class.
   * @param[in]  control  The control the tooltip should be shown on.
   */
  static TooltipPtr New( Toolkit::Control control );

  /**
   * @brief Sets the properties of the Tooltip.
   * @details The properties are merged over the currently stored properties.
   *          If a Property::STRING is passed, then the style set previously by the stylesheet is used.
   *          If a Property::MAP then the map is merged.
   *          If a Property::ARRAY of Visuals then all are displayed in one row.
   * @param[in]  value  This can either be a Property::STRING, Property::MAP or Property::ARRAY.
   */
  void SetProperties( const Property::Value& value );

  /**
   * @brief Creates a property map of the tooltip properties.
   * @param[out]  map  Filled with all the properties of the tooltip.
   * @note map should be empty.
   */
  void CreatePropertyMap( Property::Map& map ) const;

private:

  /**
   * @brief Private constructor.
   */
  Tooltip( Toolkit::Control control );

  /**
   * @brief Private destructor.
   */
  ~Tooltip();

  Tooltip( const Tooltip& ); ///< Undefined
  Tooltip& operator=( const Tooltip& ); ///< ///< Undefined

  /**
   * @brief Sets the content of the tooltip.
   * @details Connects to the signals if there is real content to display.
   * @param[in]  control  Is used to connect to this control's signals.
   * @param[in]  value    The content property value.
   */
  void SetContent( Toolkit::Control& control, const Property::Value& value );

  /**
   * @brief Sets the background properties of the tooltip.
   * @param[in]  value  The background property value.
   */
  void SetBackground( const Property::Value& value );

  /**
   * @brief Sets the tail properties of the tooltip.
   * @param[in]  value  The tail property value.
   */
  void SetTail( const Property::Value& value );

  /**
   * @brief Method used to connect to the control's Hovered signal.
   * @param[in]  hover  The hover event.
   */
  bool OnHovered( Actor /* actor */, const HoverEvent& hover );

  /**
   * @brief Method used to connect to the internal timer used by Tooltip.
   * @return Always return false as we're only interested in one timeout.
   */
  bool OnTimeout();

  /**
   * @brief Used to know when the we're laying out the actor used to display the tooltip.
   * @details This is required so we can appropriately position it.
   * @param[in]  actor  The actor being laid out.
   */
  void OnRelayout( Actor actor );

  // Data

  Toolkit::Popup mPopup; ///< The Popup class is used to display the actual tooltip.
  Timer mTooltipTimer; ///< Timer used to wait a certain length of time before we display the tooltip.

  WeakHandle< Toolkit::Control > mControl; ///< A weak handle to the control we are setting the tooltip on.

  Property::Map mContentTextVisual; ///< If using just one visual, then this is set.
  Property::Map mTailImages; ///< The different images used by the tail.
  Property::Array mContentArray; ///< If using an array of visuals, then this is used.

  Rect< int > mBackgroundBorder; ///< The size of the background border in the order: left, right, bottom, top. @see Toolkit::Tooltip::Border::Property::BORDER

  Vector2 mLayout; ///< The layout of the content if using an array.
  Vector2 mHoverPoint; ///< The first point where hover starts.
  Vector2 mHoverPointOffset; ///< The tooltip is displayed with this offset from hover point if using Toolkit::Tooltip::Position::HOVER_POINT.

  std::string mBackgroundImage; ///< The path to the background image used for the tooltip.

  float mMovementThreshold; ///< This is the allowable movement of the hover before the tooltip processing is cancelled.

  int mWaitTime; ///< Time in milliseconds to wait before we display the tooltip.

  Toolkit::Tooltip::Position::Type mPositionType; ///< The position of the tooltip.
  bool mTailVisibility; ///< Whether we are showing a tail or not.
  bool mDisappearOnMovement; ///< Whether the tooltip is set to disappear on movement or when we go out of the bounds of mControl.
  bool mSignalsConnected; ///< Whether any signals required for Tooltip functionality have been connected.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_INTERNAL_TOOLTIP_H
