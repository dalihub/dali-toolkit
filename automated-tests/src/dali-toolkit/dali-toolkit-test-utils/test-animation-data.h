#ifndef DALI_TOOLKIT_TEST_TEST_ANIMATION_DATA_H
#define DALI_TOOLKIT_TEST_TEST_ANIMATION_DATA_H

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
#include <string>

namespace Test
{

class TestAnimationData
{
public:
  TestAnimationData();
  ~TestAnimationData();

  /**
   * @brief AnimationDataElement Describes one part of an animation.
   */
  struct AnimationDataElement
  {
    std::string target;
    std::string property;
    Dali::Property::Value value;
    Dali::AlphaFunction::BuiltinFunction alphaFunction;
    float timePeriodDelay;
    float timePeriodDuration;

    AnimationDataElement();
  };

  /**
   * @brief AnimationData holds the required data required to define an
   * animation to be performed on a property source.
   */
  typedef Dali::Vector< AnimationDataElement* > AnimationDataList;

  /**
   * @brief Adds one AnimationDataElement to the list to describe one animation.
   * @param[in] animationDataElement A pre-populated struct to add
   */
  void Add( AnimationDataElement* animationDataElement );

  std::size_t Size() const;

  void Clear();

  AnimationDataList mAnimationDataList;
};

void NewAnimator( const Dali::Property::Map& map, TestAnimationData::AnimationDataElement& element );
void NewAnimation( const Dali::Property::Map& map, TestAnimationData& outputAnimationData );
void NewAnimation( const Dali::Property::Array& array, TestAnimationData& outputAnimationData );
} // Test

#endif  //DALI_TOOLKIT_TEST_TEST_ANIMATION_DATA_H
