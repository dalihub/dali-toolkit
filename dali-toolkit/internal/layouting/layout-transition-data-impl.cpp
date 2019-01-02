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

#include <dali/integration-api/debug.h>

#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>
#include <dali-toolkit/internal/layouting/layout-transition-data-impl.h>
#include <dali-toolkit/internal/layouting/layout-item-data-impl.h>

#include <dali/devel-api/scripting/enum-helper.h>

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLayoutFilter = Debug::Filter::New( Debug::NoLogging, false, "LOG_LAYOUT" );
#endif

// Key tokens
const char* TOKEN_CONDITION("condition");
const char* TOKEN_AFFECTS_SIBLINGS("affectsSiblings");
const char* TOKEN_PROPERTY("property");
const char* TOKEN_INITIAL_VALUE("initialValue");
const char* TOKEN_TARGET_VALUE("targetValue");
const char* TOKEN_ANIMATOR("animator");
const char* TOKEN_TYPE("type");
const char* TOKEN_NAME("name");
const char* TOKEN_TIME_PERIOD("timePeriod");
const char* TOKEN_DURATION("duration");
const char* TOKEN_DELAY("delay");
const char* TOKEN_ALPHA_FUNCTION("alphaFunction");

DALI_ENUM_TO_STRING_TABLE_BEGIN( ANIMATOR_TYPE )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::Toolkit::LayoutTransitionData::Animator::Type, ANIMATE_TO )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::Toolkit::LayoutTransitionData::Animator::Type, ANIMATE_BY )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::Toolkit::LayoutTransitionData::Animator::Type, ANIMATE_BETWEEN )
DALI_ENUM_TO_STRING_WITH_SCOPE( Dali::Toolkit::LayoutTransitionData::Animator::Type, ANIMATE_PATH )
DALI_ENUM_TO_STRING_TABLE_END( ANIMATOR_TYPE )

DALI_ENUM_TO_STRING_TABLE_BEGIN( ALPHA_FUNCTION_BUILTIN )
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, LINEAR)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, REVERSE)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, EASE_IN)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, EASE_OUT)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, EASE_IN_OUT)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, EASE_IN_SQUARE)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, EASE_OUT_SQUARE)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, EASE_IN_SINE)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, EASE_OUT_SINE)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, EASE_IN_OUT_SINE)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, EASE_OUT_BACK)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, BOUNCE)
DALI_ENUM_TO_STRING_WITH_SCOPE(Dali::AlphaFunction, SIN)
DALI_ENUM_TO_STRING_TABLE_END( ALPHA_FUNCTION_BUILTIN )

}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

bool LayoutDataElement::AdjustMeasuredSize( float& width, float& height, Toolkit::LayoutTransitionData::Animator::Type animatorType )
{
  bool adjusted = true;
  if( targetValue.GetType() == Property::NONE )
  {
    return false;
  }

  Actor actor = handle.GetHandle();
  float animateByMultiplier = ( animatorType == Toolkit::LayoutTransitionData::Animator::Type::ANIMATE_BY ) ? 1.0f : 0.0f;
  Vector3 size = actor.GetCurrentSize();

  switch ( targetValue.GetType() )
  {
    case Property::Type::VECTOR3:
    {
      Vector3 value = targetValue.Get<Vector3>();
      switch( propertyIndex )
      {
        case Actor::Property::SCALE:
        {
          width = size.x * ( animateByMultiplier + value.x );
          height = size.y * ( animateByMultiplier + value.y );
          break;
        }
        case Actor::Property::SIZE:
        {
          width = value.x + ( animateByMultiplier * size.x );
          height = value.y + ( animateByMultiplier * size.y );
          break;
        }
        default:
        {
          adjusted = false;
          break;
        }
      }
      break;
    }
    case Property::Type::FLOAT:
    {
      float value = targetValue.Get<float>();
      switch( propertyIndex )
      {
        case Actor::Property::SCALE_X:
        {
          width = size.x * ( animateByMultiplier + value );
          break;
        }
        case Actor::Property::SCALE_Y:
        {
          height = size.y * ( animateByMultiplier + value );
          break;
        }
        case Actor::Property::SIZE_WIDTH:
        {
          width = value + ( animateByMultiplier * size.x );
          break;
        }
        case Actor::Property::SIZE_HEIGHT:
        {
          height = value + ( animateByMultiplier * size.y );
          break;
        }
        default:
        {
          adjusted = true;
          break;
        }
      }
      break;
    }
    default:
    {
      adjusted = false;
      break;
    }
  }

  return adjusted;
}

