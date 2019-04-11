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
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>

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
  mFocusChangedFunctor( *this ),
  mSlotDelegate( this )
{
}

LayoutController::~LayoutController()
{
}

void LayoutController::Initialize()
{
  mAnimation = Animation::New( 0.0f );

  Dali::Toolkit::KeyInputFocusManager manager = Dali::Toolkit::KeyInputFocusManager::Get();
  manager.KeyInputFocusChangedSignal().Connect( mSlotDelegate.GetConnectionTracker(), mFocusChangedFunctor );
}

void LayoutController::FocusChangedFunctor::operator() ( Dali::Toolkit::Control gainingControl, Dali::Toolkit::Control lostControl )
{
  Toolkit::LayoutItem layoutItem = Toolkit::DevelControl::GetLayout( gainingControl );
  if( layoutItem )
  {
    Toolkit::Internal::LayoutItem& layoutItemImpl = GetImplementation( layoutItem );
    LayoutParent* layoutParent = layoutItemImpl.GetParent();
    if( layoutParent )
    {
      LayoutGroup* layoutGroup = static_cast< LayoutGroup* >( layoutParent );
      layoutController.RequestLayout( dynamic_cast< Toolkit::Internal::LayoutItem& >( *layoutGroup ), Dali::Toolkit::LayoutTransitionData::ON_CHILD_FOCUS, gainingControl, lostControl );
    }
  }
}

void LayoutController::RequestLayout( LayoutItem& layoutItem, int layoutTransitionType, Actor gainedChild, Actor lostChild )
{
  auto actor = Actor::DownCast( layoutItem.GetOwner() );
  if ( actor )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "LayoutController::RequestLayout owner[%s] layoutItem[%p] layoutTransitionType(%d) gainedChild[%s] lostChild[%s]\n",
      actor.GetName().c_str(), &layoutItem, layoutTransitionType,
      gainedChild ? gainedChild.GetName().c_str() : "",
      lostChild ? lostChild.GetName().c_str() : "");
  }
  else
  {
    DALI_LOG_INFO( gLogFilter, Debug::Concise, "LayoutController::RequestLayout layoutItem[%p] layoutAnimationType(%d)\n", &layoutItem, layoutTransitionType );
  }

  mLayoutRequested = true;
  if( layoutTransitionType != -1 )
  {
    LayoutTransition layoutTransition = LayoutTransition( layoutItem, layoutTransitionType, gainedChild, lostChild );
    if( std::find( mLayoutTransitions.begin(), mLayoutTransitions.end(), layoutTransition ) == mLayoutTransitions.end() && layoutItem.GetTransitionData( layoutTransitionType ).Get() )
    {
      DALI_LOG_INFO( gLogFilter, Debug::Concise, "LayoutController::RequestLayout Add transition layoutTransitionType(%d)\n", layoutTransitionType );
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

    LayoutTransition layoutTransition;
    LayoutPositionDataArray layoutPositionDataArray;
    LayoutAnimatorArray layoutAnimatorArray;
    LayoutDataArray layoutDataArray;
    LayoutDataArray childrenLayoutDataArray;

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

    LayoutData layoutData( layoutTransition, layoutPositionDataArray, layoutAnimatorArray, layoutDataArray, childrenLayoutDataArray );
    LayoutItem::Impl::sLayoutData = &layoutData;

    if( layoutTransition.layoutTransitionType != -1 )
    {
      UpdateMeasureHierarchyForAnimation( layoutData );
    }

    // Test how to perform a measure on each control.
    MeasureHierarchy( stage.GetRootLayer(), widthSpec, heightSpec );

    LAYOUT_DEBUG_MEASURE_STATES( stage.GetRootLayer() );

    if( layoutTransition.layoutTransitionType != -1 )
    {
      RestoreActorsSpecs();
    }

    layoutAnimatorArray.clear();
    layoutDataArray.clear();
    childrenLayoutDataArray.clear();

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

void LayoutController::UpdateMeasureHierarchyForAnimation( LayoutData& layoutData )
{
  LayoutTransition& layoutTransition = layoutData.layoutTransition;
  Actor transitionOwner = Actor::DownCast( layoutTransition.layoutItem.Get()->GetOwner() );
  LayoutTransitionDataPtr layoutTransitionDataPtr = layoutTransition.layoutItem->GetTransitionData( layoutTransition.layoutTransitionType );

  if( !layoutTransitionDataPtr->HasUpdateMeasuredSize() )
  {
    return;
  }

  layoutData.updateMeasuredSize = true;
  layoutTransitionDataPtr->CollectLayoutDataElements( transitionOwner, layoutData );

  UpdateMeasureHierarchyForAnimation( transitionOwner, layoutData );

  for( auto layoutDataElement : layoutData.layoutDataArray )
  {
    if( !layoutDataElement.updateMeasuredSize )
    {
      continue;
    }

    Actor actor = layoutDataElement.handle.GetHandle();
    if( actor )
    {
      LayoutDataAnimator animator = layoutData.layoutAnimatorArray[ layoutDataElement.animatorIndex ];
      float width = actor.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION );
      float height = actor.GetProperty<int>( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION );

      if( layoutDataElement.AdjustMeasuredSize( width, height, animator.animatorType ) )
      {
        mActorSizeSpecs.push_back( ActorSizeSpec( actor ) );
        actor.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, static_cast<int>( width ) );
        actor.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, static_cast<int>( height ) );
      }
    }
  }

  layoutData.updateMeasuredSize = false;
}

