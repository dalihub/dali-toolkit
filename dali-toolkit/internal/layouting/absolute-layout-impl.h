#ifndef DALI_TOOLKIT_INTERNAL_ABSOLUTE_LAYOUT_H
#define DALI_TOOLKIT_INTERNAL_ABSOLUTE_LAYOUT_H

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

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/devel-api/layouting/absolute-layout.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class AbsoluteLayout;
using AbsoluteLayoutPtr = IntrusivePtr<AbsoluteLayout>;

class AbsoluteLayout final : public LayoutGroup
{
public:
  static AbsoluteLayoutPtr New();

protected:

  /**
   * Constructor
   */
  AbsoluteLayout();

  /**
   * Destructor
   */
  virtual ~AbsoluteLayout();

  /**
   * @copydoc LayoutItem::OnMeasure
   */
  virtual void OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec ) override;

  /**
   * @copydoc LayoutItem::OnLayout
   */
  virtual void OnLayout( bool changed, LayoutLength l, LayoutLength t, LayoutLength r, LayoutLength b ) override;

private:
  AbsoluteLayout( const AbsoluteLayout& other ) = delete;
  AbsoluteLayout& operator=( const AbsoluteLayout& other ) = delete;

};

} // namespace Internal

inline Internal::AbsoluteLayout& GetImplementation( Dali::Toolkit::AbsoluteLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "AbsoluteLayout handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::AbsoluteLayout&>( object );
}

inline const Internal::AbsoluteLayout& GetImplementation( const Dali::Toolkit::AbsoluteLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "AbsoluteLayout handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::AbsoluteLayout&>( object );
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_ABSOLUTE_LAYOUT_H
