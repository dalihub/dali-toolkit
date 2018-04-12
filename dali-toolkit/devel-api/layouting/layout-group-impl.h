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
#include <dali-toolkit/devel-api/layouting/layout-base-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class LayoutGroup;
using LayoutGroupPtr = IntrusivePtr<LayoutGroup>;

/**
 * LayoutGroup is an abstract class that provides child layout management and
 * basic measuring and layouting.
 *
 * Implementing classes should ensure that they initialize the object with
 * the handle of the control that owns the layout.
 */
class DALI_IMPORT_API LayoutGroup : public LayoutBase, public LayoutParent, public ConnectionTracker
{
public:
  LayoutGroup();

protected:
  ~LayoutGroup();

public:
  LayoutGroup( const LayoutGroup& copy ) = delete;
  LayoutGroup& operator=( const LayoutGroup& rhs ); // once wrapped up should use = delete;

  static Dali::Toolkit::LayoutGroup New( LayoutGroupPtr layoutGroup );

  Toolkit::LayoutGroup::LayoutId Add( LayoutBase& layoutData );
  void Remove( Toolkit::LayoutGroup::LayoutId childId );
  void Remove( LayoutBase& child );

  LayoutBasePtr GetChild( Toolkit::LayoutGroup::LayoutId childId );
  unsigned int GetChildCount();

  /**
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
  static MeasureSpec GetChildMeasureSpec( MeasureSpec measureSpec, int padding, uint16_t childDimension );

  /**
   * Check if the layout has already been requested
   */
  bool IsLayoutRequested();

  /**
   * Get the padding information
   */
  Extents GetPadding();

public: // Implementation of LayoutParent
  virtual LayoutParent* GetParent();

protected:
  /**
   * @copydoc LayoutBase::DoRegisterChildProperties()
   */
  virtual void DoRegisterChildProperties( const std::type_info& containerType ) override;

  /**
   * Callback when a child property is set on any given child
   * @param[in] handle The handle to the child
   * @param[in] index The index of the property that has been set
   * @param[in] value The new value of the property
   */
  void OnSetChildProperties( Handle& handle, Property::Index index, Property::Value value );

  /**
   * Create default layout data suitable for this layout group or derived layouter
   */
  virtual void GenerateDefaultChildProperties( Handle child );

  /**
   * Ask all of the children of this view to measure themselves, taking into
   * account both the MeasureSpec requirements for this view and its padding.
   * We skip children that are in the GONE state The heavy lifting is done in
   * getChildMeasureSpec.
   *
   * @param widthMeasureSpec The width requirements for this view
   * @param heightMeasureSpec The height requirements for this view
   */
  virtual void MeasureChildren( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec );

  /**
   * Ask one of the children of this view to measure itself, taking into
   * account both the MeasureSpec requirements for this view and its padding.
   * The heavy lifting is done in getChildMeasureSpec.
   *
   * @param child The child to measure
   * @param parentWidthMeasureSpec The width requirements for this view
   * @param parentHeightMeasureSpec The height requirements for this view
   */
  void MeasureChild( LayoutBasePtr child, MeasureSpec parentWidthMeasureSpec, MeasureSpec parentHeightMeasureSpec );

  /**
   * Ask one of the children of this view to measure itself, taking into
   * account both the MeasureSpec requirements for this view and its padding
   * and margins. The child must have MarginLayoutParams The heavy lifting is
   * done in getChildMeasureSpec.
   *
   * @param child The child to measure
   * @param parentWidthMeasureSpec The width requirements for this view
   * @param widthUsed Extra space that has been used up by the parent
   *        horizontally (possibly by other children of the parent)
   * @param parentHeightMeasureSpec The height requirements for this view
   * @param heightUsed Extra space that has been used up by the parent
   *        vertically (possibly by other children of the parent)
   */
  virtual void MeasureChildWithMargins( LayoutBasePtr child,
                                        MeasureSpec parentWidthMeasureSpec,
                                        uint16_t widthUsed,
                                        MeasureSpec parentHeightMeasureSpec,
                                        uint16_t heightUsed );
public:
  class Impl;

private:
  std::unique_ptr<Impl> mImpl;
};

}//namespace Internal

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
