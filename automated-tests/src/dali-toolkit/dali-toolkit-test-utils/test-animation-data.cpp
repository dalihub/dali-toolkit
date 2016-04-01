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
 */

#include <dali/dali.h>
#include <test-animation-data.h>

using namespace Dali;

namespace Test
{

TestAnimationData::TestAnimationData()
{
}

TestAnimationData::~TestAnimationData()
{
}

TestAnimationData::AnimationDataElement::AnimationDataElement()
: alphaFunction( AlphaFunction::DEFAULT ),
  timePeriodDelay( 0.0f ),
  timePeriodDuration( 1.0f )
{
}

void TestAnimationData::Add( AnimationDataElement* animationDataElement )
{
  mAnimationDataList.PushBack( animationDataElement );
}

std::size_t TestAnimationData::Size() const
{
  return mAnimationDataList.Size();
}

void TestAnimationData::Clear()
{
  AnimationDataList::Iterator end = mAnimationDataList.End();
  for( AnimationDataList::Iterator iter = mAnimationDataList.Begin(); iter != end; ++iter )
  {
    delete ( *iter );
  }
  mAnimationDataList.Clear();
}


void NewAnimator( const Property::Map& map, TestAnimationData::AnimationDataElement& element )
{
  // Now set the properties, or create children
  for( unsigned int i = 0, animationMapCount = map.Count(); i < animationMapCount; ++i )
  {
    const StringValuePair& pair( map.GetPair( i ) );
    const std::string& key( pair.first );
    const Property::Value& value( pair.second );

    if( key == "actor" || key == "target" )
    {
      element.target = value.Get< std::string >();
    }
    else if( key == "property" )
    {
      element.property = value.Get< std::string >();
    }
    else if( key == "value" )
    {
      element.value = value;
    }
    else if( key == "alphaFunction" )
    {
      std::string alphaFunctionValue = value.Get< std::string >();

      if( alphaFunctionValue == "LINEAR" )
      {
        element.alphaFunction = AlphaFunction::LINEAR;
      }
      else if( alphaFunctionValue == "REVERSE" )
      {
        element.alphaFunction = AlphaFunction::REVERSE;
      }
      else if( alphaFunctionValue == "EASE_IN_SQUARE" )
      {
        element.alphaFunction = AlphaFunction::EASE_IN_SQUARE;
      }
      else if( alphaFunctionValue == "EASE_OUT_SQUARE" )
      {
        element.alphaFunction = AlphaFunction::EASE_OUT_SQUARE;
      }
      else if( alphaFunctionValue == "EASE_IN" )
      {
        element.alphaFunction = AlphaFunction::EASE_IN;
      }
      else if( alphaFunctionValue == "EASE_OUT" )
      {
        element.alphaFunction = AlphaFunction::EASE_OUT;
      }
      else if( alphaFunctionValue == "EASE_IN_OUT" )
      {
        element.alphaFunction = AlphaFunction::EASE_IN_OUT;
      }
      else if( alphaFunctionValue == "EASE_IN_SINE" )
      {
        element.alphaFunction = AlphaFunction::EASE_IN_SINE;
      }
      else if( alphaFunctionValue == "EASE_OUT_SINE" )
      {
        element.alphaFunction = AlphaFunction::EASE_OUT_SINE;
      }
      else if( alphaFunctionValue == "EASE_IN_OUT_SINE" )
      {
        element.alphaFunction = AlphaFunction::EASE_IN_OUT_SINE;
      }
      else if( alphaFunctionValue == "BOUNCE" )
      {
        element.alphaFunction = AlphaFunction::BOUNCE;
      }
      else if( alphaFunctionValue == "SIN" )
      {
        element.alphaFunction = AlphaFunction::SIN;
      }
      else if( alphaFunctionValue == "EASE_OUT_BACK" )
      {
        element.alphaFunction = AlphaFunction::EASE_OUT_BACK;
      }
    }
    else if( key == "timePeriod" )
    {
      Property::Map timeMap = value.Get< Property::Map >();
      for( unsigned int i = 0; i < timeMap.Count(); ++i )
      {
        const StringValuePair& pair( timeMap.GetPair( i ) );
        if( pair.first == "delay" )
        {
          element.timePeriodDelay = pair.second.Get< float >();
        }
        else if( pair.first == "duration" )
        {
          element.timePeriodDuration = pair.second.Get< float >();
        }
      }
    }
    else if( key == "animator" )
    {
      if( value.GetType() == Property::MAP )
      {
        Property::Map* map = value.GetMap();
        const Property::Map& mapref = *map;
        NewAnimator( mapref, element ); // Merge the map into element
      }
    }
  }
}

void NewAnimation( const Property::Map& map, TestAnimationData& outputAnimationData )
{
  TestAnimationData::AnimationDataElement* element = new TestAnimationData::AnimationDataElement();
  NewAnimator( map, *element );

  outputAnimationData.Add( element );
}

void NewAnimation( const Property::Array& array, TestAnimationData& outputAnimationData )
{
  for(unsigned int i=0; i<array.Size(); ++i )
  {
    TestAnimationData::AnimationDataElement* element = new TestAnimationData::AnimationDataElement();
    const Property::Value& value = array.GetElementAt(i);
    if( value.GetType() == Property::MAP )
    {
      Property::Map* map = value.GetMap();
      NewAnimator( *map, *element );
      outputAnimationData.Add( element );
    }
  }
}

} // Test
