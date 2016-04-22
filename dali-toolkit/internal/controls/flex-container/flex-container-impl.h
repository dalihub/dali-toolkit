#ifndef __DALI_TOOLKIT_INTERNAL_FLEX_CONTAINER_H__
#define __DALI_TOOLKIT_INTERNAL_FLEX_CONTAINER_H__

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
#include <dali/devel-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/devel-api/controls/flex-container/flex-container.h>
extern "C"
{
#include "layout.h"
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * FlexContainer is a custom control for laying out actors in a flexbox layout
 * @see Dali::Toolkit:FlexContainer for more details
 */
class FlexContainer : public Control
{
public:

  /**
   * The structure to store the style properties and layout information of flex item
   */
  struct FlexItemNode
  {
    WeakHandle< Dali::Actor > actor;      ///< Actor handle of the flex item
    css_node_t* node;                     ///< The style properties and layout information
  };

  typedef std::vector< FlexItemNode > FlexItemNodeContainer;

public:

  /**
   * Construct a new FlexContainer.
   */
  FlexContainer();

  /**
   * Create a new FlexContainer.
   * @return A smart-pointer to the newly allocated FlexContainer.
   */
  static Toolkit::FlexContainer New();

  /**
   * @brief Set the primary direction in which content is ordered.
   * @param[in] contentDirection The direction of the content.
   */
  void SetContentDirection(Toolkit::FlexContainer::ContentDirection contentDirection);

  /**
   * @brief Get the direction of the content.
   * @return The direction of the content.
   */
  Toolkit::FlexContainer::ContentDirection GetContentDirection();

  /**
   * @brief Set the direction flex items are laid out.
   * @param[in] flexDirection The direction flex items are laid out.
   */
  void SetFlexDirection(Toolkit::FlexContainer::FlexDirection flexDirection);

  /**
   * @brief Get the direction flex items are laid out.
   * @return The direction flex items are laid out.
   */
  Toolkit::FlexContainer::FlexDirection GetFlexDirection();

  /**
   * @brief Set whether the flex items should wrap or not, if there
   * is no enough room for them on one flex line.
   * @param[in] flexWrap The wrap type.
   */
  void SetFlexWrap(Toolkit::FlexContainer::WrapType flexWrap);

  /**
   * @brief Get whether the flex items should wrap or not, if there
   * is no enough room for them on one flex line.
   * @return The wrap type.
   */
  Toolkit::FlexContainer::WrapType GetFlexWrap();

  /**
   * @brief Set the horizontal alignment of the flex items when the items
   * do not use all available space on the main-axis.
   * @param[in] justifyContent The horizontal alignment of flex items.
   */
  void SetJustifyContent(Toolkit::FlexContainer::Justification justifyContent);

  /**
   * @brief Get the horizontal alignment of the flex items when the items
   * do not use all available space on the main-axis.
   * @return The horizontal alignment of flex items.
   */
  Toolkit::FlexContainer::Justification GetJustifyContent();

  /**
   * @brief Set the vertical alignment of the flex items when the items
   * do not use all available space on the cross-axis.
   * @param[in] alignItems The vertical alignment of flex items.
   */
  void SetAlignItems(Toolkit::FlexContainer::Alignment alignItems);

  /**
   * @brief Get the vertical alignment of the flex items when the items
   * do not use all available space on the cross-axis.
   * @return The vertical alignment of flex items.
   */
  Toolkit::FlexContainer::Alignment GetAlignItems();

  /**
   * @brief Set the vertical alignment of the flex lines when the lines
   * do not use all available space on the cross-axis.
   * @param[in] alignItems The vertical alignment of flex lines.
   */
  void SetAlignContent(Toolkit::FlexContainer::Alignment alignContent);

  /**
   * @brief Get the vertical alignment of the flex lines when the lines
   * do not use all available space on the cross-axis.
   * @return The vertical alignment of flex lines.
   */
  Toolkit::FlexContainer::Alignment GetAlignContent();

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
  static Property::Value GetProperty( BaseObject* object, Property::Index index );

private: // From Control

  /**
   * @copydoc Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc Control::OnChildAdd(Actor& child)
   */
  virtual void OnChildAdd( Actor& child );

  /**
   * @copydoc Control::OnChildRemove(Actor& child)
   */
  virtual void OnChildRemove( Actor& child );

  /**
   * @copydoc Control::OnRelayout
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * @copydoc Control::RelayoutDependentOnChildren()
   */
  virtual bool RelayoutDependentOnChildren( Dimension::Type dimension = Dimension::ALL_DIMENSIONS );

  /**
   * @copydoc Control::GetNextKeyboardFocusableActor
   */
  virtual Actor GetNextKeyboardFocusableActor( Actor currentFocusedActor, Toolkit::Control::KeyboardFocus::Direction direction, bool loopEnabled );

  /**
   * @copydoc CustomActorImpl::OnSizeSet( const Vector3& size )
   */
  virtual void OnSizeSet( const Vector3& size );

  /**
   * @copydoc CustomActorImpl::OnSizeAnimation(Animation&, const Vector3&)
   */
  virtual void OnSizeAnimation(Animation& animation, const Vector3& targetSize);

private: // Implementation

  /**
   * Calculate the layout properties of all the children
   */
  void ComputeLayout();

  /**
   * Calculate the layout of the children and relayout them with their new size and position
   */
  void RelayoutChildren();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~FlexContainer();

private:

  // Undefined copy constructor and assignment operators
  FlexContainer(const FlexContainer&);
  FlexContainer& operator=(const FlexContainer& rhs);

private: // Data

  FlexItemNode mRootNode;                    ///< Style properties and layout information of flex container
  FlexItemNodeContainer mChildrenNodes;      ///< Style properties and layout information of flex items in the container

  Toolkit::FlexContainer::ContentDirection mContentDirection;        ///< The content direction of the container
  Toolkit::FlexContainer::FlexDirection mFlexDirection;              ///< The flex direction of the container
  Toolkit::FlexContainer::WrapType mFlexWrap;                        ///< The wrap type of the container
  Toolkit::FlexContainer::Justification mJustifyContent;             ///< The alignment of flex items in the container on the main-axis
  Toolkit::FlexContainer::Alignment mAlignItems;                     ///< The alignment of flex items in the container on the cross-axis
  Toolkit::FlexContainer::Alignment mAlignContent;                   ///< The alignment of flex lines in the container on the cross-axis
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::FlexContainer& GetImpl( Toolkit::FlexContainer& tableView )
{
  DALI_ASSERT_ALWAYS(tableView);

  Dali::RefObject& handle = tableView.GetImplementation();

  return static_cast<Toolkit::Internal::FlexContainer&>(handle);
}

inline const Toolkit::Internal::FlexContainer& GetImpl( const Toolkit::FlexContainer& tableView )
{
  DALI_ASSERT_ALWAYS(tableView);

  const Dali::RefObject& handle = tableView.GetImplementation();

  return static_cast<const Toolkit::Internal::FlexContainer&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_FLEX_CONTAINER_H__
