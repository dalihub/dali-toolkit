/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/animation/key-frames-devel.h>
#include <dali/integration-api/debug.h>
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
  std::vector<std::vector<float>> mKeys{{}};
  std::string_view                mNodeName{};
  uint32_t                        mNumberOfMorphTarget{0u};
  std::string_view                mVersion{};
  std::string_view                mFullName{};
  std::vector<std::string_view>   mMorphNames{};
};

struct FacialAnimation
{
  std::string_view        mName{};
  std::vector<BlendShape> mBlendShapes{{}};
  std::string_view        mVersion{};
  uint32_t                mNumberOfShapes{0u};
  std::vector<uint32_t>   mTime{};
  uint32_t                mNumberOfFrames{0u};
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

const js::Reader<BlendShape>& GetBlendShapeReader()
{
  static const auto BLEND_SHAPE_READER = std::move(js::Reader<BlendShape>()
                                                     .Register(*js::MakeProperty("key", ReadBlendShapeKeys, &BlendShape::mKeys))
                                                     .Register(*new js::Property<BlendShape, std::string_view>("name", js::Read::StringView, &BlendShape::mNodeName))
                                                     .Register(*js::MakeProperty("morphtarget", js::Read::Number<uint32_t>, &BlendShape::mNumberOfMorphTarget))
                                                     .Register(*new js::Property<BlendShape, std::string_view>("blendShapeVersion", js::Read::StringView, &BlendShape::mVersion))
                                                     .Register(*new js::Property<BlendShape, std::string_view>("fullName", js::Read::StringView, &BlendShape::mFullName))
                                                     .Register(*js::MakeProperty("morphname", js::Read::Array<std::string_view, js::Read::StringView>, &BlendShape::mMorphNames)));
  return BLEND_SHAPE_READER;
}

const js::Reader<FacialAnimation>& GetFacialAnimationReader()
{
  static const auto FACIAL_ANIMATION_READER = std::move(js::Reader<FacialAnimation>()
                                                          .Register(*new js::Property<FacialAnimation, std::string_view>("name", js::Read::StringView, &FacialAnimation::mName))
                                                          .Register(*js::MakeProperty("blendShapes", js::Read::Array<BlendShape, js::ObjectReader<BlendShape>::Read>, &FacialAnimation::mBlendShapes))
                                                          .Register(*new js::Property<FacialAnimation, std::string_view>("version", js::Read::StringView, &FacialAnimation::mVersion))
                                                          .Register(*js::MakeProperty("shapesAmount", js::Read::Number<uint32_t>, &FacialAnimation::mNumberOfShapes))
                                                          .Register(*js::MakeProperty("time", js::Read::Array<uint32_t, js::Read::Number>, &FacialAnimation::mTime))
                                                          .Register(*js::MakeProperty("frames", js::Read::Number<uint32_t>, &FacialAnimation::mNumberOfFrames)));
  return FACIAL_ANIMATION_READER;
}

Dali::Scene3D::Loader::AnimationDefinition LoadFacialAnimationInternal(json::unique_ptr& root)
{
  static bool setObjectReaders = true;
  if(setObjectReaders)
  {
    // NOTE: only referencing own, anonymous namespace, const objects; the pointers will never need to change.
    js::SetObjectReader(GetBlendShapeReader());
    setObjectReaders = false;
  }

  auto& rootObj = js::Cast<json_object_s>(*root);

  FacialAnimation facialAnimation;
  GetFacialAnimationReader().Read(rootObj, facialAnimation);

  Dali::Scene3D::Loader::AnimationDefinition animationDefinition;

  // Check validation of the facial animation data.
  if(facialAnimation.mNumberOfFrames == 0u)
  {
    DALI_LOG_ERROR("The number of frames is zero! return empty animation.\n");
    return animationDefinition;
  }
  if(facialAnimation.mTime.size() != static_cast<size_t>(facialAnimation.mNumberOfFrames))
  {
    DALI_LOG_ERROR("The number of frames does not match the number of time values.\n");
    return animationDefinition;
  }

  // Calculate the number of animated properties.
  uint32_t numberOfAnimatedProperties = 0u;
  for(const auto& blendShape : facialAnimation.mBlendShapes)
  {
    numberOfAnimatedProperties += blendShape.mNumberOfMorphTarget;

    if(blendShape.mKeys.size() != static_cast<uint32_t>(facialAnimation.mNumberOfFrames))
    {
      DALI_LOG_ERROR("The number of frames does not match the number of node[%s]'s keys.\n", std::string(blendShape.mNodeName).c_str());
      return animationDefinition;
    }

    for(const auto& keyframes : blendShape.mKeys)
    {
      if(keyframes.size() != static_cast<size_t>(blendShape.mNumberOfMorphTarget))
      {
        DALI_LOG_ERROR("The number of keys does not match the number of node[%s]'s values.\n", std::string(blendShape.mNodeName).c_str());
        return animationDefinition;
      }
    }
  }

  animationDefinition.SetName(facialAnimation.mName.data());
  animationDefinition.SetDuration(MILLISECONDS_TO_SECONDS * static_cast<float>(facialAnimation.mTime[facialAnimation.mNumberOfFrames - 1u]));
  animationDefinition.ReserveSize(numberOfAnimatedProperties);

  uint32_t targets = 0u;
  for(const auto& blendShape : facialAnimation.mBlendShapes)
  {
    for(uint32_t morphTargetIndex = 0u; morphTargetIndex < blendShape.mNumberOfMorphTarget; ++morphTargetIndex)
    {
      Dali::Scene3D::Loader::AnimatedProperty animatedProperty;
      animatedProperty.mTimePeriod = Dali::TimePeriod(animationDefinition.GetDuration());
      animatedProperty.mNodeName   = blendShape.mNodeName;
      std::stringstream weightPropertyStream;
      weightPropertyStream << Dali::Scene3D::Loader::BlendShapes::WEIGHTS_UNIFORM << "[" << morphTargetIndex << "]";
      animatedProperty.mPropertyName = weightPropertyStream.str();

      animatedProperty.mKeyFrames = Dali::KeyFrames::New();
      for(uint32_t timeIndex = 0u; timeIndex < facialAnimation.mNumberOfFrames; ++timeIndex)
      {
        const float progress = Dali::EqualsZero(animationDefinition.GetDuration()) ? 0.0f : MILLISECONDS_TO_SECONDS * static_cast<float>(facialAnimation.mTime[timeIndex]) / animationDefinition.GetDuration();
        animatedProperty.mKeyFrames.Add(progress, blendShape.mKeys[timeIndex][morphTargetIndex]);
      }
      // Optimize keyframes, for heuristic!
      DevelKeyFrames::OptimizeKeyFramesLinear(animatedProperty.mKeyFrames);
      animationDefinition.SetProperty(targets + morphTargetIndex, std::move(animatedProperty));
    }

    targets += blendShape.mNumberOfMorphTarget;
  }

  return animationDefinition;
}
} // unnamed namespace

