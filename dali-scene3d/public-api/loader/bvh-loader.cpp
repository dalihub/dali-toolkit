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
#include <dali-scene3d/public-api/loader/bvh-loader.h>

// EXTERNAL INCLUDES
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string_view>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
namespace
{
static constexpr std::string_view TOKEN_OFFSET              = "OFFSET";
static constexpr std::string_view TOKEN_CHANNELS            = "CHANNELS";
static constexpr std::string_view TOKEN_XPOSITION           = "Xposition";
static constexpr std::string_view TOKEN_YPOSITION           = "Yposition";
static constexpr std::string_view TOKEN_ZPOSITION           = "Zposition";
static constexpr std::string_view TOKEN_XROTATION           = "Xrotation";
static constexpr std::string_view TOKEN_YROTATION           = "Yrotation";
static constexpr std::string_view TOKEN_ZROTATION           = "Zrotation";
static constexpr std::string_view TOKEN_JOINT               = "JOINT";
static constexpr std::string_view TOKEN_END_SITE            = "End Site";
static constexpr std::string_view TOKEN_FRAMES              = "Frames";
static constexpr std::string_view TOKEN_FRAME_TIME          = "Frame Time";
static constexpr std::string_view TOKEN_HIERARCHY           = "HIERARCHY";
static constexpr std::string_view TOKEN_ROOT                = "ROOT";
static constexpr std::string_view TOKEN_MOTION              = "MOTION";
static constexpr std::string_view PROPERTY_NAME_POSITION    = "position";
static constexpr std::string_view PROPERTY_NAME_ORIENTATION = "orientation";
static constexpr std::string_view TOKEN_CLOSING_BRACE       = "}";

enum class Channel
{
  XPOSITION = 0,
  YPOSITION,
  ZPOSITION,
  XROTATION,
  YROTATION,
  ZROTATION
};

struct Frame
{
  std::vector<float> values;
};

struct Joint
{
  std::string                         name;
  Vector3                             offset;
  std::vector<Vector3>                translations;
  std::vector<Quaternion>             rotations;
  std::vector<Channel>                channels;
  std::vector<std::shared_ptr<Joint>> children;
};

void trim(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
          }));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
          }).base(),
          s.end());
}

void ParseHierarchy(std::ifstream& file, std::shared_ptr<Joint>& joint)
{
  std::string line;
  while(std::getline(file, line))
  {
    trim(line);
    std::istringstream stream(line);
    std::string        token;
    std::getline(stream, token, ' ');

    if(token == TOKEN_OFFSET.data())
    {
      stream >> joint->offset.x >> joint->offset.y >> joint->offset.z;
    }
    else if(token == TOKEN_CHANNELS.data())
    {
      uint32_t channelCount = 0;
      std::getline(stream, token, ' ');
      channelCount = static_cast<uint32_t>(std::atoi(token.c_str()));
      for(uint32_t i = 0; i < channelCount; ++i)
      {
        std::getline(stream, token, ' ');
        trim(token);
        if(token == TOKEN_XPOSITION.data())
        {
          joint->channels.push_back(Channel::XPOSITION);
        }
        else if(token == TOKEN_YPOSITION.data())
        {
          joint->channels.push_back(Channel::YPOSITION);
        }
        else if(token == TOKEN_ZPOSITION.data())
        {
          joint->channels.push_back(Channel::ZPOSITION);
        }
        else if(token == TOKEN_XROTATION.data())
        {
          joint->channels.push_back(Channel::XROTATION);
        }
        else if(token == TOKEN_YROTATION.data())
        {
          joint->channels.push_back(Channel::YROTATION);
        }
        else if(token == TOKEN_ZROTATION.data())
        {
          joint->channels.push_back(Channel::ZROTATION);
        }
      }
    }
    else if(token == TOKEN_JOINT.data())
    {
      std::shared_ptr<Joint> child(new Joint);
      joint->children.push_back(child);
      std::getline(stream, token, ' ');
      child->name = token;
      ParseHierarchy(file, child);
    }
    else if(line == TOKEN_END_SITE.data())
    {
      while(std::getline(file, line))
      {
        trim(line);
        if(line == TOKEN_CLOSING_BRACE.data())
        {
          break;
        }
      }
    }
    else if(token == TOKEN_CLOSING_BRACE.data())
    {
      break;
    }
  }
}

void MakeList(std::shared_ptr<Joint>& joint, std::vector<std::shared_ptr<Joint>>& jointList)
{
  jointList.push_back(joint);
  for(uint32_t i = 0; i < joint->children.size(); ++i)
  {
    MakeList(joint->children[i], jointList);
  }
}

