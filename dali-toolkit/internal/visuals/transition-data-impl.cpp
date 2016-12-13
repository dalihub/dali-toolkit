/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 *
 */

// CLASS HEADER
#include <dali-toolkit/internal/visuals/transition-data-impl.h>

// EXTERNAL HEADERS
#include <dali/dali.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/integration-api/debug.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <sstream>

using namespace Dali;

namespace
{
const char* TOKEN_TARGET("target");
const char* TOKEN_PROPERTY("property");
const char* TOKEN_INITIAL_VALUE("initialValue");
const char* TOKEN_TARGET_VALUE("targetValue");
const char* TOKEN_ANIMATOR("animator");
const char* TOKEN_TIME_PERIOD("timePeriod");
const char* TOKEN_DURATION("duration");
const char* TOKEN_DELAY("delay");
const char* TOKEN_ALPHA_FUNCTION("alphaFunction");


DALI_ENUM_TO_STRING_TABLE_BEGIN( ALPHA_FUNCTION_BUILTIN )
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, LINEAR)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, REVERSE)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, EASE_IN)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, EASE_OUT)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, EASE_IN_OUT)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, EASE_IN_SQUARE)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, EASE_OUT_SQUARE)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, EASE_IN_SINE)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, EASE_OUT_SINE)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, EASE_IN_OUT_SINE)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, EASE_OUT_BACK)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, BOUNCE)
DALI_ENUM_TO_STRING_WITH_SCOPE(AlphaFunction, SIN)
DALI_ENUM_TO_STRING_TABLE_END( ALPHA_FUNCTION_BUILTIN )
}

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

TransitionData::TransitionData()
{
}

TransitionData::~TransitionData()
{
}

TransitionDataPtr TransitionData::New( const Property::Array& value )
{
  TransitionDataPtr transitionData( new TransitionData() );
  transitionData->Initialize(value);
  return transitionData;
}

TransitionDataPtr TransitionData::New( const Property::Map& value )
{
  TransitionDataPtr transitionData( new TransitionData() );
  transitionData->Initialize(value);
  return transitionData;
}


void TransitionData::Initialize( const Property::Map& map )
{
  TransitionData::Animator* animator = ConvertMap( map );
  Add( animator );
}

void TransitionData::Initialize( const Property::Array& array )
{
  for( unsigned int arrayIdx = 0, transitionArrayCount = array.Count(); arrayIdx < transitionArrayCount; ++arrayIdx )
  {
    const Property::Value& element = array.GetElementAt( arrayIdx );
    // Expect each child to be an object representing an animator:

    Property::Map* map = element.GetMap();
    if( map != NULL )
    {
      TransitionData::Animator* animator = ConvertMap( *map );
      Add( animator );
    }
  }
}

