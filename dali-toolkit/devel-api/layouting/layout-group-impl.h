#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_GROUP_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_GROUP_H
/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

#include <memory>

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali-toolkit/devel-api/layouting/child-layout-data.h>
#include <dali-toolkit/devel-api/layouting/layout-group.h>
#include <dali-toolkit/devel-api/layouting/layout-parent-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class LayoutGroup;
using LayoutGroupPtr = IntrusivePtr<LayoutGroup>;

/**
 * LayoutGroup is an abstract class that provides child layout management and basic measuring and layouting.
 *
 * Deriving classes should override LayoutItem::DoInitialize for second stage initialization,
 * LayoutGroup::DoRegisterChildProperties to register child property types with the owner,
 * LayoutGroup::OnChildAdd to apply default child property values to the child.
 * Deriving classes may override LayoutGroup::OnChildRemove.
 *
 * Deriving classes must also override OnMeasure and OnLayout as follows:
 *
 * OnMeasure should measure each child using LayoutGroup::MeasureChildWithMargins or LayoutGroup::MeasureChild.
 * We recommend calling LayoutItem::ResolveSizeAndState() to resolve measure specs.
 * If some children don't fit, then they can be measured again with different MeasureSpecs as required.
 *
 * After measurement, the derived class must also call SetMeasuredDimensions to set it's own requested size.
 *
 * OnLayout should use it's own layout parameters and the measured children's size to determine the children's
 * position and size; it should then call Layout() on the child layout to layout the child and it's hierarchy.
 */
