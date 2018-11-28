#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_ITEM_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_ITEM_H
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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/animation/animation.h>
#include <dali-toolkit/devel-api/layouting/child-layout-data.h>
#include <dali-toolkit/devel-api/layouting/layout-item.h>
#include <dali-toolkit/devel-api/layouting/layout-child-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-controller.h>
#include <dali-toolkit/devel-api/layouting/layout-size.h>
#include <dali-toolkit/devel-api/layouting/measure-spec.h>
#include <dali-toolkit/devel-api/layouting/measured-size.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

struct LayoutData;

class LayoutItem;
using LayoutItemPtr = IntrusivePtr<LayoutItem>;

class LayoutTransitionData;
using LayoutTransitionDataPtr = IntrusivePtr<LayoutTransitionData>;

/**
 * Base class for layouts.
 */
class DALI_TOOLKIT_API LayoutItem : public BaseObject,
                                    public LayoutChild
{
public:
  /**
   * Constructor.
   */
  LayoutItem();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~LayoutItem();

public:

/**
   * @brief Construct
   *
   * @param[in] owner The owner (container view / child view / visual ) of this layout
   * @return a new LayoutItem object
   */
  static LayoutItemPtr New( Handle& owner );

  /**
   * @brief Remove the default copy constructor
   */
  LayoutItem(const LayoutItem& copy)=delete;

  /**
   * @brief Remove the default assignment operator
   */
  LayoutItem& operator=(const LayoutItem& rhs)=delete;

  /**
   * @brief Initialize the layout with it's owner and owner's type name
   * @param[in] owner a handle to the owner container
   * @param[in] containerType the type name of the owner container
   */
  void Initialize( Handle& owner, const std::string& containerType );

  /**
   * @brief Get a handle to the owner of this layout
   *
   * @return a handle to the owner of this layout
   */
  Handle GetOwner() const;

  /**
   * @brief Unparent this layout from it's parent, remove it from it's owner
   * and remove any layout children in derived types.
   */
  void Unparent();

  /**
   * @brief Set whether this layout should be animated or not
   *
   * @param[in] animateLayout True if the layout should be animated when applied
   */
  void SetAnimateLayout( bool animateLayout );

  /**
   * @brief Get whether this layout should be animated or not
   *
   * @return True if the layout should be animated when applied
   */
  bool IsLayoutAnimated() const;

  /**
   * @brief Get the default transition
   *
   * @return The default transition
   */
  LayoutTransitionDataPtr GetDefaultTransition();

  /**
   * @brief Set the layout transition data
   * @param[in] layoutTransitionType The type of the transition
   * @param[in] layoutTransitionDataPtr The transition data pointer
   */
  void SetTransitionData( int layoutTransitionType, LayoutTransitionDataPtr layoutTransitionDataPtr );

  /**
   * @brief Get the transition data
   * @param[in] layoutTransitionType The type of the transition
   *
   * @return The transition
   */
  LayoutTransitionDataPtr GetTransitionData( int layoutTransitionType ) const;

  /**
   * @brief  This is called to find out how big a layout should be.
   *
   * The parent supplies constraint information in the width and height parameters.
   *
   * The actual measurement work of a layout is performed in
   * {@link #OnMeasure(MeasureSpec, MeasureSpec)}, called by this method. Therefore, only
   * {@link #OnMeasure(MeasureSpec, MeasureSpec)} can and must be overridden by subclasses.
   *
   * @param[in] widthMeasureSpec Horizontal space requirements as imposed by the parent
   * @param[in] heightMeasureSpec Vertical space requirements as imposed by the parent
   *
   * @see #OnMeasure(MeasureSpec, MeasureSpec)
   */
  void Measure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec );

  /**
   * @brief Assign a size and position to a layout and all of its descendants.
   *
   * This is the second phase of the layout mechanism.  (The first is measuring). In this phase, each parent
   * calls layout on all of its children to position them.  This is typically done using the child
   * measurements that were stored in the measure pass.
   *
   * Derived classes with children should override OnLayout. In that method, they should call Layout on each
   * of their children.
   *
   * @param[in] left position, relative to parent
   * @param[in] top position, relative to parent
   * @param[in] right position, relative to parent
   * @param[in] bottom position, relative to parent
   */
  void Layout( LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom );

  /**
   * @brief Utility to return a default size.
   *
   * Uses the supplied size if the MeasureSpec imposed no constraints. Will get larger if allowed by the
   * MeasureSpec.
   *
   * @param[in] size Default size for this layout
   * @param[in] measureSpec Constraints imposed by the parent
   * @return The size this layout should be.
   */
  static LayoutLength GetDefaultSize( LayoutLength size, MeasureSpec measureSpec );

  /**
   * @copydoc LayoutChild::SetParent
   */
  void SetParent( LayoutParent* parent ) override;

  /**
   * @copydoc LayoutChild::GetParent
   */
  LayoutParent* GetParent() override;

  /**
   * @brief Request that this layout is re-laid out.
   *
   * This will make this layout and all it's parent layouts dirty.
   */
  void RequestLayout();

  /**
   * @brief Request that this layout is re-laid out with particular transition.
   * @param[in] layoutTranstionType The transition type
   *
   * This will make this layout and all it's parent layouts dirty and set the transition queued.
   */
  void RequestLayout( Dali::Toolkit::LayoutTransitionData::Type layoutTranstionType );

  /**
   * @brief Request that this layout is re-laid out with particular transition.
   * @param[in] layoutTranstionType The transition type
   * @param[in] gainedChild The gained owners's child due to add/remove or focus gained/lost
   * @param[in] lostChild The lost owners's child due to add/remove or focus gained/lost
   *
   * This will make this layout and all it's parent layouts dirty and set the transition queued.
   */
  void RequestLayout( Dali::Toolkit::LayoutTransitionData::Type layoutTranstionType, Actor gainedChild, Actor lostChild );

  /**
   * @brief Predicate to determine if this layout has been requested to re-layout
   *
   * @return True if a layout request has occured on this layout
   */
  bool IsLayoutRequested() const;

  /**
   * @brief Set layout requested flag (mark the layout dirty).
   */
  void SetLayoutRequested();

  /**
   * @brief Checks if the Resize policy is being used for this LayoutItem
   * @return true is ResizePolicy is used
   */
  bool IsResizePolicyRequired() const;

  /**
   * @brief Sets if the ResizePolicy is needed or not for this LayoutItem
   * @param[in] resizeRequired true or false flag
   */
  void SetResizePolicyRequired( bool resizeRequired );

  /**
   * @brief Get the measured width (without any measurement flags).
   *
   * This method should be used only during measurement and layout calculations.
   *
   * Use {@link Dali::Actor::GetTargetSize()} to see how wide a control is after layout
   */
  LayoutLength GetMeasuredWidth() const;

  /**
   * @brief Get the measured height (without any measurement flags).
   *
   * This method should be used only during measurement and layout calculations.
   *
   * Use {@link Dali::Actor::GetTargetSize()} to see how high a control is after layout
   */
  LayoutLength GetMeasuredHeight() const;

  /**
   * @brief Get the measured width and state.
   *
   * This method should be used only during measurement and layout calculations.
   *
   * Use {@link Dali::Actor::GetTargetSize()} to see how wide a view is after layout
   */
  MeasuredSize GetMeasuredWidthAndState() const;

  /**
   * @brief Get the measured height and state.
   *
   * This method should be used only during measurement and layout calculations.
   *
   * Use {@link Dali::Actor::GetTargetSize()} to see how high a view is after layout
   */
  MeasuredSize GetMeasuredHeightAndState() const;

  /**
   * @brief Returns the suggested minimum width that the layout should use.
   *
   * This returns the maximum of the layout's minimum width and the background's minimum width
   *
   * When being used in {@link #OnMeasure()}, the caller should still
   * ensure the returned width is within the requirements of the parent.
   *
   * @return The suggested minimum width of the layout.
   */
  LayoutLength GetSuggestedMinimumWidth() const;

  /**
   * @brief Returns the suggested minimum height that the layout should use.
   *
   * This returns the maximum of the layout's minimum height and the background's minimum height
   *
   * When being used in {@link #OnMeasure()}, the caller should still
   * ensure the returned height is within the requirements of the parent.
   *
   * @return The suggested minimum height of the layout.
   */
  LayoutLength GetSuggestedMinimumHeight() const;

  /**
   * @brief Sets the minimum width of the layout.
   *
   * It is not guaranteed the layout will be able to achieve this minimum width (for example, if its parent
   * layout constrains it with less available width).
   *
   * @param[in] minWidth The minimum width the layout will try to be, in pixels
   *
   * @see #GetMinimumWidth()
   */
  void SetMinimumWidth( LayoutLength minWidth );

  /**
   * @brief Sets the minimum height of the layout.
   *
   * It is not guaranteed the layout will be able to achieve this minimum height (for example, if its parent
   * layout constrains it with less available height).
   *
   * @param[in] minHeight The minimum height the layout will try to be, in pixels
   *
   * @see #GetMinimumHeight()
   */
  void SetMinimumHeight( LayoutLength minHeight );

  /**
   * @brief Returns the minimum width of the layout.
   *
   * @return the minimum width the layout will try to be, in pixels
   *
   * @see #SetMinimumWidth(LayoutLength)
   */
  LayoutLength GetMinimumWidth() const;

  /**
   * @brief Returns the minimum height of the layout.
   *
   * @return the minimum height the layout will try to be, in pixels
   *
   * @see #SetMinimumHeight(LayoutLength)
   */
  LayoutLength GetMinimumHeight() const;

  /**
   * Get the padding information.
   * @return The padding information
   */
  Extents GetPadding() const;

  /**
   * Get the margin information.
   * @return The margin information
   */
  Extents GetMargin() const;

