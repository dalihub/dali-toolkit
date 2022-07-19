/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali-scene3d/public-api/loader/facial-animation-loader.h>

// EXTERNAL INCLUDES
#include <sstream>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/loader/json-reader.h>
#include <dali-scene3d/public-api/loader/blend-shape-details.h>
#include <dali-scene3d/public-api/loader/utils.h>

namespace js = json;

namespace Dali
{
namespace
{
const float MILLISECONDS_TO_SECONDS = 0.001f;

struct BlendShape
{
  std::vector<std::vector<float>> mKeys;
  std::string_view                mNodeName;
  uint32_t                        mNumberOfMorphTarget;
  std::string_view                mVersion;
  std::string_view                mFullName;
  std::vector<std::string_view>   mMorphNames;
};

struct FacialAnimation
{
  std::string_view        mName;
  std::vector<BlendShape> mBlendShapes;
  std::string_view        mVersion;
  uint32_t                mNumberOfShapes;
  std::vector<uint32_t>   mTime;
  uint32_t                mNumberOfFrames;
};

std::vector<std::vector<float>> ReadBlendShapeKeys(const json_value_s& j)
{
  auto&                           jo = js::Cast<json_array_s>(j);
  std::vector<std::vector<float>> result;

  result.reserve(jo.length);

  auto i = jo.start;
  while(i)
  {
    result.push_back(std::move(js::Read::Array<float, js::Read::Number>(*i->value)));
    i = i->next;
  }

  return result;
}

const auto BLEND_SHAPE_READER = std::move(js::Reader<BlendShape>()
                                            .Register(*js::MakeProperty("key", ReadBlendShapeKeys, &BlendShape::mKeys))
                                            .Register(*new js::Property<BlendShape, std::string_view>("name", js::Read::StringView, &BlendShape::mNodeName))
                                            .Register(*js::MakeProperty("morphtarget", js::Read::Number<uint32_t>, &BlendShape::mNumberOfMorphTarget))
                                            .Register(*new js::Property<BlendShape, std::string_view>("blendShapeVersion", js::Read::StringView, &BlendShape::mVersion))
                                            .Register(*new js::Property<BlendShape, std::string_view>("fullName", js::Read::StringView, &BlendShape::mFullName))
                                            .Register(*js::MakeProperty("morphname", js::Read::Array<std::string_view, js::Read::StringView>, &BlendShape::mMorphNames)));

const auto FACIAL_ANIMATION_READER = std::move(js::Reader<FacialAnimation>()
                                                 .Register(*new js::Property<FacialAnimation, std::string_view>("name", js::Read::StringView, &FacialAnimation::mName))
                                                 .Register(*js::MakeProperty("blendShapes", js::Read::Array<BlendShape, js::ObjectReader<BlendShape>::Read>, &FacialAnimation::mBlendShapes))
                                                 .Register(*new js::Property<FacialAnimation, std::string_view>("version", js::Read::StringView, &FacialAnimation::mVersion))
                                                 .Register(*js::MakeProperty("shapesAmount", js::Read::Number<uint32_t>, &FacialAnimation::mNumberOfShapes))
                                                 .Register(*js::MakeProperty("time", js::Read::Array<uint32_t, js::Read::Number>, &FacialAnimation::mTime))
                                                 .Register(*js::MakeProperty("frames", js::Read::Number<uint32_t>, &FacialAnimation::mNumberOfFrames)));

} // unnamed namespace

namespace Scene3D
{
namespace Loader
{
AnimationDefinition LoadFacialAnimation(const std::string& url)
{
  bool failed = false;
  auto js     = LoadTextFile(url.c_str(), &failed);
  if(failed)
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Failed to load " << url << ".";
  }

  json::unique_ptr root(json_parse(js.c_str(), js.size()));
  if(!root)
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Failed to parse " << url << ".";
  }

  static bool setObjectReaders = true;
  if(setObjectReaders)
  {
    // NOTE: only referencing own, anonymous namespace, const objects; the pointers will never need to change.
    js::SetObjectReader(BLEND_SHAPE_READER);
    setObjectReaders = false;
  }

  auto& rootObj = js::Cast<json_object_s>(*root);

  FacialAnimation facialAnimation;
  FACIAL_ANIMATION_READER.Read(rootObj, facialAnimation);

  AnimationDefinition animationDefinition;
  animationDefinition.mName     = std::string(facialAnimation.mName.data());
  animationDefinition.mDuration = MILLISECONDS_TO_SECONDS * static_cast<float>(facialAnimation.mTime[facialAnimation.mNumberOfFrames - 1u]);

  // Calculate the number of animated properties.
  uint32_t numberOfAnimatedProperties = 0u;
  for(const auto& blendShape : facialAnimation.mBlendShapes)
  {
    numberOfAnimatedProperties += blendShape.mNumberOfMorphTarget;
  }
  animationDefinition.mProperties.resize(numberOfAnimatedProperties);

  // Create the key frame instances.
  for(auto& animatedProperty : animationDefinition.mProperties)
  {
    animatedProperty.mKeyFrames = Dali::KeyFrames::New();
  }

  // Set the property names
  uint32_t targets = 0u;
  for(const auto& blendShape : facialAnimation.mBlendShapes)
  {
    for(uint32_t morphTargetIndex = 0u; morphTargetIndex < blendShape.mNumberOfMorphTarget; ++morphTargetIndex)
    {
      AnimatedProperty& animatedProperty = animationDefinition.mProperties[targets + morphTargetIndex];
      animatedProperty.mTimePeriod       = Dali::TimePeriod(animationDefinition.mDuration);

      animatedProperty.mNodeName = blendShape.mNodeName;

      std::stringstream weightPropertyStream;
      weightPropertyStream << BlendShapes::WEIGHTS_UNIFORM << "[" << morphTargetIndex << "]";
      animatedProperty.mPropertyName = weightPropertyStream.str();
    }
    targets += blendShape.mNumberOfMorphTarget;
  }

  targets = 0u;
  for(const auto& blendShape : facialAnimation.mBlendShapes)
  {
    for(uint32_t timeIndex = 0u; timeIndex < facialAnimation.mNumberOfFrames; ++timeIndex)
    {
      const float progress = MILLISECONDS_TO_SECONDS * static_cast<float>(facialAnimation.mTime[timeIndex]) / animationDefinition.mDuration;

      for(uint32_t morphTargetIndex = 0u; morphTargetIndex < blendShape.mNumberOfMorphTarget; ++morphTargetIndex)
      {
        AnimatedProperty& animatedProperty = animationDefinition.mProperties[targets + morphTargetIndex];

        animatedProperty.mKeyFrames.Add(progress, blendShape.mKeys[timeIndex][morphTargetIndex]);
      }
    }
    targets += blendShape.mNumberOfMorphTarget;
  }

  return animationDefinition;
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