void LayoutDataElement::UpdatePropertyIndex()
{
  if( propertyIndex == -1 && !propertyName.empty() )
  {
    Actor actor = handle.GetHandle();
    if( actor )
    {
      propertyIndex = DevelHandle::GetPropertyIndex( actor, Property::Key( propertyName ) );
    }
  }
}

void LayoutDataElement::UpdateAnimatorIndex( const LayoutAnimatorArray& animators )
{
  if( animatorIndex == -1 )
  {
    if( animatorName.empty() )
    {
      animatorIndex = 0;
      return;
    }

    std::string animatorName = this->animatorName;
    auto animator = std::find_if( animators.begin(), animators.end(), [ &animatorName ](const LayoutDataAnimator& iter) {
      return ( iter.name == animatorName ); } );
    if( animator != animators.end() )
    {
      animatorIndex = std::distance( animators.begin(), animator );
    }
  }
}

void LayoutDataElement::UpdatePositionDataIndex( LayoutData& layoutData )
{
    positionDataIndex = layoutData.layoutPositionDataArray.size() - 1;
    switch( propertyIndex )
    {
      case Actor::Property::SCALE:
      case Actor::Property::SCALE_X:
      case Actor::Property::SCALE_Y:
        if( positionDataIndex != -1 && updateMeasuredSize )
        {
          layoutData.layoutPositionDataArray[ positionDataIndex ].updateWithCurrentSize = true;
        }
        break;
    }
}

LayoutTransitionData::LayoutTransitionData() :
  mUpdateMeasuredSize( false )
{
}

LayoutTransitionData::~LayoutTransitionData()
{
}

LayoutTransitionDataPtr LayoutTransitionData::New()
{
  LayoutTransitionDataPtr layoutAnimationData( new LayoutTransitionData() );
  return layoutAnimationData;
}

LayoutTransitionData::PropertyAnimator::PropertyAnimator( )
  : handle()
  , map()
  , interpolation( Animation::Linear )
{
}

LayoutTransitionData::PropertyAnimator::PropertyAnimator( Actor actor, Property::Map map )
  : handle( actor )
  , map( map )
  , interpolation( Animation::Linear )
{
}

LayoutTransitionData::PropertyAnimator::PropertyAnimator( Actor actor,  Property::Map map, Path path, Vector3 forward )
  : handle( actor )
  , map( map )
  , interpolation( Animation::Linear )
  , path( path )
  , forward( forward )
{
}

LayoutTransitionData::PropertyAnimator::PropertyAnimator( Actor actor, Property::Map map, KeyFrames keyFrames, Animation::Interpolation interpolation  )
  : handle( actor )
  , map( map )
  , keyFrames( keyFrames )
  , interpolation( interpolation )
{
}

void LayoutTransitionData::AddPropertyAnimator( Actor actor, Property::Map map )
{
  LayoutDataElement layoutDataElement;
  if( ConvertToLayoutDataElement( PropertyAnimator( actor, map ), layoutDataElement ) )
  {
    mLayoutDataElements.push_back( layoutDataElement );
  }

  UpdateAnimatorsIndices();
}

void LayoutTransitionData::AddPropertyAnimator( Actor actor, Property::Map map, KeyFrames keyFrames, Animation::Interpolation interpolation )
{
  LayoutDataElement layoutDataElement;
  if( ConvertToLayoutDataElement( PropertyAnimator( actor, map, keyFrames, interpolation ), layoutDataElement ) )
  {
    mLayoutDataElements.push_back( layoutDataElement );
  }

  UpdateAnimatorsIndices();
}

