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
// Key tokens
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

LayoutTransitionData::LayoutTransitionData()
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
  : handle( Actor( ) )
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
  mPropertyAnimators.push_back( PropertyAnimator( actor, map ) );
}

void LayoutTransitionData::AddPropertyAnimator( Actor actor, Property::Map map, KeyFrames keyFrames, Animation::Interpolation interpolation )
{
  mPropertyAnimators.push_back( PropertyAnimator( actor, map, keyFrames, interpolation ) );
}

void LayoutTransitionData::AddPropertyAnimator( Actor actor, Property::Map map, Path path, Vector3 forward )
{
  mPropertyAnimators.push_back( PropertyAnimator( actor, map, path, forward ) );
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
        if( animatorType == "ANIMATE_TO" )
        {
          layoutDataAnimator.animatorType = LayoutDataAnimator::AnimatorType::ANIMATE_TO;
        }
        else if( animatorType == "ANIMATE_BY" )
        {
          layoutDataAnimator.animatorType = LayoutDataAnimator::AnimatorType::ANIMATE_BY;
        }
        else if( animatorType == "ANIMATE_BETWEEN" )
        {
          layoutDataAnimator.animatorType = LayoutDataAnimator::AnimatorType::ANIMATE_BETWEEN;
          layoutDataAnimator.keyFrames = propertyAnimator.keyFrames;
        }
        else if( animatorType == "ANIMATE_PATH" )
        {
          layoutDataAnimator.animatorType = LayoutDataAnimator::AnimatorType::ANIMATE_PATH;
          layoutDataAnimator.path = propertyAnimator.path;
          layoutDataAnimator.forward = propertyAnimator.forward;
        }
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

bool LayoutTransitionData::ConvertToLayoutDataElement( const PropertyAnimator& propertyAnimator, LayoutDataElement& layoutDataElement, LayoutData& layoutData )
{
  const Property::Map& map = propertyAnimator.map;
  bool propertyFound = false;

  for( unsigned int mapIdx = 0; mapIdx < map.Count(); ++mapIdx )
  {
    const KeyValuePair pair( map.GetKeyValue( mapIdx ) );
    const Property::Value& value( pair.second );
    Property::Index indexKey = Property::INVALID_INDEX;

    if ( pair.first.type == Property::Key::STRING )
    {
      const std::string& key( pair.first.stringKey );
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
    }
    else
    {
      indexKey = pair.first.indexKey;
    }

    if( indexKey == Dali::Toolkit::LayoutTransitionData::AnimatorKey::PROPERTY )
    {
      if( value.GetType() == Property::STRING )
      {
        Actor actor = Actor::DownCast( layoutDataElement.handle );
        layoutDataElement.propertyIndex = DevelHandle::GetPropertyIndex( actor, Property::Key( value.Get<std::string>() ) );
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
        std::string animatorName = value.Get<std::string>();
        if ( animatorName.empty() )
        {
          layoutDataElement.animatorIndex = 0;
        }
        else
        {
          auto animator = std::find_if( layoutData.layoutAnimatorArray.begin(), layoutData.layoutAnimatorArray.end(), [&animatorName](const LayoutDataAnimator& iter) {
            return (iter.name == animatorName); } );
          if( animator != layoutData.layoutAnimatorArray.end() )
          {
            layoutDataElement.animatorIndex = std::distance( layoutData.layoutAnimatorArray.begin(), animator );
          }
        }
      }
      else if ( value.GetType() == Property::MAP )
      {
        Property::Map animatorMap = value.Get< Property::Map >();
        LayoutDataAnimator layoutDataAnimator;
        if( ConvertToLayoutAnimator( animatorMap, propertyAnimator, layoutDataAnimator ) )
        {
          layoutData.layoutAnimatorArray.push_back( layoutDataAnimator );
          layoutDataElement.animatorIndex = layoutData.layoutAnimatorArray.size()-1;
        }
      }
    }
  }

  return propertyFound;
}

void LayoutTransitionData::ConvertToLayoutDataElements( Actor owner, LayoutData& layoutData )
{
  LayoutDataArray& layoutDataArray = layoutData.layoutDataArray;
  // Add the children animators
  for( const PropertyAnimator& iter : layoutData.childrenPropertyAnimators )
  {
    LayoutDataElement layoutDataElement;
    layoutDataElement.handle = owner;
    layoutDataElement.positionDataIndex = layoutData.layoutPositionDataArray.size() - 1;

    if( ConvertToLayoutDataElement( iter, layoutDataElement, layoutData ) )
    {
      layoutDataArray.push_back( layoutDataElement );
    }
  }

  // Add the transition animators
  for( const PropertyAnimator& iter : mPropertyAnimators )
  {
    if( iter.handle == nullptr )
    {
      layoutData.childrenPropertyAnimators.push_back( iter );
      continue;
    }

    LayoutDataElement layoutDataElement;
    layoutDataElement.handle = iter.handle;
    if( ConvertToLayoutDataElement( iter, layoutDataElement, layoutData ) )
    {
      layoutDataArray.push_back( layoutDataElement );
    }
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
    mFinishedSignal.Emit( static_cast<Dali::Toolkit::LayoutTransitionData::LayoutTransitionType>(layoutTransitionType), handle );
  }
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
