#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_BASE_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_BASE_H
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
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali-toolkit/devel-api/layouting/child-layout-data.h>
#include <dali-toolkit/devel-api/layouting/layout-base.h>
#include <dali-toolkit/devel-api/layouting/layout-parent-impl.h>
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

class LayoutBase;
using LayoutBasePtr = IntrusivePtr<LayoutBase>;

class DALI_IMPORT_API LayoutBase : public BaseObject
{
public:
  LayoutBase();

protected:
  ~LayoutBase()=default;

public:
  /**
   * @param[in] owner The owner (container view / child view / visual ) of this layout
   */
  static LayoutBasePtr New( Handle& owner );

  LayoutBase(const LayoutBase& copy)=delete;
  LayoutBase& operator=(const LayoutBase& rhs)=delete;

  void Initialize( Handle& owner, const std::string& containerType );

  /**
   * @brief Initialization method for deriving classes to override
   * @note Deriving types MUST chain up for all child properties to be registered.
   */
  virtual void DoInitialize();

  /**
   * Get a handle to the owner of this layout
   */
  Handle GetOwner() const;

  /**
   * Unparent this layout from it's owner, and remove any layout children in derived types
   */
  void Unparent();

  /**
   * Allow deriving classes to remove layout children when unparented
   */
  virtual void DoUnparent(){};

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
  bool GetAnimateLayout();

  /**
   * Register child properties of layout with owner type.
   *
   * The Actor hierarchy uses these registered properties in the type
   * system to ensure child custom properties are properly initialized.
   *
   * @param[in] containerType The type of the containing view (owner)
   */
  void RegisterChildProperties( const std::string& containerType );

  /**
   * Ensure derived types register their child properties with the owner
   *
   * @param[in] containerType The type name of the owner container
   * @note Deriving types MUST chain up for all child properties to be registered.
   */
  virtual void DoRegisterChildProperties( const std::string& containerType );


  /**
   * This is called to find out how big a view should be. The parent
   * supplies constraint information in the width and height parameters.
   *
   * The actual measurement work of a view is performed in
   * {@link #OnMeasure(MeasureSpec, MeasureSpec)}, called by this method. Therefore, only
   * {@link #OnMeasure(MeasureSpec, MeasureSpec)} can and must be overridden by subclasses.
   *
   * @param widthMeasureSpec Horizontal space requirements as imposed by the parent
   * @param heightMeasureSpec Vertical space requirements as imposed by the parent
   *
   * @see #OnMeasure(MeasureSpec, MeasureSpec)
   */
  void Measure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec );

  /**
   * @brief Assign a size and position to a view and all of its descendants
   *
   * This is the second phase of the layout mechanism.  (The first is
   * measuring). In this phase, each parent calls layout on all of its
   * children to position them.  This is typically done using the
   * child measurements that were stored in the measure pass.
   *
   * Derived classes should not override this method.  Derived classes
   * with children should override OnLayout. In that method, they
   * should call Layout on each of their children.
   *
   * @param[in] l Left position, relative to parent
   * @param[in] t Top position, relative to parent
   * @param[in] r Right position, relative to parent
   * @param[in] b Bottom position, relative to parent
   */
  void Layout( LayoutLength l, LayoutLength t, LayoutLength r, LayoutLength b );

  /**
   * Utility to return a default size. Uses the supplied size if the
   * MeasureSpec imposed no constraints. Will get larger if allowed
   * by the MeasureSpec.
   *
   * @param size Default size for this view
   * @param measureSpec Constraints imposed by the parent
   * @return The size this view should be.
   */
  static LayoutLength GetDefaultSize( LayoutLength size, MeasureSpec measureSpec );

  /**
   * Get the layout object that contains this LayoutBase.
   */
  virtual LayoutParent* GetParent();

  /**
   * Request that this layout is re-laid out
   */
  void RequestLayout();


  /**
   * Get the measured width (without any measurement flags). This
   * method should be used only during measurement and layout calculations.
   *
   * Use {@link Dali::Actor::GetTargetSize()} to see how wide a control is after layout
   */
  LayoutLength GetMeasuredWidth();

  /**
   * Get the measured height (without any measurement flags). This
   * method should be used only during measurement and layout calculations.
   *
   * Use {@link Dali::Actor::GetTargetSize()} to see how high a control is after layout
   */
  LayoutLength GetMeasuredHeight();

  /**
   * Get the measured width and state This method should be used only
   * during measurement and layout calculations.
   *
   * Use {@link Dali::Actor::GetTargetSize()} to see how wide a view is after layout
   */
  MeasuredSize GetMeasuredWidthAndState();

  /**
   * Get the measured height and state. This method should be used
   * only during measurement and layout calculations.
   *
   * Use {@link Dali::Actor::GetTargetSize()} to see how high a view is after layout
   */
  MeasuredSize GetMeasuredHeightAndState();

  /**
   * Returns the suggested minimum width that the view should use. This
   * returns the maximum of the view's minimum width
   * and the background's minimum width
   *
   * When being used in {@link #OnMeasure()}, the caller should still
   * ensure the returned width is within the requirements of the parent.
   *
   * @return The suggested minimum width of the view.
   */
  LayoutLength GetSuggestedMinimumWidth();

  /**
   * Returns the suggested minimum height that the view should use. This
   * returns the maximum of the view's minimum height
   * and the background's minimum height
   *
   * When being used in {@link #OnMeasure()}, the caller should still
   * ensure the returned height is within the requirements of the parent.
   *
   * @return The suggested minimum height of the view.
   */
  LayoutLength GetSuggestedMinimumHeight();

  /**
   * Sets the minimum width of the view. It is not guaranteed the view will
   * be able to achieve this minimum width (for example, if its parent layout
   * constrains it with less available width).
   *
   * @param minWidth The minimum width the view will try to be, in pixels
   *
   * @see #GetMinimumWidth()
   */
  void SetMinimumWidth( LayoutLength minWidth );

  /**
   * Sets the minimum height of the view. It is not guaranteed the view will
   * be able to achieve this minimum height (for example, if its parent layout
   * constrains it with less available height).
   *
   * @param minHeight The minimum height the view will try to be, in pixels
   *
   * @see #GetMinimumHeight()
   */
  void SetMinimumHeight( LayoutLength minHeight );

  /**
   * Returns the minimum width of the view.
   *
   * @return the minimum width the view will try to be, in pixels
   *
   * @see #SetMinimumWidth(LayoutLength)
   */
  LayoutLength GetMinimumWidth();

  /**
   * Returns the minimum height of the view.
   *
   * @return the minimum height the view will try to be, in pixels
   *
   * @see #SetMinimumHeight(LayoutLength)
   */
  LayoutLength GetMinimumHeight();