void LayoutTransitionData::AddPropertyAnimator( Actor actor, Property::Map map, Path path, Vector3 forward )
{
  LayoutDataElement layoutDataElement;
  if( ConvertToLayoutDataElement( PropertyAnimator( actor, map, path, forward ), layoutDataElement ) )
  {
    mLayoutDataElements.push_back( layoutDataElement );
  }

  UpdateAnimatorsIndices();
}

bool LayoutTransitionData::ConvertToLayoutAnimator( const Property::Map& animatorMap, const PropertyAnimator& propertyAnimator, LayoutDataAnimator& layoutDataAnimator )
{
  bool valid = true;

  for ( size_t animatorMapIdx = 0; animatorMapIdx < animatorMap.Count(); ++animatorMapIdx )
  {
    const KeyValuePair pair( animatorMap.GetKeyValue( animatorMapIdx ) );

    Property::Index indexKey = Property::INVALID_INDEX;
    if ( pair.first.type == Property::Key::STRING )
    {
      const std::string& key(pair.first.stringKey);
      if( key == TOKEN_TYPE )
      {
        indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::TYPE;
      }
      else if( key == TOKEN_NAME )
      {
        indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::NAME;
      }
      else if( key == TOKEN_TIME_PERIOD )
      {
        indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::TIME_PERIOD;
      }
      else if( key == TOKEN_ALPHA_FUNCTION )
      {
        indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION;
      }
    }
    else
    {
      indexKey = pair.first.indexKey;
    }

    const Property::Value& value( pair.second );

    if ( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::ALPHA_FUNCTION )
    {
      if ( value.GetType() == Property::ARRAY )
      {
        valid = true;
        Vector4 controlPoints;
        Property::Array *array = value.GetArray();
        if ( array && array->Count() >= 4 )
        {
          for ( size_t vecIdx = 0; vecIdx < 4; ++vecIdx )
          {
            Property::Value& v = array->GetElementAt( vecIdx );
            if ( v.GetType() == Property::FLOAT )
            {
              controlPoints[vecIdx] = v.Get<float>();
            }
            else
            {
              valid = false;
              break;
            }
          }
        }
        else
        {
          valid = false;
        }

        if ( valid )
        {
          Vector2 controlPoint1( controlPoints.x, controlPoints.y );
          Vector2 controlPoint2( controlPoints.z, controlPoints.w );
          layoutDataAnimator.alphaFunction = AlphaFunction( controlPoint1, controlPoint2 );
        }
        else
        {
          valid = false;
        }
      }
      else if ( value.GetType() == Property::VECTOR4 )
      {
        Vector4 controlPoints = value.Get<Vector4>();
        Vector2 controlPoint1( controlPoints.x, controlPoints.y );
        Vector2 controlPoint2( controlPoints.z, controlPoints.w );
        layoutDataAnimator.alphaFunction = AlphaFunction( controlPoint1, controlPoint2 );
      }
      else if ( value.GetType() == Property::INTEGER )
      {
        layoutDataAnimator.alphaFunction = AlphaFunction( static_cast<AlphaFunction::BuiltinFunction>( value.Get<int>() ) );
      }
      else if ( value.GetType() == Property::STRING )
      {
        std::string alphaFunctionValue = value.Get<std::string>();

        if ( alphaFunctionValue == "LINEAR" )
        {
          layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::LINEAR );
        }
        else if ( !alphaFunctionValue.compare( 0, 5, "EASE_" ) )
        {
          if ( alphaFunctionValue == "EASE_IN" )
          {
            layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::EASE_IN );
          }
          else if ( alphaFunctionValue == "EASE_OUT" )
          {
            layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::EASE_OUT );
          }
          else if ( !alphaFunctionValue.compare( 5, 3, "IN_" ) )
          {
            if ( !alphaFunctionValue.compare( 8, -1, "SQUARE" ) )
            {
              layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::EASE_IN_SQUARE );
            }
            else if ( !alphaFunctionValue.compare( 8, -1, "OUT" ) )
            {
              layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::EASE_IN_OUT );
            }
            else if ( !alphaFunctionValue.compare( 8, -1, "OUT_SINE" ) )
            {
              layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::EASE_IN_OUT_SINE );
            }
            else if ( !alphaFunctionValue.compare( 8, -1, "SINE" ) )
            {
              layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::EASE_IN_SINE );
            }
          }
          else if ( !alphaFunctionValue.compare( 5, 4, "OUT_" ) )
          {
            if ( !alphaFunctionValue.compare( 9, -1, "SQUARE" ) )
            {
              layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::EASE_OUT_SQUARE );
            }
            else if ( !alphaFunctionValue.compare( 9, -1, "SINE" ) )
            {
              layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::EASE_OUT_SINE );
            }
            else if ( !alphaFunctionValue.compare( 9, -1, "BACK" ) )
            {
              layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::EASE_OUT_BACK );
            }
          }
        }
        else if ( alphaFunctionValue == "REVERSE" )
        {
          layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::REVERSE );
        }
        else if ( alphaFunctionValue == "BOUNCE" )
        {
          layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::BOUNCE );
        }
        else if ( alphaFunctionValue == "SIN" )
        {
          layoutDataAnimator.alphaFunction = AlphaFunction( AlphaFunction::SIN );
        }
        else
        {
          valid = false;
        }
      }
      else
      {
        valid = false;
      }
    }
    else if ( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::NAME )
    {
      if( value.GetType() == Property::STRING )
      {
        layoutDataAnimator.name = value.Get<std::string>();
      }
    }
    else if ( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::TYPE )
    {
      if( value.GetType() == Property::STRING )
      {
        std::string animatorType = value.Get<std::string>();
        if( animatorType == ANIMATOR_TYPE_TABLE[ Toolkit::LayoutTransitionData::Animator::Type::ANIMATE_TO ].string )
        {
          layoutDataAnimator.animatorType = Toolkit::LayoutTransitionData::Animator::Type::ANIMATE_TO;
        }
        else if( animatorType == ANIMATOR_TYPE_TABLE[ Toolkit::LayoutTransitionData::Animator::Type::ANIMATE_BY ].string )
        {
          layoutDataAnimator.animatorType = Toolkit::LayoutTransitionData::Animator::Type::ANIMATE_BY;
        }
        else if( animatorType == ANIMATOR_TYPE_TABLE[ Toolkit::LayoutTransitionData::Animator::Type::ANIMATE_BETWEEN ].string )
        {
          layoutDataAnimator.animatorType = Toolkit::LayoutTransitionData::Animator::Type::ANIMATE_BETWEEN;
          layoutDataAnimator.keyFrames = propertyAnimator.keyFrames;
        }
        else if( animatorType == ANIMATOR_TYPE_TABLE[ Toolkit::LayoutTransitionData::Animator::Type::ANIMATE_PATH ].string )
        {
          layoutDataAnimator.animatorType = Toolkit::LayoutTransitionData::Animator::Type::ANIMATE_PATH;
          layoutDataAnimator.path = propertyAnimator.path;
          layoutDataAnimator.forward = propertyAnimator.forward;
        }
      }
      else if ( value.GetType() == Property::INTEGER )
      {
        layoutDataAnimator.animatorType = static_cast<Toolkit::LayoutTransitionData::Animator::Type>( pair.second.Get<int>() );
      }
    }
    else if ( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::TIME_PERIOD )
    {
      Property::Map timeMap = value.Get<Property::Map>();
      for ( size_t timeMapIdx = 0; timeMapIdx < timeMap.Count(); ++timeMapIdx )
      {
        const KeyValuePair pair( timeMap.GetKeyValue( timeMapIdx ) );
        indexKey = Property::INVALID_INDEX;

        if ( pair.first.type == Property::Key::STRING)
        {
          const std::string& key( pair.first.stringKey );
          if( key == TOKEN_DURATION )
          {
            indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::DURATION;
          }
          else if( key == TOKEN_DELAY )
          {
            indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::DELAY;
          }
        }
        else
        {
          indexKey = pair.first.indexKey;
        }

        if ( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::DELAY )
        {
          layoutDataAnimator.timePeriod.delaySeconds = pair.second.Get<float>();
        }
        else if ( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::DURATION )
        {
          layoutDataAnimator.timePeriod.durationSeconds = pair.second.Get<float>();
        }
      }
    }
  }

  return valid;
}