class DALI_TOOLKIT_API LayoutGroup : public LayoutItem,
                                     public LayoutParent,
                                     public ConnectionTracker
{
public:
  /**
   * Constructor. Returns an initialized object
   */
  LayoutGroup();

protected:
  /**
   * Virtual destructor may only be called by Unreference()
   */
  virtual ~LayoutGroup();

public:
  LayoutGroup( const LayoutGroup& copy ) = delete;
  LayoutGroup& operator=( const LayoutGroup& rhs ) = delete;

  /**
   * @brief Add a layout child to this group.
   *
   * @param[in] layoutChild The child to add
   * @return The layout id of this child.
   */
  Toolkit::LayoutGroup::LayoutId Add( LayoutItem& layoutChild ) override;

  /**
   * @brief Remove a layout child from this group.
   * @param[in] childId The layout child id
   */
  void Remove( Toolkit::LayoutGroup::LayoutId childId ) override;

  /**
   * @brief Remove a layout child from this group
   * @param[in] child The layout child
   */
  void Remove( LayoutItem& child ) override;

  /**
   * @brief Insert a child to the parent
   * @param[in] target The target item
   * @param[in] child The item to insert to this layout parent
   */
  Toolkit::LayoutGroup::LayoutId Insert( LayoutItem& target, LayoutItem& child ) override;

  /**
   * @brief Move a child to another position
   * @param[in] target The target item
   * @param[in] child The item to move
   */
  Toolkit::LayoutGroup::LayoutId Move( LayoutItem& target, LayoutItem& child ) override;

  /**
   * @brief Move a child to back
   * @param[in] child The item to move
   */
  Toolkit::LayoutGroup::LayoutId MoveBack( LayoutItem& child ) override;

  /**
   * @brief Remove all layout children.
   *
   * @note This will not unparent owner's children
   */
  void RemoveAll();


  /**
   * @brief Get the number of children contained by this layout group
   *
   * @return the number of children
   */
  unsigned int GetChildCount() const;

  /**
   * Get the child layout at the given index
   */
  LayoutItemPtr GetChildAt( unsigned int childIndex ) const;

  /**
   * Get the child layout id of the given child
   */
  Toolkit::LayoutGroup::LayoutId GetChildId( LayoutItem& child ) const;

  /**
   * @brief Get the layout child with the given layout id.
   * @note child id's start at 1, and follow the insertion order
   * @param[in] childId the layout id of the child within this group
   * @return A pointer to the child layout
   */
  LayoutItemPtr GetChild( Toolkit::LayoutGroup::LayoutId childId ) const;

  template <typename T>
    LayoutItemPtr GetChild( T childId ) = delete; // Prevent implicit casting of int/uint to LayoutId

  /**
   * Callback when child is added to container.
   * Derived classes can use this to set their own child properties on the child layout's owner.
   */
  virtual void OnChildAdd( LayoutItem& child );

  /**
   * Callback when child is removed from container.
   */
  virtual void OnChildRemove( LayoutItem& child );

  /**
   * @brief Calculate the right measure spec for this child.
   *
   * Does the hard part of MeasureChildren: figuring out the MeasureSpec to
   * pass to a particular child. This method figures out the right MeasureSpec
   * for one dimension (height or width) of one child view.
   *
   * The goal is to combine information from our MeasureSpec with the
   * LayoutParams of the child to get the best possible results. For example,
   * if the this view knows its size (because its MeasureSpec has a mode of
   * EXACTLY), and the child has indicated in its LayoutParams that it wants
   * to be the same size as the parent, the parent should ask the child to
   * layout given an exact size.
   *
   * @param measureSpec The requirements for this view
   *
   * @param padding The padding of this view for the current dimension
   *        and margins, if applicable
   *
   * @param childDimension How big the child wants to be in the
   *        current dimension
   * @return a MeasureSpec for the child
   */
  static MeasureSpec GetChildMeasureSpec( MeasureSpec measureSpec,
                                          LayoutLength padding,
                                          LayoutLength childDimension );

protected:
  /**
   * @brief Second stage initialization method for deriving classes to override
   */
  virtual void DoInitialize();

  /**
   * @brief Method for derived classes to implement in order to register child
   * property types with the container.
   *
   * @param[in] containerType The fully qualified typename of the container
   */
  virtual void DoRegisterChildProperties( const std::string& containerType );

  /**
   * Create default child property values suitable for this layout group or derived layouter
   */
  virtual void GenerateDefaultChildPropertyValues( Handle child );

  /**
   * Ask all of the children of this view to measure themselves, taking into
   * account both the MeasureSpec requirements for this view and its padding.
   * The heavy lifting is done in GetChildMeasureSpec.
   *
   * @param widthMeasureSpec The width requirements for this view
   * @param heightMeasureSpec The height requirements for this view
   */
  virtual void MeasureChildren( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec );

  /**
   * Ask one of the children of this view to measure itself, taking into
   * account both the MeasureSpec requirements for this view and its padding.
   * The heavy lifting is done in GetChildMeasureSpec.
   *
   * @param child The child to measure
   * @param parentWidthMeasureSpec The width requirements for this view
   * @param parentHeightMeasureSpec The height requirements for this view
   */
  virtual void MeasureChild( LayoutItemPtr child, MeasureSpec parentWidthMeasureSpec, MeasureSpec parentHeightMeasureSpec );

  /**
   * Ask one of the children of this view to measure itself, taking into
   * account both the MeasureSpec requirements for this view and its padding
   * and margins. The child must have MarginLayoutParams The heavy lifting is
   * done in GetChildMeasureSpec.
   *
   * @param child The child to measure
   * @param parentWidthMeasureSpec The width requirements for this view
   * @param widthUsed Extra space that has been used up by the parent
   *        horizontally (possibly by other children of the parent)
   * @param parentHeightMeasureSpec The height requirements for this view
   * @param heightUsed Extra space that has been used up by the parent
   *        vertically (possibly by other children of the parent)
   */
  virtual void MeasureChildWithMargins( LayoutItemPtr child,
                                        MeasureSpec parentWidthMeasureSpec,
                                        LayoutLength widthUsed,
                                        MeasureSpec parentHeightMeasureSpec,
                                        LayoutLength heightUsed );

private:
  /**
   * Second stage initialization for LayoutGroup only.
   * This will call DoInitialize on most derived class.
   */
  void OnInitialize() override final;

  /**
   * @copydoc LayoutItem::OnRegisterChildProperties()
   */
  void OnRegisterChildProperties( const std::string& containerType ) override final;

  /**
   * @copydoc LayoutItem::OnUnparent
   */
  void OnUnparent() override final;

  /**
   * Method to remove a child from this group
   */
  void RemoveChild( LayoutItem& item );

  /**
   * Callback when child is added to owner
   */
  void ChildAddedToOwner( Actor child );

  /**
   * Callback when child is removed from owner
   */
  void ChildRemovedFromOwner( Actor child );

  /**
   * Callback when child order is changed
   */
  void ChildOrderChanged( Actor child );

  /**
   * Callback when an owner property is set. Triggers a relayout if it's a child property
   */
  void OnOwnerPropertySet( Handle& handle, Property::Index index, Property::Value value );

  /**
   * Callback when a child property is set on any given child
   * @param[in] handle The handle to the child
   * @param[in] index The index of the property that has been set
   * @param[in] value The new value of the property
   */
  void OnSetChildProperties( Handle& handle, Property::Index index, Property::Value value );

  /**
   * @brief Called when a layer animation state is changed.
   */
  void OnAnimationStateChanged( bool animateLayout ) override final;

public:
  class Impl; // Class declaration is public so we can add devel API's in the future

private:
  std::unique_ptr<Impl> mImpl; // The implementation data for this class.
  SlotDelegate<LayoutGroup> mSlotDelegate; ///< Slot delegate allows this class to connect safely to signals
};

} //namespace Internal

inline Internal::LayoutGroup& GetImplementation( Dali::Toolkit::LayoutGroup& handle )
{
  DALI_ASSERT_ALWAYS( handle && "LayoutGroup handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast< Internal::LayoutGroup& >( object );
}

inline const Internal::LayoutGroup& GetImplementation( const Dali::Toolkit::LayoutGroup& handle )
{
  DALI_ASSERT_ALWAYS( handle && "LayoutGroup handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast< const Internal::LayoutGroup& >( object );
}

}//namespace Toolkit
}//namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_GROUP_H