protected:
  /**
   * @brief Allow directly deriving classes to remove layout children when unparented
   */
  virtual void OnUnparent(){};

  /**
   * @brief Ensure direct derived types register their child properties with the owner
   *
   * @param[in] containerType The type name of the owner container
   */
  virtual void OnRegisterChildProperties( const std::string& containerType );

  /**
   * @brief Measure the layout and its content to determine the measured width and the
   * measured height.
   *
   * This method is invoked by {@link #Measure(MeasureSpec, MeasureSpec)} and
   * should be overridden by subclasses to provide accurate and efficient
   * measurement of their contents.
   *
   * <strong>CONTRACT:</strong> When overriding this method, you
   * <em>must</em> call {@link #SetMeasuredDimensions(MeasuredSize,MeasuredSize)} to store the
   * measured width and height of this layout. Failure to do so will trigger an
   * <code>IllegalStateException</code>, thrown by
   * {@link #Measure(MeasureSpec,MeasureSpec)}. Calling the superclass'
   * {@link #OnMeasure(MeasureSpec,MeasureSpec)} is a valid use.
   *
   * The base class implementation of measure defaults to the background size,
   * unless a larger size is allowed by the MeasureSpec. Subclasses should
   * override {@link #OnMeasure(MeasureSpec,MeasureSpec)} to provide better measurements of
   * their content.
   *
   * If this method is overridden, it is the subclass's responsibility to make
   * sure the measured height and width are at least the layout's minimum height
   * and width ({@link #GetSuggestedMinimumHeight()} and
   * {@link #GetSuggestedMinimumWidth()}).
   *
   * @param[in] widthMeasureSpec horizontal space requirements as imposed by the parent.
   *                             The requirements are encoded with
   *                             {@link MeasureSpec}.
   * @param[in] heightMeasureSpec vertical space requirements as imposed by the parent.
   *                              The requirements are encoded with
   *                              {@link MeasureSpec}.
   *
   * @see #GetMeasuredWidth()
   * @see #GetMeasuredHeight()
   * @see #GetSuggestedMinimumHeight()
   * @see #GetSuggestedMinimumWidth()
   * @see MeasureSpec#GetMode(int)
   * @see MeasureSpec#GetSize(int)
   */
  virtual void OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec );

  /**
   * @brief Called from Layout() when this layout should assign a size and position to each of its children.
   *
   * Derived classes with children should override this method and call Layout() on each of their children.
   *
   * @param[in] changed This is a new size or position for this layout
   * @param[in] left Left position, relative to parent
   * @param[in] top Top position, relative to parent
   * @param[in] right Right position, relative to parent
   * @param[in] bottom Bottom position, relative to parent
   */
  virtual void OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom );


  /**
   * @brief This method must be called by {@link #OnMeasure(MeasureSpec,MeasureSpec)} to store the
   * measured width and measured height.
   *
   * Failing to do so will trigger an exception at measurement time.
   *
   * @param[in] measuredWidth The measured width of this layout. This may have a state of
   * {@link MeasuredSize::MEASURED_SIZE_TOO_SMALL}
   *
   * @param[in] measuredHeight The measured height of this layout. This may have a state of
   * {@link MeasuredSize::MEASURED_SIZE_TOO_SMALL}
   */
  void SetMeasuredDimensions( MeasuredSize measuredWidth, MeasuredSize measuredHeight );

  /**
   * @brief Utility to reconcile a desired size and state, with constraints imposed by a MeasureSpec.
   *
   * @param[in] size How big the layout wants to be.
   * @param[in] measureSpec Constraints imposed by the parent.
   * @param[in] childMeasuredState Size information bit mask for the layout's children.
   *
   * @return A measured size, which may indicate that it is too small.
   */
  static MeasuredSize ResolveSizeAndState( LayoutLength size, MeasureSpec measureSpec,
                                           MeasuredSize::State childMeasuredState );

  /**
   * @brief Sets the frame (the size and position) of the layout onto it's owner.
   * Collect all properties to animate after the layout update.
   *
   * @param[in] left The horizontal position of the left edge of this frame within the parent layout
   * @param[in] top The vertical position of the top edge of this frame within the parent layout
   * @param[in] right The horizontal position of the right edge of this frame within the parent layout
   * @param[in] bottom The vertical position of the bottom edge of this frame within the parent layout
   */
  bool SetFrame( LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom );

  /**
   * Virtual method to inform derived classes when the layout size changed
   * @param[in] newSize The new size of the layout
   * @param[in] oldSize The old size of the layout
   */
  virtual void OnSizeChanged( LayoutSize newSize, LayoutSize oldSize );


  /**
   * @brief Initialization method for LayoutGroup to override
   */
  virtual void OnInitialize();

  /**
   * @brief Called when a layer animation state is changed.
   */
  virtual void OnAnimationStateChanged( bool animateLayout ) {}