bool LayoutTransitionData::ConvertToLayoutDataElement(
    const PropertyAnimator& propertyAnimator, LayoutDataElement& layoutDataElement )
{
  const Property::Map& map = propertyAnimator.map;
  bool propertyFound = false;

  if( mLayoutAnimators.size() == 0 )
  {
    mLayoutAnimators.push_back( LayoutDataAnimator() );
  }

  layoutDataElement.handle = propertyAnimator.handle;

  for( unsigned int mapIdx = 0; mapIdx < map.Count(); ++mapIdx )
  {
    const KeyValuePair pair( map.GetKeyValue( mapIdx ) );
    const Property::Value& value( pair.second );
    Property::Index indexKey = Property::INVALID_INDEX;

    if ( pair.first.type == Property::Key::STRING )
    {
      const std::string& key( pair.first.stringKey );
      if ( key == TOKEN_CONDITION )
      {
        indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::CONDITION;
      }
      if ( key == TOKEN_PROPERTY )
      {
        indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::PROPERTY;
      }
      else if( key == TOKEN_INITIAL_VALUE )
      {
        indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::INITIAL_VALUE;
      }
      else if( key == TOKEN_TARGET_VALUE )
      {
        indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::TARGET_VALUE;
      }
      else if( key == TOKEN_ANIMATOR )
      {
        indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::ANIMATOR;
      }
      else if( key == TOKEN_AFFECTS_SIBLINGS )
      {
        indexKey = Dali::Toolkit::LayoutTransitionData::AnimatorKey::AFFECTS_SIBLINGS;
      }
    }
    else
    {
      indexKey = pair.first.indexKey;
    }

    if( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::CONDITION )
    {
      layoutDataElement.condition = value.Get<int>();
    }
    else if ( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::AFFECTS_SIBLINGS )
    {
      layoutDataElement.updateMeasuredSize = value.Get<bool>();
      if( layoutDataElement.updateMeasuredSize )
      {
        mUpdateMeasuredSize = true;
      }
    }
    else if( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::PROPERTY )
    {
      if( value.GetType() == Property::STRING )
      {
        layoutDataElement.propertyName = value.Get<std::string>();
        layoutDataElement.UpdatePropertyIndex();
      }
      else
      {
        layoutDataElement.propertyIndex = value.Get<int>();
      }
      propertyFound = true;
    }
    else if( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::INITIAL_VALUE )
    {
      layoutDataElement.initialValue = value;
    }
    else if( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::TARGET_VALUE )
    {
      layoutDataElement.targetValue = value;
    }
    else if( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::ANIMATOR )
    {
      if( value.GetType() == Property::STRING )
      {
        layoutDataElement.animatorName = value.Get<std::string>();
        layoutDataElement.UpdateAnimatorIndex( mLayoutAnimators );
      }
      else if ( value.GetType() == Property::MAP )
      {
        Property::Map animatorMap = value.Get< Property::Map >();
        LayoutDataAnimator layoutDataAnimator;
        if( ConvertToLayoutAnimator( animatorMap, propertyAnimator, layoutDataAnimator ) )
        {
          mLayoutAnimators.push_back( layoutDataAnimator );
          layoutDataElement.animatorIndex = mLayoutAnimators.size() - 1;
        }
      }
    }
  }

  return propertyFound;
}

