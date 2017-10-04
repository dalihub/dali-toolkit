#ifndef DALI_TOOLKIT_LAYOUTING_LAYOUT_GROUP_H
#define DALI_TOOLKIT_LAYOUTING_LAYOUT_GROUP_H
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

#include <memory>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali-toolkit/devel-api/layouting/layout-base.h>
#include <dali-toolkit/devel-api/layouting/measure-spec.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal
{
class LayoutGroup;
}


/**
 * A layout that has children. Implements LayoutBase.
 * CONSIDER HOW TO WRITE CUSTOM LAYOUTERS.
 * (Handle and body are both public; body contains no data apart from an extensible Impl ptr)
 * Do we really need a specific handle type?
 */
class DALI_IMPORT_API LayoutGroup : public LayoutBase
{
public:
  using LayoutId = unsigned int;

  LayoutGroup();
  ~LayoutGroup()=default;
  LayoutGroup(const LayoutGroup& copy);
  LayoutGroup& operator=(const LayoutGroup& rhs);

  static LayoutGroup DownCast( BaseHandle handle );

  LayoutId Add( LayoutBase& layoutData );
  void Remove( LayoutId childId );
  LayoutBase GetChild( LayoutId childId );

  MeasureSpec GetChildMeasureSpec( MeasureSpec measureSpec, int padding, Dali::Dimension::Type dimension );

public:
  explicit DALI_INTERNAL LayoutGroup( Internal::LayoutGroup* layoutGroup );
};


}//namespace Toolkit
}//namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_LAYOUT_GROUP_H