namespace Scene3D
{
namespace Loader
{
AnimationDefinition LoadFacialAnimation(const std::string& url)
{
  bool failed   = false;
  auto jsonData = LoadTextFile(url.c_str(), &failed);
  if(failed)
  {
    DALI_LOG_ERROR("Failed to load file. url : %s\n", url.c_str());
    AnimationDefinition animationDefinition;
    return animationDefinition;
  }

  json::unique_ptr root(json_parse(jsonData.c_str(), jsonData.size()));
  if(!root)
  {
    DALI_LOG_ERROR("Failed to parse json. url : %s\n", url.c_str());
    AnimationDefinition animationDefinition;
    return animationDefinition;
  }

  return LoadFacialAnimationInternal(root);
}

AnimationDefinition LoadFacialAnimationFromBuffer(const uint8_t* rawBuffer, int rawBufferLength)
{
  json::unique_ptr root(json_parse(rawBuffer, static_cast<size_t>(static_cast<uint32_t>(rawBufferLength))));
  if(!root)
  {
    DALI_LOG_ERROR("Failed to parse json from buffer.\n");
    AnimationDefinition animationDefinition;
    return animationDefinition;
  }

  return LoadFacialAnimationInternal(root);
}
} // namespace Loader
} // namespace Scene3D
} // namespace Dali
