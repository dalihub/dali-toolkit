#ifndef DALI_TOOLKIT_LAYOUTING_LAYOUT_GROUP_H
#define DALI_TOOLKIT_LAYOUTING_LAYOUT_GROUP_H
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
 * A layout that has layout children. Implements LayoutBase.
 * It can both layout it's children, and be laid out by a parent container.
 *
 * A layout group automatically handles adding a Control container's children to itself,
 * both on startup and on child add/remove. If this functionality is not desired, this needs
 * splitting into two classes; one that manages just the layout part, and the other which
 * handles Actor hierarchy. (@todo Consider doing this anyway...)
 *
 * This handle class allows the application to set up layout properties for the layout group;
 * it doesn't access measure/layout directly.
 *
 * To write a new layout, inherit from both LayoutGroup handle and Internal::LayoutGroup body.
 *
 */
class DALI_IMPORT_API LayoutGroup : public LayoutBase
{
public:
  using LayoutId = unsigned int;
  static const unsigned int UNKNOWN_ID = 0;

  enum PropertyRange
  {
    CHILD_PROPERTY_START_INDEX = CHILD_PROPERTY_REGISTRATION_START_INDEX+100,
    CHILD_PROPERTY_END_INDEX   = CHILD_PROPERTY_START_INDEX+1000
  };

  struct ChildProperty
  {
    enum
    {
      MARGIN_SPECIFICATION = CHILD_PROPERTY_START_INDEX,
    };
  };

  LayoutGroup();
  ~LayoutGroup()=default;
  LayoutGroup(const LayoutGroup& copy);
  LayoutGroup& operator=(const LayoutGroup& rhs);

  static LayoutGroup DownCast( BaseHandle handle );

  LayoutId Add( LayoutBase& layoutData );
  void Remove( LayoutId childId );
  void Remove( LayoutBase& layoutData );
  LayoutBase GetChildAt( unsigned int index ) const;
  unsigned int GetChildCount() const ;

  LayoutBase GetChild( LayoutId childId ) const ;
  template <typename T>
    LayoutBase GetChild( T childId ) = delete; // Prevent implicit casting

  MeasureSpec GetChildMeasureSpec( MeasureSpec measureSpec, int padding, Dali::Dimension::Type dimension ) const;

public:
  explicit LayoutGroup( Internal::LayoutGroup* layoutGroup );
};


} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_LAYOUTING_LAYOUT_GROUP_H
