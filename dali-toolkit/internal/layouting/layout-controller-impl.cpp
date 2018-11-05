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
#include <dali-toolkit/internal/layouting/layout-item-data-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/internal/controls/control/control-data-impl.h>
#include <dali-toolkit/internal/layouting/layout-controller-debug.h>

using namespace Dali;

namespace
{

#if defined(DEBUG_ENABLED)
static Debug::Filter* gLogFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_LAYOUT" );
#endif

}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

LayoutController::LayoutController()
: mLayoutRequested( false ),
  mSlotDelegate( this )
{
}

LayoutController::~LayoutController()
{
}

void LayoutController::Initialize()
{
  mAnimation = Animation::New( 0.0f );
}

void LayoutController::RequestLayout( LayoutItem& layoutItem, int layoutTransitionType )
{
  auto actor = Actor::DownCast( layoutItem.GetOwner() );
  if ( actor )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "LayoutController::RequestLayout owner[%s] layoutItem[%p] layoutAnimationType(%d)\n", actor.GetName().c_str(), &layoutItem, layoutTransitionType );
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "LayoutController::RequestLayout layoutItem[%p] layoutAnimationType(%d)\n", &layoutItem, layoutTransitionType );
  }

  mLayoutRequested = true;
  if( layoutTransitionType != -1 )
  {
    LayoutTransition layoutTransition = LayoutTransition( layoutItem, layoutTransitionType );
    if( std::find( mLayoutTransitions.begin(), mLayoutTransitions.end(), layoutTransition ) == mLayoutTransitions.end() && layoutItem.GetTransitionData( layoutTransitionType ).Get() )
    {
      mLayoutTransitions.push_back( layoutTransition );
    }
  }

  // Go up the tree and mark all parents to relayout
  LayoutParent* layoutParent = layoutItem.GetParent();
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
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "LayoutController::Process\n" );

  if( mLayoutRequested )
  {
    // If window size has changed, expect stage to have already been updated
    Stage stage = Stage::GetCurrent();
    auto stageWidth  = stage.GetSize().width;
    auto stageHeight = stage.GetSize().height;

    MeasureSpec widthSpec( stageWidth, MeasureSpec::Mode::EXACTLY );
    MeasureSpec heightSpec( stageHeight, MeasureSpec::Mode::EXACTLY );

    // Test how to perform a measure on each control.
    MeasureHierarchy( stage.GetRootLayer(), widthSpec, heightSpec );

    LAYOUT_DEBUG_MEASURE_STATES( stage.GetRootLayer() );

    LayoutTransition layoutTransition;
    LayoutPositionDataArray layoutPositionDataArray;
    LayoutDataArray layoutDataArray;
    LayoutAnimatorArray layoutAnimatorArray;
    layoutAnimatorArray.push_back( LayoutDataAnimator() );
    PropertyAnimatorArray childrenPropertiesAnimators;

    if ( mLayoutTransitions.size() )
    {
      layoutTransition = mLayoutTransitions.front();
      mLayoutTransitions.pop_front();
      mLayoutRequested = ( mLayoutTransitions.size() != 0 );
    }
    else
    {
      mLayoutRequested = false;
    }

    LayoutData layoutData( layoutTransition, layoutPositionDataArray, layoutDataArray, layoutAnimatorArray, childrenPropertiesAnimators );
    LayoutItem::Impl::sLayoutData = &layoutData;
    PerformLayout( stage.GetRootLayer(), 0, 0, stageWidth, stageHeight );

    PerformLayoutPositioning( layoutPositionDataArray, false );

    PerformLayoutAnimation( layoutTransition, layoutPositionDataArray, layoutDataArray, layoutAnimatorArray );
    LayoutItem::Impl::sLayoutData = nullptr;

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
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutController::Measuring control:%s\n", control.GetName().c_str() );
    Internal::Control& controlImpl = GetImplementation( control );

    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    LayoutItemPtr layout = controlDataImpl.GetLayout();

    if( layout )
    {
      layout->Measure( widthSpec, heightSpec );
    }
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutController::Measuring (%u) children\n", root.GetChildCount() );
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
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutController::PerformLayout on control[%s]\n", control.GetName().c_str() );
    Internal::Control& controlImpl = GetImplementation( control );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    LayoutItemPtr layout = controlDataImpl.GetLayout();

    if( layout )
    {
      layout->Layout( left, top, right, bottom );
    }
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutController::PerformLayout (%u) children\n", root.GetChildCount() );
    // Depth first descent through actor children
    for( unsigned int i = 0, count = root.GetChildCount(); i < count; ++i )
    {
      Actor child = root.GetChildAt( i );
      PerformLayout( child, left, top, right, bottom );
    }
  }
}

