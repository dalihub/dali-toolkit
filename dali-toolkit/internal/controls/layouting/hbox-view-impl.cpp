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

//CLASS HEADER
#include <dali-toolkit/internal/controls/layouting/hbox-view-impl.h>

//EXTERNAL HEADERS
//INTERNAL HEADERS

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

Dali::Toolkit::HboxView HboxView::New()
{
  HboxViewPtr object( new HboxView() );
  Dali::Toolkit::HboxView handle( *object );
  object->Initialize();
  return handle;
}

void HboxView::AddChild( Actor child )
{
}

Actor HboxView::GetChildAt( Dali::Toolkit::HboxView::CellPosition position )
{
  return Actor(0);
}

Actor HboxView::RemoveChildAt( Dali::Toolkit::HboxView::CellPosition position )
{
  return Actor(0);
}

void HboxView::SetCellPadding( LayoutSize size )
{
}

LayoutSize HboxView::GetCellPadding()
{
  return LayoutSize(0, 0);
}

void HboxView::SetMode( Dali::Toolkit::HboxView::Mode mode )
{
}

void HboxView::SetCellWidth( Dali::Toolkit::HboxView::CellPosition cellPosition, LayoutLength width )
{
}

void HboxView::SetCellHeight( Dali::Toolkit::HboxView::CellPosition cellPosition, LayoutLength height )
{
}

HboxView::HboxView()
: Control(ControlBehaviour( CONTROL_BEHAVIOUR_DEFAULT )),
  mSize(0, 0)
{
}

HboxView::~HboxView()
{
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