void ParseMotion(std::ifstream& file, std::shared_ptr<Joint>& hierarchy, uint32_t& frameCount, float& frameTime)
{
  std::vector<std::shared_ptr<Joint>> jointList;
  MakeList(hierarchy, jointList);

  bool        frameCountLoaded = false;
  bool        frameTimeLoaded  = false;
  std::string line;
  while((!frameCountLoaded || !frameTimeLoaded) && std::getline(file, line))
  {
    trim(line);
    std::istringstream stream(line);
    std::string        token;
    std::getline(stream, token, ':');
    trim(token);
    if(token == TOKEN_FRAMES.data())
    {
      stream >> frameCount;
      frameCountLoaded = true;
    }
    else if(token == TOKEN_FRAME_TIME.data())
    {
      stream >> frameTime;
      frameTimeLoaded = true;
    }
  }

  while(getline(file, line))
  {
    trim(line);
    std::istringstream stream(line);
    for(auto&& joint : jointList)
    {
      Vector3    translation;
      Quaternion rotation[3];
      for(uint32_t i = 0; i < joint->channels.size(); ++i)
      {
        if(joint->channels[i] == Channel::XPOSITION)
        {
          stream >> translation.x;
        }
        else if(joint->channels[i] == Channel::YPOSITION)
        {
          stream >> translation.y;
        }
        else if(joint->channels[i] == Channel::ZPOSITION)
        {
          stream >> translation.z;
        }
        else if(joint->channels[i] == Channel::XROTATION)
        {
          float radian;
          stream >> radian;
          rotation[0] = Quaternion(Radian(Degree(radian)), Vector3::XAXIS);
        }
        else if(joint->channels[i] == Channel::YROTATION)
        {
          float radian;
          stream >> radian;
          rotation[1] = Quaternion(Radian(Degree(radian)), Vector3::YAXIS);
        }
        else if(joint->channels[i] == Channel::ZROTATION)
        {
          float radian;
          stream >> radian;
          rotation[2] = Quaternion(Radian(Degree(radian)), Vector3::ZAXIS);
        }
      }
      joint->translations.push_back(translation);
      joint->rotations.push_back(rotation[2] * rotation[0] * rotation[1]);
    }
  }
}

bool ParseBvh(const std::string& path, uint32_t& frameCount, float& frameTime, std::shared_ptr<Joint>& rootJoint)
{
  std::ifstream file(path.data());
  if(!file.is_open())
  {
    return false;
  }

  std::string line;
  while(getline(file, line))
  {
    trim(line);
    std::istringstream stream(line);
    std::string        token;
    std::getline(stream, token, ' ');
    if(token == TOKEN_HIERARCHY.data())
    {
      std::string line;
      while(getline(file, line))
      {
        trim(line);
        std::istringstream stream(line);
        std::string        token;
        std::getline(stream, token, ' ');
        if(token == TOKEN_ROOT.data())
        {
          std::getline(stream, token, ' ');
          rootJoint->name = token;
          ParseHierarchy(file, rootJoint);
          break;
        }
      }
    }
    if(token == TOKEN_MOTION.data())
    {
      ParseMotion(file, rootJoint, frameCount, frameTime);
    }
  }
  return true;
}

AnimationDefinition GenerateAnimation(const std::string& animationName, std::shared_ptr<Joint>& hierarchy, uint32_t frameCount, float frameTime, const Vector3& scale)
{
  AnimationDefinition animationDefinition;

  animationDefinition.mName     = animationName;
  animationDefinition.mDuration = frameTime * (frameCount - 1);
  float keyFrameInterval        = (frameCount > 1u) ? 1.0f / static_cast<float>(frameCount - 1u) : Dali::Math::MACHINE_EPSILON_10;

  std::vector<std::shared_ptr<Joint>> jointList;
  MakeList(hierarchy, jointList);

  if(!jointList.empty())
  {
    animationDefinition.mProperties.resize(jointList.size() * 2u); // translation and rotation
    for(uint32_t i = 0; i < jointList.size(); ++i)
    {
      AnimatedProperty& translationProperty = animationDefinition.mProperties[i * 2u];
      translationProperty.mTimePeriod       = Dali::TimePeriod(animationDefinition.mDuration);
      translationProperty.mNodeName         = jointList[i]->name;
      translationProperty.mPropertyName     = PROPERTY_NAME_POSITION.data();

      AnimatedProperty& rotationProperty = animationDefinition.mProperties[i * 2u + 1];
      rotationProperty.mTimePeriod       = Dali::TimePeriod(animationDefinition.mDuration);
      rotationProperty.mNodeName         = jointList[i]->name;
      rotationProperty.mPropertyName     = PROPERTY_NAME_ORIENTATION.data();

      translationProperty.mKeyFrames = Dali::KeyFrames::New();
      rotationProperty.mKeyFrames    = Dali::KeyFrames::New();
      for(uint32_t j = 0; j < frameCount; ++j)
      {
        translationProperty.mKeyFrames.Add(static_cast<float>(j) * keyFrameInterval, (jointList[i]->translations[j] * scale));
        rotationProperty.mKeyFrames.Add(static_cast<float>(j) * keyFrameInterval, jointList[i]->rotations[j]);
      }
    }
  }

  return animationDefinition;
}
} // namespace

AnimationDefinition LoadBvh(const std::string& path, const std::string& animationName, const Vector3& scale)
{
  uint32_t               frameCount = 0;
  float                  frameTime  = 0.0f;
  std::shared_ptr<Joint> rootJoint(new Joint);
  if(!ParseBvh(path, frameCount, frameTime, rootJoint))
  {
    AnimationDefinition animationDefinition;
    return animationDefinition;
  }

  return GenerateAnimation(animationName, rootJoint, frameCount, frameTime, scale);
}
} // namespace Loader
} // namespace Scene3D
} // namespace Dali