TransitionData::Animator* TransitionData::ConvertMap( const Property::Map& map)
{
  TransitionData::Animator* animator = new TransitionData::Animator();
  animator->alphaFunction = AlphaFunction::LINEAR;
  animator->timePeriodDelay = 0.0f;
  animator->timePeriodDuration = 1.0f;

  for( unsigned int mapIdx = 0; mapIdx < map.Count(); ++mapIdx )
  {
    const KeyValuePair pair( map.GetKeyValue( mapIdx ) );
    if( pair.first.type == Property::Key::INDEX )
    {
      continue; // We don't consider index keys.
    }

    const std::string& key( pair.first.stringKey );
    const Property::Value& value( pair.second );

    if( key == TOKEN_TARGET )
    {
      animator->objectName = value.Get< std::string >();
    }
    else if( key == TOKEN_PROPERTY )
    {
      if( value.GetType() == Property::STRING )
      {
        animator->propertyKey = Property::Key( value.Get<std::string>() );
      }
      else
      {
        animator->propertyKey = Property::Key( value.Get<int>() );
      }
    }
    else if( key == TOKEN_INITIAL_VALUE )
    {
      animator->initialValue = value;
    }
    else if( key == TOKEN_TARGET_VALUE )
    {
      animator->targetValue = value;
    }
    else if( key == TOKEN_ANIMATOR )
    {
      animator->animate = true;
      Property::Map animatorMap = value.Get< Property::Map >();
      for( size_t animatorMapIdx = 0; animatorMapIdx < animatorMap.Count(); ++animatorMapIdx )
      {
        const KeyValuePair pair( animatorMap.GetKeyValue( animatorMapIdx ) );

        if( pair.first.type == Property::Key::INDEX )
        {
          continue; // We don't consider index keys.
        }

        const std::string& key( pair.first.stringKey );
        const Property::Value& value( pair.second );

        if( key == TOKEN_ALPHA_FUNCTION )
        {
          if( value.GetType() == Property::ARRAY )
          {
            bool valid = true;
            Vector4 controlPoints;
            Property::Array *array = value.GetArray();
            if( array && array->Count() >= 4 )
            {
              for( size_t vecIdx = 0; vecIdx < 4; ++vecIdx )
              {
                Property::Value& v = array->GetElementAt(vecIdx);
                if( v.GetType() == Property::FLOAT )
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

            if( valid )
            {
              Vector2 controlPoint1( controlPoints.x, controlPoints.y );
              Vector2 controlPoint2( controlPoints.z, controlPoints.w );
              animator->alphaFunction = AlphaFunction( controlPoint1, controlPoint2 );
            }
            else
            {
              animator->animate = false;
            }
          }
          else if( value.GetType() == Property::VECTOR4 )
          {
            Vector4 controlPoints = value.Get<Vector4>();
            Vector2 controlPoint1( controlPoints.x, controlPoints.y );
            Vector2 controlPoint2( controlPoints.z, controlPoints.w );
            animator->alphaFunction = AlphaFunction( controlPoint1, controlPoint2 );
          }
          else if( value.GetType() == Property::STRING )
          {
            std::string alphaFunctionValue = value.Get< std::string >();

            if( alphaFunctionValue == "LINEAR" )
            {
              animator->alphaFunction = AlphaFunction(AlphaFunction::LINEAR);
            }
            else if( ! alphaFunctionValue.compare(0, 5, "EASE_" ) )
            {
              if( alphaFunctionValue == "EASE_IN" )
              {
                animator->alphaFunction = AlphaFunction(AlphaFunction::EASE_IN);
              }
              else if( alphaFunctionValue == "EASE_OUT" )
              {
                animator->alphaFunction = AlphaFunction(AlphaFunction::EASE_OUT);
              }
              else if( ! alphaFunctionValue.compare( 5, 3, "IN_" ) )
              {
                if( ! alphaFunctionValue.compare(8, -1, "SQUARE" ))
                {
                  animator->alphaFunction = AlphaFunction(AlphaFunction::EASE_IN_SQUARE);
                }
                else if( ! alphaFunctionValue.compare(8, -1, "OUT" ))
                {
                  animator->alphaFunction = AlphaFunction(AlphaFunction::EASE_IN_OUT);
                }
                else if( ! alphaFunctionValue.compare(8, -1, "OUT_SINE" ))
                {
                  animator->alphaFunction = AlphaFunction(AlphaFunction::EASE_IN_OUT_SINE);
                }
                else if( ! alphaFunctionValue.compare(8, -1, "SINE" ))
                {
                  animator->alphaFunction = AlphaFunction(AlphaFunction::EASE_IN_SINE);
                }
              }
              else if( ! alphaFunctionValue.compare( 5, 4, "OUT_" ) )
              {
                if( ! alphaFunctionValue.compare(9, -1, "SQUARE" ) )
                {
                  animator->alphaFunction = AlphaFunction(AlphaFunction::EASE_OUT_SQUARE);
                }
                else if( ! alphaFunctionValue.compare(9, -1, "SINE" ) )
                {
                  animator->alphaFunction = AlphaFunction(AlphaFunction::EASE_OUT_SINE);
                }
                else if( ! alphaFunctionValue.compare(9, -1, "BACK" ) )
                {
                  animator->alphaFunction = AlphaFunction(AlphaFunction::EASE_OUT_BACK);
                }
              }
            }
            else if( alphaFunctionValue == "REVERSE" )
            {
              animator->alphaFunction = AlphaFunction(AlphaFunction::REVERSE);
            }
            else if( alphaFunctionValue == "BOUNCE" )
            {
              animator->alphaFunction = AlphaFunction(AlphaFunction::BOUNCE);
            }
            else if( alphaFunctionValue == "SIN" )
            {
              animator->alphaFunction = AlphaFunction(AlphaFunction::SIN);
            }
          }
          else
          {
            animator->animate = false;
          }
        }
        else if( key == TOKEN_TIME_PERIOD )
        {
          Property::Map timeMap = value.Get< Property::Map >();
          for( size_t timeMapIdx = 0; timeMapIdx < timeMap.Count(); ++timeMapIdx )
          {
            const KeyValuePair pair( timeMap.GetKeyValue( timeMapIdx ) );
            if( pair.first.type == Property::Key::INDEX )
            {
              continue;
            }
            const std::string& key( pair.first.stringKey );

            if( key == TOKEN_DELAY )
            {
              animator->timePeriodDelay = pair.second.Get< float >();
            }
            else if( key == TOKEN_DURATION )
            {
              animator->timePeriodDuration = pair.second.Get< float >();
            }
          }
        }
      }
    }
  }
  return animator;
}

void TransitionData::Add( Animator* animator )
{
  mAnimators.PushBack( animator );
}

TransitionData::Iterator TransitionData::Begin() const
{
  return mAnimators.Begin();
}

TransitionData::Iterator TransitionData::End() const
{
  return mAnimators.End();
}

size_t TransitionData::Count() const
{
  return mAnimators.Count();
}

Property::Map TransitionData::GetAnimatorAt( size_t index )
{
  DALI_ASSERT_ALWAYS( index < Count() && "index exceeds bounds" );

  Animator* animator = mAnimators[index];
  Property::Map map;
  map[TOKEN_TARGET] = animator->objectName;
  if( animator->propertyKey.type == Property::Key::INDEX )
  {
    map[TOKEN_PROPERTY] = animator->propertyKey.indexKey;
  }
  else
  {
    map[TOKEN_PROPERTY] = animator->propertyKey.stringKey;
  }
  if( animator->initialValue.GetType() != Property::NONE )
  {
    map[TOKEN_INITIAL_VALUE] = animator->initialValue;
  }
  if( animator->targetValue.GetType() != Property::NONE )
  {
    map[TOKEN_TARGET_VALUE] = animator->targetValue;
  }
  if( animator->animate )
  {
    Property::Map animateMap;

    if( animator->alphaFunction.GetMode() == AlphaFunction::BUILTIN_FUNCTION )
    {
      animateMap.Add(TOKEN_ALPHA_FUNCTION, GetEnumerationName( animator->alphaFunction.GetBuiltinFunction(),
                                                               ALPHA_FUNCTION_BUILTIN_TABLE,
                                                               ALPHA_FUNCTION_BUILTIN_TABLE_COUNT ));
    }
    else if( animator->alphaFunction.GetMode() == AlphaFunction::BEZIER )
    {
      Vector4 controlPoints = animator->alphaFunction.GetBezierControlPoints();
      animateMap.Add( TOKEN_ALPHA_FUNCTION, controlPoints );
    }
    animateMap.Add(TOKEN_TIME_PERIOD, Property::Map()
                   .Add( TOKEN_DELAY, animator->timePeriodDelay )
                   .Add( TOKEN_DURATION, animator->timePeriodDuration ));

    map[TOKEN_ANIMATOR] = animateMap;
  }

  return map;
}

} // namespace Internal
} // namespace Toolkit
} // namespace Dali
