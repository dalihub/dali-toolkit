#ifndef DALI_TOOLKIT_LAYOUTING_TABLE_LAYOUT_H
#define DALI_TOOLKIT_LAYOUTING_TABLE_LAYOUT_H

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

#include <dali/public-api/common/dali-common.h>
#include <dali-toolkit/devel-api/layouting/layout-group.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal
{
class TableLayout;
}

class DALI_IMPORT_API TableLayout : public LayoutGroup
{
public:
  TableLayout();
  ~TableLayout()=default;
  TableLayout( const TableLayout& copy );
  TableLayout& operator=( const TableLayout& rhs );

  /**
   * Create a new layouter
   * @param[in] handle Handle to the owning control
   * @param[in] rows The number of rows
   * @param[in] columns The number of columns
   */
  static TableLayout New( BaseHandle handle, int rows, int columns );

  static TableLayout DownCast( BaseHandle handle );

public:
  explicit DALI_INTERNAL TableLayout( Internal::TableLayout* tableLayout );
};

} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_LAYOUTING_TABLE_LAYOUT_H
