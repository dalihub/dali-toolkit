#ifndef DALI_TOOLKIT_LAYOUTING_HBOX_LAYOUT_H
#define DALI_TOOLKIT_LAYOUTING_HBOX_LAYOUT_H

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

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali-toolkit/devel-api/layouting/layout-group.h>
#include <dali-toolkit/devel-api/layouting/layout-size.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class HboxLayout;
}

class DALI_IMPORT_API HboxLayout : public LayoutGroup
{
public:

  enum PropertyRange
  {
    CHILD_PROPERTY_START_INDEX = LayoutGroup::CHILD_PROPERTY_START_INDEX+100,
    CHILD_PROPERTY_END_INDEX   = CHILD_PROPERTY_START_INDEX+1000
  };

  struct ChildProperty
  {
    enum
    {
      WEIGHT = CHILD_PROPERTY_START_INDEX,
    };
  };

  HboxLayout();
  static HboxLayout New();
  static HboxLayout DownCast( BaseHandle handle );
  HboxLayout( const HboxLayout& other );
  HboxLayout& operator=( const HboxLayout& other );
  ~HboxLayout()=default;

  void SetCellPadding( LayoutSize size );
  LayoutSize GetCellPadding();

public: // Not intended for application developers

  /// @cond internal
  explicit DALI_INTERNAL HboxLayout( Internal::HboxLayout* body );
  /// @endcond
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_HBOX_LAYOUT_H
