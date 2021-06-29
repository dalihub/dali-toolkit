/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/hidden-text.h>
#include <dali-toolkit/internal/text/input-filter.h>

// INTERNAL INCLUDES

using namespace Dali::Toolkit;

namespace Dali
{
namespace Toolkit
{
namespace Text
{
const char* const PROPERTY_ACCEPTED = "accepted";
const char* const PROPERTY_REJECTED = "rejected";

InputFilter::InputFilter()
: mAccepted(""),
  mRejected("")
{
}

void InputFilter::SetProperties(const Property::Map& map)
{
  const Property::Map::SizeType count = map.Count();

  for(Property::Map::SizeType position = 0; position < count; ++position)
  {
    KeyValuePair     keyValue = map.GetKeyValue(position);
    Property::Key&   key      = keyValue.first;
    Property::Value& value    = keyValue.second;

    if(key == Toolkit::InputFilter::Property::ACCEPTED || key == PROPERTY_ACCEPTED)
    {
      value.Get(mAccepted);
    }
    else if(key == Toolkit::InputFilter::Property::REJECTED || key == PROPERTY_REJECTED)
    {
      value.Get(mRejected);
    }
  }
}

void InputFilter::GetProperties(Property::Map& map)
{
  map[Toolkit::InputFilter::Property::ACCEPTED] = mAccepted.c_str();
  map[Toolkit::InputFilter::Property::REJECTED] = mRejected.c_str();
}

bool InputFilter::Contains(Toolkit::InputFilter::Property::Type type, std::string source)
{
  bool       match = false;
  std::regex pattern;

  if(type == Toolkit::InputFilter::Property::ACCEPTED)
  {
    if(mAccepted.empty())
    {
      return true;
    }
    pattern = mAccepted;
  }
  else if(type == Toolkit::InputFilter::Property::REJECTED)
  {
    if(mRejected.empty())
    {
      return false;
    }
    pattern = mRejected;
  }

  match = std::regex_match(source, pattern);

  return match;
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
