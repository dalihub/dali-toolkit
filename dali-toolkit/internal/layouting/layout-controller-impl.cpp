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

#include <dali/public-api/common/stage.h>
#include <dali/public-api/actors/layer.h>
#include <dali-toolkit/internal/layouting/layout-controller-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/layouting/layout-controller-debug.h>

using namespace Dali;

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

LayoutController::LayoutController()
: mLayoutRequested(false)
{
}

LayoutController::~LayoutController()
{
}

void LayoutController::Initialize()
{
}

void LayoutController::RegisterLayout( LayoutGroup& layout )
{
  // cache the layout into a vector for faster traversal
}


void LayoutController::RequestLayout( LayoutBase& layoutBase )
{
  mLayoutRequested = true;

  LayoutParent* layoutParent = layoutBase.GetParent();
  if( layoutParent )
  {
    LayoutGroup& layoutGroup = static_cast< LayoutGroup& >( *layoutParent );
    if( ! layoutGroup.IsLayoutRequested() )
    {
      layoutGroup.RequestLayout();
    }
  }
}

void LayoutController::Process()
{
  // Perform the full process.

  if( mLayoutRequested )
  {
    mLayoutRequested = false;

    // If window size has changed, expect stage to have already been updated
    Stage stage = Stage::GetCurrent();
    uint32_t stageWidth  = stage.GetSize().width;
    uint32_t stageHeight = stage.GetSize().height;

    MeasureSpec widthSpec = MeasureSpec::MakeMeasureSpec( stageWidth, MeasureSpec::EXACTLY );
    MeasureSpec heightSpec = MeasureSpec::MakeMeasureSpec( stageHeight, MeasureSpec::EXACTLY );

    // Test how to perform a measure on each control.
    MeasureHierarchy( stage.GetRootLayer(), widthSpec, heightSpec );

    LAYOUT_DEBUG_MEASURE_STATES( stage.GetRootLayer() );
    PerformLayout( stage.GetRootLayer(), 0, 0, stageWidth, stageHeight );

    LAYOUT_DEBUG_AFTER_LAYOUT( stage.GetRootLayer() );
  }
}

void LayoutController::MeasureHierarchy( Actor root, MeasureSpec widthSpec, MeasureSpec heightSpec )
{
  // Does this actor have a layout?
  // Yes - measure the layout. It will call this method again for each of it's children.
  // No - recurse through actor children.
  //
  // If in a leaf actor with no layout, it's natural size is bubbled back up.
  //
  // What happens if nothing in the tree has a layout?

  Toolkit::Control control = Toolkit::Control::DownCast( root );
  if( control )
  {
    Internal::Control& controlImpl = GetImplementation( control );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    LayoutBasePtr layout = controlDataImpl.GetLayout();

    if( layout )
    {
      layout->Measure( widthSpec, heightSpec );
    }
  }
  else
  {
    // Depth first descent through actor children
    for( unsigned int i = 0, count = root.GetChildCount(); i < count; ++i )
    {
      Actor child = root.GetChildAt( i );
      MeasureHierarchy( child, widthSpec, heightSpec );
    }
  }
}

void LayoutController::PerformLayout( Actor root, int left, int top, int right, int bottom )
{
  Toolkit::Control control = Toolkit::Control::DownCast( root );
  if( control )
  {
    Internal::Control& controlImpl = GetImplementation( control );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    LayoutBasePtr layout = controlDataImpl.GetLayout();

    // This is the first control. Give it the stage size
    control.SetSize( Vector3(right, bottom, 0.0f) );

    if( layout )
    {
      layout->Layout( left, top, right, bottom, false ); // @todo why pass animate flag down? - should be per layout.
    }
  }
  else
  {
    // Depth first descent through actor children
    for( unsigned int i = 0, count = root.GetChildCount(); i < count; ++i )
    {
      Actor child = root.GetChildAt( i );
      PerformLayout( child, left, top, right, bottom );
    }
  }
}


} // namespace Internal
} // namespace Toolkit
} // namespace Dali