void LayoutController::UpdateMeasureHierarchyForAnimation( Actor root, LayoutData& layoutData )
{
  Toolkit::Control control = Toolkit::Control::DownCast( root );
  if( control )
  {
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutController::UpdateMeasureHierarchyForAnimation control:%s\n", control.GetName().c_str() );
    Internal::Control& controlImpl = GetImplementation( control );
    Internal::Control::Impl& controlDataImpl = Internal::Control::Impl::Get( controlImpl );
    LayoutItemPtr layout = controlDataImpl.GetLayout();

    if( layout )
    {
      auto layoutGroup = Toolkit::LayoutGroup::DownCast( layout.Get() );
      if( layoutGroup )
      {
        unsigned int childCount = layoutGroup.GetChildCount();
        for( unsigned int i=0; i<childCount; ++i )
        {
          auto childLayout = layoutGroup.GetChildAt( i );
          if( childLayout )
          {
            auto childControl = Toolkit::Control::DownCast( childLayout.GetOwner() );
            LayoutTransitionData::CollectChildrenLayoutDataElements( childControl, layoutData );
          }
        }
      }
    }
  }
  else
  {
    // Depth first descent through actor children
    for( unsigned int i = 0, count = root.GetChildCount(); i < count; ++i )
    {
      UpdateMeasureHierarchyForAnimation( root.GetChildAt( i ), layoutData );
    }
  }
}