protected:
  /**
   * Measure the view and its content to determine the measured width and the
   * measured height. This method is invoked by {@link #Measure(MeasureSpec, MeasureSpec)} and
   * should be overridden by subclasses to provide accurate and efficient
   * measurement of their contents.
   *
   * <strong>CONTRACT:</strong> When overriding this method, you
   * <em>must</em> call {@link #SetMeasuredDimensions(MeasuredSize,MeasuredSize)} to store the
   * measured width and height of this view. Failure to do so will trigger an
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
   * sure the measured height and width are at least the view's minimum height
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
   * Called from layout when this view should assign a size and
   * position to each of its children.
   *
   * Derived classes with children should override this method and
   * call Layout() on each of their children.
   *
   * @param[in] changed This is a new size or position for this view
   * @param[in] left Left position, relative to parent
   * @param[in] top Top position, relative to parent
   * @param[in] right Right position, relative to parent
   * @param[in] bottom Bottom position, relative to parent
   */
  virtual void OnLayout( bool changed, LayoutLength left, LayoutLength top, LayoutLength right, LayoutLength bottom );


  /**
   * This method must be called by {@link #OnMeasure(MeasureSpec,MeasureSpec)} to store the
   * measured width and measured height. Failing to do so will trigger an exception at measurement time.
   *
   * @param[in] measuredWidth The measured width of this view. This may have a state of {@link MeasuredSize::MEASURED_SIZE_TOO_SMALL}
   * @param[in] measuredHeight The measured height of this view. This may have a state of {@link MeasuredSize::MEASURED_SIZE_TOO_SMALL}
   */
  void SetMeasuredDimensions( MeasuredSize measuredWidth, MeasuredSize measuredHeight );

  /**
   * @brief
   */
  static MeasuredSize ResolveSizeAndState( LayoutLength size, MeasureSpec measureSpec, MeasuredSize::State childMeasuredState );

  /**
   * @brief Sets the frame (the size and position) of the layout onto it's owner
   *
   * @todo Consider instead, collating properties into LayoutCollector in order to set/animate them all in one block.
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

private:
  /**
   * @brief Called to change the size of the layout.
   * @param[in] newSize The new size of the layout
   * @param[in] oldSize The old size of the layout
   */
  void SizeChange( LayoutSize newSize, LayoutSize oldSize );

  /**
   * Triggered when a layout animation finished
   * @param[in] animation  A handle to the layout animation
   */
  void OnLayoutAnimationFinished( Animation& animation );

public:
  class Impl; // Class declaration is public so we can add devel API's in the future


private:
  std::unique_ptr<Impl> mImpl; ///< Implementation class holds all the data
  SlotDelegate<LayoutBase> mSlotDelegate; ///< Slot delegate allows this class to connect safely to signals
};

} //namespace Internal

inline Internal::LayoutBase& GetImplementation( Dali::Toolkit::LayoutBase& handle )
{
  DALI_ASSERT_ALWAYS( handle && "LayoutBase handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast< Internal::LayoutBase& >( object );
}

inline const Internal::LayoutBase& GetImplementation( const Dali::Toolkit::LayoutBase& handle )
{
  DALI_ASSERT_ALWAYS( handle && "LayoutBase handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast< const Internal::LayoutBase& >( object );
}

} //namespace Toolkit
} //namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUTING_LAYOUT_BASE_H
