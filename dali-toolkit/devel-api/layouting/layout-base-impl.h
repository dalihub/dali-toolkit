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
#include <dali/public-api/math/uint-16-pair.h>
#include <dali-toolkit/devel-api/layouting/child-layout-data.h>
#include <dali-toolkit/devel-api/layouting/layout-base.h>
#include <dali-toolkit/devel-api/layouting/layout-parent-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-controller.h>
#include <dali-toolkit/devel-api/layouting/measure-spec.h>
#include <dali-toolkit/devel-api/layouting/measured-size.h>
#include <dali-toolkit/internal/layouting/child-layout-data-impl.h>

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
   * @param[in] owner The owner (container view ) of this layout
   */
  static LayoutBasePtr New( IntrusivePtr<RefObject> owner );

  LayoutBase(const LayoutBase& copy)=delete;
  LayoutBase& operator=(const LayoutBase& rhs)=delete;

  virtual void Initialize( IntrusivePtr<RefObject> handle );

  IntrusivePtr<RefObject> GetOwner() const;

  /**
   * Register child properties of layout
   * @param[in] containerType The type of the containing view (owner)
   */
  void RegisterChildProperties( const std::type_info& containerType );

  /**
   * Ensure derived types register their child properties
   * Must chain up to parent.
   */
  virtual void DoRegisterChildProperties( const std::type_info& containerType );

  void SetLayoutData( ChildLayoutDataPtr childLayoutData );

  ChildLayoutDataPtr GetLayoutData();

  /**
   * <p>
   * This is called to find out how big a view should be. The parent
   * supplies constraint information in the width and height parameters.
   * </p>
   *
   * <p>
   * The actual measurement work of a view is performed in
   * {@link #OnMeasure(MeasureSpec, MeasureSpec)}, called by this method. Therefore, only
   * {@link #OnMeasure(MeasureSpec, MeasureSpec)} can and must be overridden by subclasses.
   * </p>
   *
   *
   * @param widthMeasureSpec Horizontal space requirements as imposed by the parent
   * @param heightMeasureSpec Vertical space requirements as imposed by the parent
   *
   * @see #OnMeasure(MeasureSpec, MeasureSpec)
   */
  void Measure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec );

  /**
   * Assign a size and position to a view and all of its descendants
   *
   * <p>This is the second phase of the layout mechanism.  (The first
   * is measuring). In this phase, each parent calls layout on all of
   * its children to position them.  This is typically done using the
   * child measurements that were stored in the measure pass.</p>
   *
   * <p>Derived classes should not override this method.  Derived
   * classes with children should override onLayout. In that method,
   * they should call layout on each of their children.</p>
   *
   * @param[in] l Left position, relative to parent
   * @param[in] t Top position, relative to parent
   * @param[in] r Right position, relative to parent
   * @param[in] b Bottom position, relative to parent
   * @param[in] animate True if the layout should be animated
   */
  void Layout( int l, int t, int r, int b, bool animate );

  /**
   * Utility to return a default size. Uses the supplied size if the
   * MeasureSpec imposed no constraints. Will get larger if allowed
   * by the MeasureSpec.
   *
   * @param size Default size for this view
   * @param measureSpec Constraints imposed by the parent
   * @return The size this view should be.
   */
  static uint16_t GetDefaultSize( uint16_t size, MeasureSpec measureSpec );

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
  uint16_t GetMeasuredWidth();

  /**
   * Get the measured height (without any measurement flags). This
   * method should be used only during measurement and layout calculations.
   *
   * Use {@link Dali::Actor::GetTargetSize()} to see how high a control is after layout
   */
  uint16_t GetMeasuredHeight();

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
   * <p>
   * When being used in {@link #OnMeasure()}, the caller should still
   * ensure the returned width is within the requirements of the parent.
   *
   * @return The suggested minimum width of the view.
   */
  uint16_t GetSuggestedMinimumWidth();

  /**
   * Returns the suggested minimum height that the view should use. This
   * returns the maximum of the view's minimum height
   * and the background's minimum height
   * <p>
   * When being used in {@link #OnMeasure()}, the caller should still
   * ensure the returned height is within the requirements of the parent.
   *
   * @return The suggested minimum height of the view.
   */
  uint16_t GetSuggestedMinimumHeight();

  /**
   * Sets the minimum width of the view. It is not guaranteed the view will
   * be able to achieve this minimum width (for example, if its parent layout
   * constrains it with less available width).
   *
   * @param minWidth The minimum width the view will try to be, in pixels
   *
   * @see #GetMinimumWidth()
   */
  void SetMinimumWidth( uint16_t minWidth );

  /**
   * Sets the minimum height of the view. It is not guaranteed the view will
   * be able to achieve this minimum height (for example, if its parent layout
   * constrains it with less available height).
   *
   * @param minHeight The minimum height the view will try to be, in pixels
   *
   * @see #GetMinimumHeight()
   */
  void SetMinimumHeight( uint16_t minHeight );

  /**
   * Returns the minimum width of the view.
   *
   * @return the minimum width the view will try to be, in pixels
   *
   * @see #SetMinimumWidth(uint16_t)
   */
  uint16_t GetMinimumWidth();

  /**
   * Returns the minimum height of the view.
   *
   * @return the minimum height the view will try to be, in pixels
   *
   * @see #SetMinimumHeight(uint16_t)
   */
  uint16_t GetMinimumHeight();


protected:
  /**
   * <p>
   * Measure the view and its content to determine the measured width and the
   * measured height. This method is invoked by {@link #Measure(MeasureSpec, MeasureSpec)} and
   * should be overridden by subclasses to provide accurate and efficient
   * measurement of their contents.
   * </p>
   *
   * <p>
   * <strong>CONTRACT:</strong> When overriding this method, you
   * <em>must</em> call {@link #SetMeasuredDimensions(MeasuredSize,MeasuredSize)} to store the
   * measured width and height of this view. Failure to do so will trigger an
   * <code>IllegalStateException</code>, thrown by
   * {@link #Measure(MeasureSpec,MeasureSpec)}. Calling the superclass'
   * {@link #OnMeasure(MeasureSpec,MeasureSpec)} is a valid use.
   * </p>
   *
   * <p>
   * The base class implementation of measure defaults to the background size,
   * unless a larger size is allowed by the MeasureSpec. Subclasses should
   * override {@link #OnMeasure(MeasureSpec,MeasureSpec)} to provide better measurements of
   * their content.
   * </p>
   *
   * <p>
   * If this method is overridden, it is the subclass's responsibility to make
   * sure the measured height and width are at least the view's minimum height
   * and width ({@link #GetSuggestedMinimumHeight()} and
   * {@link #GetSuggestedMinimumWidth()}).
   * </p>
   *
   * @param[in] widthMeasureSpec horizontal space requirements as imposed by the parent.
       *                         The requirements are encoded with
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
   * call layout on each of their children.
   *
   * @param[in] changed This is a new size or position for this view
   * @param[in] left Left position, relative to parent
   * @param[in] top Top position, relative to parent
   * @param[in] right Right position, relative to parent
   * @param[in] bottom Bottom position, relative to parent
   * @param[in] animate Whether the layout is about to animate into place
   */
  virtual void OnLayout( bool changed, int left, int top, int right, int bottom, bool animate );

  /**
   * Called after new layout data has been set on this layout.
   *
   * May be overridden by deriving layouters.
   * @param[in] layoutData Base ptr to the layout data
   */
  virtual void OnSetLayoutData( ChildLayoutDataPtr layoutData );

  /**
   * <p>This method must be called by {@link #OnMeasure(MeasureSpec,MeasureSpec)} to store the
   * measured width and measured height. Failing to do so will trigger an
   * exception at measurement time.</p>
   *
   * @param[in] measuredWidth The measured width of this view. This may have a state of {@link MeasuredSize::MEASURED_SIZE_TOO_SMALL}
   * @param[in] measuredHeight The measured height of this view. This may have a state of {@link MeasuredSize::MEASURED_SIZE_TOO_SMALL}
   */
  void SetMeasuredDimensions( MeasuredSize measuredWidth, MeasuredSize measuredHeight );

  static MeasuredSize ResolveSizeAndState( uint16_t size, MeasureSpec measureSpec, uint16_t childMeasuredState );

  bool SetFrame( int left, int top, int right, int bottom );

  virtual void OnSizeChanged( Uint16Pair newSize, Uint16Pair oldSize );

private:
  void SizeChange( Uint16Pair newSize, Uint16Pair oldSize );

public:
  class Impl; // Class declaration is public so we can add devel API's in the future

private:
  std::unique_ptr<Impl> mImpl; ///< Implementation class holds all the data
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
