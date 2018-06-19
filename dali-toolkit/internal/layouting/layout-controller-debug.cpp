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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/layouting/layout-controller-debug.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

#if defined( DEBUG_ENABLED )
Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_LAYOUT_TREE" );


void GetLayoutMeasureStateString( std::ostringstream& oss, LayoutItemPtr layout )
{
  if( layout )
  {
    auto widthSizeAndState = layout->GetMeasuredWidthAndState();
    auto heightSizeAndState = layout->GetMeasuredHeightAndState();

    oss << "LayoutMeasureState:" <<
      "  w:" << widthSizeAndState.GetSize() << (widthSizeAndState.GetState()==MeasuredSize::MEASURED_SIZE_TOO_SMALL?"/TooSmall":"") <<
      "  h:" << heightSizeAndState.GetSize() << (heightSizeAndState.GetState()==MeasuredSize::MEASURED_SIZE_TOO_SMALL?"/TooSmall":"");
  }
}

void LayoutDebugMeasureStateRecurseLayout( LayoutItemPtr layout, int depth );

void LayoutDebugMeasureStateRecurseActor( Actor root, int depth )
{
  std::ostringstream oss;
  for(int i=0;i<depth;++i) { oss << "  "; };
  oss << "Actor " << root.GetId() << ":" << root.GetName() << " ";

  bool descendActor = true;
  Toolkit::Control control = Toolkit::Control::DownCast( root );
  if( control )
  {
    Internal::Control& controlImpl = GetImplementation( control );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    LayoutItemPtr layout = controlDataImpl.GetLayout();

    if( layout != nullptr )
    {
      GetLayoutMeasureStateString( oss, layout );
      oss << std::endl;
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, oss.str().c_str() );

      auto layoutGroup = LayoutGroupPtr( dynamic_cast< LayoutGroup* >( layout.Get() ) );
      if( layoutGroup )
      {
        for( unsigned int i=0; i<layoutGroup->GetChildCount(); ++i )
        {
          auto layoutChild = layoutGroup->GetChildAt( i );
          LayoutDebugMeasureStateRecurseLayout( layoutChild, depth+1 );
        }
      }
      descendActor = false;
    }
  }

  if( descendActor == true )
  {
    oss << std::endl;
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, oss.str().c_str() );

    // Depth first descent through actor children
    for( unsigned int i = 0, count = root.GetChildCount(); i < count; ++i )
    {
      Actor child = root.GetChildAt( i );
      LayoutDebugMeasureStateRecurseActor( child, depth+1 );
    }
  }
}

void LayoutDebugMeasureStateRecurseLayout( LayoutItemPtr layout, int depth )
{
  std::ostringstream oss;
  for(int i=0;i<depth;++i) { oss << "  "; }; // indent

  Actor actor = Actor::DownCast( layout->GetOwner() );
  if( actor )
  {
    oss << "Actor " << actor.GetId() << ":" << actor.GetName() << " ";
  }
  else
  {
    oss << "Owner: " << layout->GetOwner().GetObjectPtr() << " ";
  }

  GetLayoutMeasureStateString( oss, layout );
  oss << std::endl;
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, oss.str().c_str() );

  auto layoutGroup = LayoutGroupPtr( dynamic_cast< LayoutGroup* >( layout.Get() ) );
  if( layoutGroup )
  {
    for( unsigned int i=0; i<layoutGroup->GetChildCount(); ++i )
    {
      auto layoutChild = layoutGroup->GetChildAt( i );
      LayoutDebugMeasureStateRecurseLayout( layoutChild, depth+1 );
    }
  }
  else if( actor )
  {
    for( unsigned int i = 0, count = actor.GetChildCount(); i < count; ++i )
    {
      LayoutDebugMeasureStateRecurseActor( actor.GetChildAt( i ), depth + 1 );
    }
  }
}


void LayoutDebugMeasureState( Actor root )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Layout tree after measure:\n\n" );
  LayoutDebugMeasureStateRecurseActor( root, 0 );
}


void LayoutDebugAfterLayoutRecurse( Actor root, int depth )
{
  std::ostringstream oss;
  for(int i=0;i<depth;++i) { oss << "  "; };
  oss << "Actor " << root.GetId() << ":" << root.GetName() << " ";
  Toolkit::Control control = Toolkit::Control::DownCast( root );
  if( control )
  {
    Internal::Control& controlImpl = GetImplementation( control );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    LayoutItemPtr layout = controlDataImpl.GetLayout();

    if( layout )
    {
      auto childOwner = layout->GetOwner();
      auto widthMeasureSpec = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
      auto heightMeasureSpec = childOwner.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      oss << "LayoutData:" << "( " << widthMeasureSpec << ", " << heightMeasureSpec << ") ";

      auto actorPos  = root.GetProperty<Vector3>( Actor::Property::POSITION );
      auto actorSize = root.GetProperty<Vector3>( Actor::Property::SIZE );
      oss << "  ActorPos: (" << actorPos.x << ", " << actorPos.y << ")";
      oss << "  ActorSize: (" << actorSize.width << ", " << actorSize.height << ")";
    }
    else
    {
      // Try getting layout data from parent control
    }
  }
  oss << std::endl;

  DALI_LOG_INFO( gLogFilter, Debug::Verbose, oss.str().c_str() );

  // Depth first descent through actor children
  for( unsigned int i = 0, count = root.GetChildCount(); i < count; ++i )
  {
    Actor child = root.GetChildAt( i );
    LayoutDebugAfterLayoutRecurse( child, depth+1 );
  }
}

void LayoutDebugAfterLayout( Actor root )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Layout tree after layout:\n\n" );
  LayoutDebugAfterLayoutRecurse( root, 0 );
}


#endif

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