void LayoutController::PerformLayoutPositioning( LayoutPositionDataArray& layoutPositionDataArray, bool all ) const
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutController::PerformLayoutPositioning %d\n", (int)all );

  for( auto layoutPositionData : layoutPositionDataArray )
  {
    Actor actor = Actor::DownCast( layoutPositionData.handle );
    if( actor && ( !layoutPositionData.animated || all ) )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutController::PerformLayoutPositioning %s\n", actor.GetName().c_str() );
      actor.SetPosition( layoutPositionData.left, layoutPositionData.top );
      actor.SetSize( layoutPositionData.right - layoutPositionData.left, layoutPositionData.bottom - layoutPositionData.top );
    }
  }
}

void LayoutController::PerformLayoutAnimation( LayoutTransition& layoutTransition, LayoutPositionDataArray& layoutPositionDataArray, LayoutDataArray& layoutDataArray, LayoutAnimatorArray& layoutAnimatorArray )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutController::PerformLayoutAnimation\n" );
  Animation animation = Animation::New( 0 );
  bool isAnimatorAdded = false;

  for( auto layoutDataElement : layoutDataArray )
  {
    if ( layoutDataElement.animatorIndex >= 0 )
    {
      Actor actor = Actor::DownCast( layoutDataElement.handle );
      if ( actor )
      {
        LayoutDataAnimator animator = layoutAnimatorArray[ layoutDataElement.animatorIndex ];
        TimePeriod timePeriod = TimePeriod( 0, animation.GetDuration() );
        if (animator.timePeriod.durationSeconds >= 0)
        {
          timePeriod = animator.timePeriod;
        }

        Property::Value value = layoutDataElement.targetValue;
        // Capture calculated position and size values after layout if target values are not set.
        // Other values are set to current actor ones.
        if( value.GetType() == Property::NONE )
        {
          if ( layoutDataElement.positionDataIndex < 0)
          {
            auto result = std::find_if( layoutPositionDataArray.begin(), layoutPositionDataArray.end(), [&actor](const LayoutPositionData& iter)
                          { return iter.handle == actor; } );
            if( result == layoutPositionDataArray.end() )
            {
              continue;
            }
            layoutDataElement.positionDataIndex = std::distance(layoutPositionDataArray.begin(), result);
          }

          LayoutPositionData& positionData = layoutPositionDataArray[ layoutDataElement.positionDataIndex ];

          switch ( layoutDataElement.propertyIndex )
          {
          case Actor::Property::POSITION:
            value = Vector3( positionData.left, positionData.top, 0.0f );
            break;
          case Actor::Property::POSITION_X:
            value = positionData.left;
            break;
          case Actor::Property::POSITION_Y:
            value = positionData.top;
            break;
          case Actor::Property::SIZE:
            value = Vector3( positionData.right - positionData.left, positionData.bottom - positionData.top, 0.0f );
            break;
          case Actor::Property::SIZE_WIDTH:
            value = positionData.right - positionData.left;
            break;
          case Actor::Property::SIZE_HEIGHT:
            value = positionData.bottom - positionData.top;
            break;
          default:
            value = actor.GetProperty( layoutDataElement.propertyIndex );
          }
        }

        // Failed to get target value, just move the next one
        if( value.GetType() == Property::NONE )
        {
          continue;
        }

        // Set initial value
        Property::Value initialValue = layoutDataElement.initialValue;
        if( initialValue.GetType() != Property::NONE )
        {
          actor.SetProperty( layoutDataElement.propertyIndex, initialValue );
        }

        // Create an animator for the property
        switch (animator.animatorType)
        {
        case LayoutDataAnimator::AnimatorType::ANIMATE_TO:
        {
          animation.AnimateTo( Property( actor, layoutDataElement.propertyIndex ), value, animator.alphaFunction, timePeriod );
          break;
        }
        case LayoutDataAnimator::AnimatorType::ANIMATE_BY:
        {
          animation.AnimateBy( Property( actor, layoutDataElement.propertyIndex ), value, animator.alphaFunction, timePeriod );
          break;
        }
        case LayoutDataAnimator::AnimatorType::ANIMATE_BETWEEN:
        {
          animation.AnimateBetween( Property( actor, layoutDataElement.propertyIndex ), animator.keyFrames, animator.alphaFunction, animator.interpolation );
          break;
        }
        case LayoutDataAnimator::AnimatorType::ANIMATE_PATH:
          animation.Animate( actor, animator.path, animator.forward, animator.alphaFunction, timePeriod );
          break;
        }
      }
      isAnimatorAdded = true;
    }
  }

  if( isAnimatorAdded )
  {
    if( mAnimation.GetState() == Animation::PLAYING )
    {
      mAnimation.Clear();
      if( mAnimationFinishedFunctors.size() != 0 )
      {
        mAnimationFinishedFunctors.front()( mAnimation );
      }
    }

    mAnimation = animation;
    mAnimationFinishedFunctors.push_back( AnimationFinishedFunctor( *this, layoutTransition, layoutPositionDataArray ) );
    mAnimation.FinishedSignal().Connect( mSlotDelegate.GetConnectionTracker(), mAnimationFinishedFunctors.back() );
    mAnimation.Play();
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
