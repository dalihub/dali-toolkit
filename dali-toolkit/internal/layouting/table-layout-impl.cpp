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

#include <dali-toolkit/internal/layouting/table-layout-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

TableLayoutPtr TableLayout::New( BaseHandle handle, int rows, int columns )
{
  TableLayoutPtr tableLayout = new TableLayout( rows, columns );
  tableLayout->Initialize( handle );
  return tableLayout;
}

TableLayout::TableLayout( int rows, int columns )
{
}

void TableLayout::OnMeasure( MeasureSpec widthSpec, MeasureSpec heightSpec )
{
}

void TableLayout::OnLayout( bool animate )
{
}

void TableLayout::OnSetLayoutData()
{
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