void LayoutTransitionData::CollectChildrenLayoutDataElements( Actor child, LayoutData& layoutData )
{
  LayoutDataArray& layoutDataArray = layoutData.layoutDataArray;
  // Add the children animators
  for( const LayoutDataElement& iter : layoutData.childrenLayoutDataArray )
  {
    Actor actor = iter.handle.GetHandle();
    if( actor && actor != child )
    {
      continue;
    }

    LayoutDataElement layoutDataElement = iter;
    Actor gainedChild = layoutData.layoutTransition.gainedChild.GetHandle();
    Actor lostChild = layoutData.layoutTransition.lostChild.GetHandle();
    switch ( layoutDataElement.condition )
    {
      case Dali::Toolkit::LayoutTransitionData::Condition::ON_ADD:
        if ( layoutData.layoutTransition.layoutTransitionType != Dali::Toolkit::LayoutTransitionData::ON_CHILD_ADD
            || gainedChild != child )
        {
          continue;
        }
        break;
      case Dali::Toolkit::LayoutTransitionData::Condition::ON_REMOVE:
        if( layoutData.layoutTransition.layoutTransitionType != Dali::Toolkit::LayoutTransitionData::ON_CHILD_REMOVE
            || lostChild != child )
        {
          continue;
        }
        break;
      case Dali::Toolkit::LayoutTransitionData::Condition::ON_FOCUS_GAINED:
        if( layoutData.layoutTransition.layoutTransitionType != Dali::Toolkit::LayoutTransitionData::ON_CHILD_FOCUS
            || gainedChild != child )
        {
          continue;
        }
        break;
      case Dali::Toolkit::LayoutTransitionData::Condition::ON_FOCUS_LOST:
        if( layoutData.layoutTransition.layoutTransitionType != Dali::Toolkit::LayoutTransitionData::ON_CHILD_FOCUS
            || lostChild != child )
        {
          continue;
        }
        break;
      default:
        break;
    }

    if( layoutData.updateMeasuredSize && !layoutDataElement.updateMeasuredSize )
    {
      continue;
    }

    layoutDataElement.handle = child;
    layoutDataElement.UpdatePropertyIndex();
    layoutDataElement.UpdatePositionDataIndex( layoutData );
    layoutDataArray.push_back( layoutDataElement );
  }
}