private:
  /**
   * @brief Called to change the size of the layout.
   *
   * @param[in] newSize The new size of the layout
   * @param[in] oldSize The old size of the layout
   */
  void SizeChange( LayoutSize newSize, LayoutSize oldSize );

  /**
   * @brief Triggered when a layout animation finished.
   *
   * @param[in] animation  A handle to the layout animation
   */
  void OnLayoutAnimationFinished( Animation& animation );

  /**
   * @brief Register child properties of layout with owner type.
   *
   * The Actor hierarchy uses these registered properties in the type
   * system to ensure child custom properties are properly initialized.
   *
   * @param[in] containerType The type of the containing view (owner)
   */
  void RegisterChildProperties( const std::string& containerType );

public:
  class Impl; // Class declaration is public so we can add devel API's in the future

private:
  std::unique_ptr<Impl> mImpl; ///< Implementation class holds all the data
  SlotDelegate<LayoutItem> mSlotDelegate;
};

} //namespace Internal

inline Internal::LayoutItem& GetImplementation( Dali::Toolkit::LayoutItem& handle )
{
  DALI_ASSERT_ALWAYS( handle && "LayoutItem handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast< Internal::LayoutItem& >( object );
}

inline const Internal::LayoutItem& GetImplementation( const Dali::Toolkit::LayoutItem& handle )
{
  DALI_ASSERT_ALWAYS( handle && "LayoutItem handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast< const Internal::LayoutItem& >( object );
}

} //namespace Toolkit
} //namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_ITEM_H
