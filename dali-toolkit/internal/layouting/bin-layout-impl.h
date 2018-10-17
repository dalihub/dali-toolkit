#ifndef DALI_TOOLKIT_INTERNAL_BIN_LAYOUT_H
#define DALI_TOOLKIT_INTERNAL_BIN_LAYOUT_H

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
#include <dali-toolkit/devel-api/layouting/bin-layout.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class BinLayout;
using BinLayoutPtr = IntrusivePtr<BinLayout>;

class BinLayout final : public LayoutGroup
{
public:
  static BinLayoutPtr New();

protected:

  /**
   * Constructor
   */
  BinLayout();

  /**
   * Destructor
   */
  virtual ~BinLayout();

  /**
   * @copydoc LayoutItem::OnMeasure
   */
  virtual void OnMeasure( MeasureSpec widthMeasureSpec, MeasureSpec heightMeasureSpec ) override;

  /**
   * @copydoc LayoutItem::OnLayout
   */
  virtual void OnLayout( bool changed, LayoutLength l, LayoutLength t, LayoutLength r, LayoutLength b ) override;

private:
  BinLayout( const BinLayout& other ) = delete;
  BinLayout& operator=( const BinLayout& other ) = delete;

};

} // namespace Internal

inline Internal::BinLayout& GetImplementation( Dali::Toolkit::BinLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "BinLayout handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::BinLayout&>( object );
}

inline const Internal::BinLayout& GetImplementation( const Dali::Toolkit::BinLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "BinLayout handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::BinLayout&>( object );
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_BIN_LAYOUT_H
