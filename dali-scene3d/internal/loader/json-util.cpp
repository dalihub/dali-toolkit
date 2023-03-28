/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/internal/loader/json-util.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/object/property-value.h>
#include <array>

namespace Dali
{
using namespace Toolkit;

namespace Scene3D
{
namespace Loader
{
namespace
{
template<typename T>
Property::Value ReadPrimitiveHelper(const TreeNode* tn, bool (*reader)(const TreeNode*, T&))
{
  T value;
  if(reader(tn, value))
  {
    return Property::Value(value);
  }
  return Property::Value();
}

template<typename T>
Property::Value ReadVectorHelper(const TreeNode* tn)
{
  static_assert(sizeof(T) % sizeof(float) == 0, "");
  T value;
  if(ReadVector(tn, value.AsFloat(), sizeof(T) / sizeof(float)))
  {
    return Property::Value(value);
  }
  return Property::Value();
}

Property::Value ReadVectorSingleFloatHelper(const TreeNode* tn)
{
  float value;
  if(ReadVector(tn, &value, 1u))
  {
    return Property::Value(value);
  }
  return Property::Value();
}

Property::Value ReadRotationHelper(const TreeNode* tn)
{
  switch(tn->Size())
  {
    case 3:
    {
      // degrees as per spec
      Vector3 rotation;
      ReadVector(tn, rotation.AsFloat(), 3u);
      return Property::Value(Quaternion(Radian(Degree(rotation.x)),
                                        Radian(Degree(rotation.y)),
                                        Radian(Degree(rotation.z))));
    }
    case 4:
    {
      Vector4 v;
      ReadVector(tn, v.AsFloat(), 4u);
      //Quaternion
      return Property::Value(Quaternion(v));
    }
    default:
      return Property::Value();
  }
}

template<typename T>
bool ReadQuadHelper(const TreeNode* tn, const std::array<T*, 4>& quad)
{
  auto i     = quad.begin();
  auto iEnd  = quad.end();
  auto iJson = tn->CBegin();
  while(iJson != tn->CEnd() && i != iEnd)
  {
    int value;
    if(ReadInt(&(*iJson).second, value) && value <= std::numeric_limits<T>::max())
    {
      **i = value;
      ++i;
      ++iJson;
    }
    else
    {
      return false;
    }
  }
  return true;
}

const std::map<std::string_view, Property::Value (*)(const TreeNode*)>& GetTypeIds()
{
  static const std::map<std::string_view, Property::Value (*)(const TreeNode*)> kTypeIds{
    // NONE
    {"boolean", [](const TreeNode* tn) {
       return ReadPrimitiveHelper<bool>(tn, ReadBool);
     }},
    {"float", [](const TreeNode* tn) {
       return ReadPrimitiveHelper<float>(tn, ReadFloat);
     }},
    {"integer", [](const TreeNode* tn) {
       return ReadPrimitiveHelper<int>(tn, ReadInt);
     }},
    {"vector2", ReadVectorHelper<Vector2>},
    {"vector3", ReadVectorHelper<Vector3>},
    {"vector4", ReadVectorHelper<Vector4>},
    {"matrix3", ReadVectorHelper<Matrix3>},
    {"matrix", ReadVectorHelper<Matrix>},
    {"rectangle", [](const TreeNode* tn) {
       Rect<int> value;
       if(ReadQuadHelper<int>(tn, {&value.x, &value.y, &value.width, &value.height}))
       {
         return Property::Value(value);
       }
       return Property::Value();
     }},
    {"rotation", ReadRotationHelper},
    // STRING - not particularly animatable
    // ARRAY - not particularly animatable
    // MAP - not particularly animatable
    {"extents", [](const TreeNode* tn) {
       Extents value;
       if(ReadQuadHelper<uint16_t>(tn, {&value.start, &value.end, &value.top, &value.bottom}))
       {
         return Property::Value(value);
       }
       return Property::Value();
     }},
  };
  return kTypeIds;
}

Property::Value (*const kArrayPropertyProcessors[])(const TreeNode*){
  ReadVectorHelper<Matrix>,
  ReadVectorHelper<Matrix3>,
  ReadVectorHelper<Vector4>,
  ReadVectorHelper<Vector3>,
  ReadVectorHelper<Vector2>,
  ReadVectorSingleFloatHelper};

} // namespace

bool ReadBool(const TreeNode* node, bool& num)
{
  if(!node)
  {
    return false;
  }

  bool returnValue = false;
  if(node->GetType() == TreeNode::BOOLEAN)
  {
    num         = node->GetBoolean();
    returnValue = true;
  }

  return returnValue;
}

bool ReadInt(const TreeNode* node, int& num)
{
  if(!node)
  {
    return false;
  }

  bool returnValue = false;
  if(node->GetType() == TreeNode::INTEGER)
  {
    num         = node->GetInteger();
    returnValue = true;
  }
  else if(node->GetType() == TreeNode::FLOAT)
  {
    num         = static_cast<int>(node->GetFloat());
    returnValue = true;
  }

  return returnValue;
}

bool ReadFloat(const TreeNode* node, float& num)
{
  if(!node)
  {
    return false;
  }

  bool returnValue = false;
  if(node->GetType() == TreeNode::FLOAT)
  {
    num         = node->GetFloat();
    returnValue = true;
  }
  else if(node->GetType() == TreeNode::INTEGER)
  {
    num         = static_cast<float>(node->GetInteger());
    returnValue = true;
  }

  return returnValue;
}

bool ReadIndex(const Toolkit::TreeNode* node, Index& num)
{
  bool returnValue = node && node->GetType() == TreeNode::INTEGER;
  if(returnValue)
  {
    num = static_cast<Index>(node->GetInteger());
  }

  return returnValue;
}

bool ReadBlob(const Toolkit::TreeNode* node, unsigned int& offset, unsigned int& length)
{
  if(!node)
  {
    return false;
  }

  int  iOffset, iLength;
  bool success = ReadInt(node->GetChild("byteOffset"), iOffset) &&
                 ReadInt(node->GetChild("byteLength"), iLength) &&
                 iOffset >= 0 && iLength >= 0; // 0 length might not be sensible, but is not an error at this stage.
  if(success)
  {
    offset = static_cast<unsigned int>(iOffset);
    length = static_cast<unsigned int>(iLength);
  }
  return success;
}

size_t GetNumericalArraySize(const TreeNode* node)
{
  size_t size = 0;
  if(node->GetType() == TreeNode::ARRAY)
  {
    for(auto i0 = node->CBegin(), i1 = node->CEnd(); i0 != i1 &&
                                                     ((*i0).second.GetType() == TreeNode::FLOAT || (*i0).second.GetType() == TreeNode::INTEGER);
        ++i0)
    {
      ++size;
    }
  }
  return size;
}

bool ReadVector(const TreeNode* node, float* num, unsigned int size)
{
  if(!node)
  {
    return false;
  }

  bool returnValue = false;
  if((node->Size() >= size) && (node->GetType() == TreeNode::ARRAY))
  {
    unsigned int offset = 0u;
    for(TreeNode::ConstIterator it = node->CBegin(); offset < size; ++it, ++offset)
    {
      const TreeNode& coord = (*it).second;
      if(!ReadFloat(&coord, *(num + offset)))
      {
        return false;
      }
    }
    returnValue = true;
  }

  return returnValue;
}

bool ReadVector(const Toolkit::TreeNode* node, int* num, unsigned int size)
{
  if(!node)
  {
    return false;
  }

  bool returnValue = false;
  if((node->Size() >= size) && (node->GetType() == TreeNode::ARRAY))
  {
    unsigned int offset = 0u;
    for(TreeNode::ConstIterator it = node->CBegin(); offset < size; ++it, ++offset)
    {
      const TreeNode& coord = (*it).second;
      if(!ReadInt(&coord, *(num + offset)))
      {
        return false;
      }
    }
    returnValue = true;
  }

  return returnValue;
}

bool ReadColor(const TreeNode* node, Vector4& color)
{
  if(nullptr == node)
  {
    return false;
  }

  if(!ReadVector(node, color.AsFloat(), 4))
  {
    if(!ReadVector(node, color.AsFloat(), 3))
    {
      return false;
    }
    color.a = 1.f;
  }

  return true;
}

bool ReadTimePeriod(const TreeNode* node, TimePeriod& timePeriod)
{
  if(!node)
  {
    return false;
  }

  if(!ReadFloat(node->GetChild("delay"), timePeriod.delaySeconds) || !ReadFloat(node->GetChild("duration"), timePeriod.durationSeconds))
  {
    return false;
  }
  return true;
}

bool ReadString(const TreeNode* node, std::string& strValue)
{
  if(!node)
  {
    return false;
  }

  bool returnValue = false;
  if(node->GetType() == TreeNode::STRING)
  {
    strValue    = node->GetString();
    returnValue = true;
  }
  return returnValue;
}

bool ReadStringVector(const TreeNode* node, std::vector<std::string>& strvector)
{
  if(!node)
  {
    return false;
  }

  bool returnValue = false;
  if(node->GetType() == TreeNode::ARRAY)
  {
    for(TreeNode::ConstIterator it = node->CBegin(); it != node->CEnd(); ++it)
    {
      const TreeNode& strNode = (*it).second;
      if(strNode.GetType() == TreeNode::STRING)
      {
        strvector.push_back(strNode.GetString());
      }
      else
      {
        return false;
      }
    }
    returnValue = true;
  }
  return returnValue;
}

Property::Value ReadPropertyValue(const Property::Type& propType, const TreeNode& tn)
{
  switch(propType)
  {
    case Property::BOOLEAN:
      return ReadPrimitiveHelper<bool>(&tn, ReadBool);

    case Property::FLOAT:
      return ReadPrimitiveHelper<float>(&tn, ReadFloat);

    case Property::INTEGER:
      return ReadPrimitiveHelper<int>(&tn, ReadInt);

    case Property::VECTOR2:
      return ReadVectorHelper<Vector2>(&tn);

    case Property::VECTOR3:
      return ReadVectorHelper<Vector3>(&tn);

    case Property::VECTOR4:
      return ReadVectorHelper<Vector4>(&tn);

    case Property::MATRIX3:
      return ReadVectorHelper<Matrix3>(&tn);

    case Property::MATRIX:
      return ReadVectorHelper<Matrix>(&tn);

    case Property::RECTANGLE:
    {
      Rect<int> value;
      if(ReadQuadHelper<int>(&tn, {&value.x, &value.y, &value.width, &value.height}))
      {
        return Property::Value(value);
      }
      break;
    }

    case Property::ROTATION:
      return ReadRotationHelper(&tn);

    case Property::EXTENTS:
    {
      Extents value;
      if(ReadQuadHelper<uint16_t>(&tn, {&value.start, &value.end, &value.top, &value.bottom}))
      {
        return Property::Value(value);
      }
      break;
    }

    case Property::NONE: // fall
    default:
    {
      DALI_ASSERT_ALWAYS(!"Property type incorrect");
    }
  }
  return Property::Value();
}

Property::Value ReadPropertyValue(const Toolkit::TreeNode& tn)
{
  Property::Value propValue;
  if(tn.GetType() == TreeNode::OBJECT) // attempt to disambiguate type.
  {
    auto jsonType = tn.GetChild("type");
    if(jsonType && jsonType->GetType() == TreeNode::STRING)
    {
      auto iFind = GetTypeIds().find(jsonType->GetString());
      if(iFind != GetTypeIds().end())
      {
        propValue = iFind->second(tn.GetChild("value"));
      }
    }
  }

  if(propValue.GetType() == Property::NONE)
  {
    if(tn.Size() == 0)
    {
      switch(tn.GetType())
      {
        case TreeNode::BOOLEAN:
          propValue = ReadPrimitiveHelper<bool>(&tn, ReadBool);
          break;

        case TreeNode::INTEGER:
          propValue = ReadPrimitiveHelper<int>(&tn, ReadInt);
          break;

        case TreeNode::FLOAT:
          propValue = ReadPrimitiveHelper<float>(&tn, ReadFloat);
          break;

        default:
          break;
      }
    }
    else
    {
      bool allNumbers = true;
      for(auto i0 = tn.CBegin(), i1 = tn.CEnd(); i0 != i1; ++i0)
      {
        auto type = (*i0).second.GetType();
        if(!(type == TreeNode::FLOAT || type == TreeNode::INTEGER))
        {
          allNumbers = false;
          break;
        }
      }

      if(allNumbers)
      {
        // NOTE: rotations / rectangles / extents must be disambiguated in all circumstances.
        for(auto& r : kArrayPropertyProcessors)
        {
          propValue = r(&tn);
          if(propValue.GetType() != Property::NONE)
          {
            break;
          }
        }
      }
    }
  }
  return propValue;
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali
