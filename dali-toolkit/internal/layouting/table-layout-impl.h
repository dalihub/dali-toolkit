#ifndef DALI_TOOLKIT_INTERNAL_LAYOUTING_TABLE_LAYOUT_IMPL_H
#define DALI_TOOLKIT_INTERNAL_LAYOUTING_TABLE_LAYOUT_IMPL_H

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
 */

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali-toolkit/devel-api/layouting/table-layout.h>
#include <dali-toolkit/devel-api/layouting/layout-base-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>


namespace Dali
{
namespace Toolkit
{
namespace Internal
{
using TableLayoutPtr = IntrusivePtr<TableLayout>;

class TableLayout : public LayoutGroup
{
public:
  static TableLayoutPtr New( BaseHandle handle, int rows, int columns );

  TableLayout(int rows, int columns);
  ~TableLayout()=default;
  TableLayout(const TableLayout& copy)=delete;
  TableLayout& operator=(const TableLayout& rhs)=delete;

  virtual void OnMeasure( MeasureSpec widthSpec, MeasureSpec heightSpec );
  virtual void OnLayout( bool animate );
  virtual void OnSetLayoutData();

private:
};


} // namespace Internal

inline Internal::TableLayout& GetImplementation( Dali::Toolkit::TableLayout& handle )
{
  DALI_ASSERT_ALWAYS( handle && "TableLayout handle is empty" );
  BaseObject& object = handle.GetBaseObject();
  return static_cast< Internal::TableLayout& >( object );
}

inline const Internal::TableLayout& GetImplementation( const Dali::Toolkit::TableLayout handle )
{
  DALI_ASSERT_ALWAYS( handle && "TableLayout handle is empty" );
  const BaseObject& object = handle.GetBaseObject();
  return static_cast< const Internal::TableLayout& >( object );
}

} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_INTERNAL_LAYOUTING_TABLE_LAYOUT_IMPL_H