void LayoutTransitionData::UpdateAnimatorsIndices()
{
  for( LayoutDataElement& iter: mLayoutDataElements )
  {
    iter.UpdateAnimatorIndex( mLayoutAnimators );
  }
}

void LayoutTransitionData::CollectLayoutDataElements( Actor owner, LayoutData& layoutData )
{
  LayoutAnimatorArray& layoutAnimatorArray = layoutData.layoutAnimatorArray;
  LayoutAnimatorArray::iterator it = mLayoutAnimators.begin();
  if (layoutAnimatorArray.size() != 0)
  {
    // skip default animator
    ++it;
  }
  std::copy( it, mLayoutAnimators.end(), std::back_inserter( layoutAnimatorArray ) );

  LayoutDataArray& layoutDataArray = layoutData.layoutDataArray;
  // Collect the transition animators
  for( const LayoutDataElement& iter : mLayoutDataElements )
  {
    Actor actor = iter.handle.GetHandle();
    if( !actor || actor != owner )
    {
      layoutData.childrenLayoutDataArray.push_back( iter );
      continue;
    }

    LayoutDataElement layoutDataElement = iter;
    if( layoutData.updateMeasuredSize && !layoutDataElement.updateMeasuredSize )
    {
      continue;
    }

    layoutDataElement.UpdatePropertyIndex();
    layoutDataElement.UpdatePositionDataIndex( layoutData );
    layoutDataArray.push_back( layoutDataElement );
  }
}

Dali::Toolkit::LayoutTransitionData::LayoutTransitionSignalType& LayoutTransitionData::FinishedSignal()
{
  return mFinishedSignal;
}

void LayoutTransitionData::EmitSignalFinish( int layoutTransitionType )
{
  if ( !mFinishedSignal.Empty() )
  {
    Dali::Toolkit::LayoutTransitionData handle( this );
    mFinishedSignal.Emit( static_cast<Dali::Toolkit::LayoutTransitionData::Type>(layoutTransitionType), handle );
  }
}

bool LayoutTransitionData::HasUpdateMeasuredSize()
{
  return mUpdateMeasuredSize;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
