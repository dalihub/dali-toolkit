#ifndef DALI_TOOLKIT_CONTROL_HBOX_VIEW_H
#define DALI_TOOLKIT_CONTROL_HBOX_VIEW_H

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
#include <dali-toolkit/devel-api/layouting/layout-group.h>
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class HboxView;
}

class DALI_IMPORT_API HboxView : public Control
{
public:

  using CellPosition = int;

  enum Mode
  {
    FIXED,
    FLEX,
  };

  HboxView();
  static HboxView New();
  static HboxView DownCast( BaseHandle handle );
  HboxView( const HboxView& other );
  HboxView& operator=( const HboxView& other );
  ~HboxView()=default;

public:
  void AddChild( Actor child );
  Actor GetChildAt( CellPosition position );
  Actor RemoveChildAt( CellPosition position );
  void SetCellPadding( LayoutSize size );
  LayoutSize GetCellPadding();
  void SetMode( Mode mode );
  void SetCellWidth( CellPosition cellPosition, LayoutLength width );
  void SetCellHeight( CellPosition cellPosition, LayoutLength height );

public: // Not intended for application developers

  /// @cond internal
  DALI_INTERNAL HboxView(Internal::HboxView& implementation);
  explicit DALI_INTERNAL HboxView( Dali::Internal::CustomActor* body );
  /// @endcond
};

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_HBOX_VIEW_H