void LayoutController::RestoreActorsSpecs()
{
  for( auto& actorSizeSpec : mActorSizeSpecs )
  {
    Actor actor = actorSizeSpec.actor.GetHandle();
    if( actor )
    {
      actor.SetProperty( Toolkit::LayoutItem::ChildProperty::WIDTH_SPECIFICATION, actorSizeSpec.widthSpec );
      actor.SetProperty( Toolkit::LayoutItem::ChildProperty::HEIGHT_SPECIFICATION, actorSizeSpec.heightSpec );
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
    Actor actor = layoutPositionData.handle.GetHandle();
    if( actor && ( !layoutPositionData.animated || all ) )
    {
      if( !layoutPositionData.animated )
      {
        actor.SetX( layoutPositionData.left );
        actor.SetY( layoutPositionData.top );

        actor.SetProperty( Actor::Property::SIZE_WIDTH, layoutPositionData.right - layoutPositionData.left );
        actor.SetProperty( Actor::Property::SIZE_HEIGHT, layoutPositionData.bottom - layoutPositionData.top );
      }
      else
      {
        actor.SetX( actor.GetCurrentPosition().x );
        actor.SetY( actor.GetCurrentPosition().y );

        actor.SetProperty( Actor::Property::SIZE_WIDTH, actor.GetCurrentSize().x );
        actor.SetProperty( Actor::Property::SIZE_HEIGHT, actor.GetCurrentSize().y );
      }
    }
  }
}

void LayoutController::PerformLayoutAnimation( LayoutTransition& layoutTransition, LayoutPositionDataArray& layoutPositionDataArray, LayoutDataArray& layoutDataArray, LayoutAnimatorArray& layoutAnimatorArray )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "LayoutController::PerformLayoutAnimation\n" );
  Animation animation = Animation::New( 0 );
  bool isAnimatorAdded = false;

  if( layoutAnimatorArray.size() == 0 )
  {
    layoutAnimatorArray.push_back( LayoutDataAnimator() );
  }

  for( auto layoutDataElement : layoutDataArray )
  {
    if( layoutDataElement.animatorIndex >= 0 )
    {
      Actor actor = layoutDataElement.handle.GetHandle();
      if ( actor )
      {
        LayoutDataAnimator animator = layoutAnimatorArray[ layoutDataElement.animatorIndex ];
        TimePeriod timePeriod = TimePeriod( 0, animation.GetDuration() );
        if( animator.timePeriod.durationSeconds >= 0 )
        {
          timePeriod = animator.timePeriod;
        }

        Property::Value value = layoutDataElement.targetValue;
        // Capture calculated position and size values after layout if target values are not set.
        // Other values are set to current actor ones.
        if( value.GetType() == Property::NONE )
        {
          if( layoutDataElement.positionDataIndex < 0)
          {
            auto result = std::find_if( layoutPositionDataArray.begin(), layoutPositionDataArray.end(), [&actor](const LayoutPositionData& iter)
                          { return iter.handle == actor; } );
            if( result == layoutPositionDataArray.end() )
            {
              continue;
            }
            layoutDataElement.positionDataIndex = std::distance( layoutPositionDataArray.begin(), result );
          }

          LayoutPositionData& positionData = layoutPositionDataArray[ layoutDataElement.positionDataIndex ];
          // with updated measured size scale animation the measured size includes scale, so we need to fit in the centre of the measured rectangle
          // the real size child so that the all scale related animations placed correctly
          if( positionData.updateWithCurrentSize )
          {
            Vector3 size = actor.GetCurrentSize();
            float dX = ( ( positionData.right - positionData.left ) - size.width ) / 2;
            float dY = ( ( positionData.bottom - positionData.top ) - size.height ) / 2;
            positionData.left += dX;
            positionData.top += dY;
            positionData.right -= dX;
            positionData.bottom -= dY;
            positionData.updateWithCurrentSize = false;
          }

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

        // Failed to get target value, just move to the next one
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
        switch( animator.animatorType )
        {
          case Toolkit::LayoutTransitionData::Animator::ANIMATE_TO:
          {
            animation.AnimateTo( Property( actor, layoutDataElement.propertyIndex ), value, animator.alphaFunction, timePeriod );
            break;
          }
          case Toolkit::LayoutTransitionData::Animator::ANIMATE_BY:
          {
            animation.AnimateBy( Property( actor, layoutDataElement.propertyIndex ), value, animator.alphaFunction, timePeriod );
            break;
          }
          case Toolkit::LayoutTransitionData::Animator::ANIMATE_BETWEEN:
          {
            animation.AnimateBetween( Property( actor, layoutDataElement.propertyIndex ), animator.keyFrames, animator.alphaFunction, animator.interpolation );
            break;
          }
          case Toolkit::LayoutTransitionData::Animator::ANIMATE_PATH:
          {
            animation.Animate( actor, animator.path, animator.forward, animator.alphaFunction, timePeriod );
            break;
          }
        }
        isAnimatorAdded = true;
      }
    }
  }

  if( isAnimatorAdded )
  {
    if( mAnimation.GetState() == Animation::PLAYING )
    {
      mAnimation.SetCurrentProgress( 1.0f );
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
