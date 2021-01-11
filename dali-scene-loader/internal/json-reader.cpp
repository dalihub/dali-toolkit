/*
* Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/internal/json-reader.h"
#include <algorithm>
#include <cstring>

namespace json
{

int StrCmp(const json_string_s& js, const char* s)
{
  auto sSize = strlen(s);
  auto shorter = std::min(js.string_size, sSize);
  auto base = strncmp(js.string, s, shorter);
  return ((base != 0) || (sSize == js.string_size)) ? base : ((js.string_size < sSize) ?
    -s[shorter] : js.string[shorter]);
}

int StrCmp(const json_string_s& js, const std::string& s)
{
  auto sSize = s.size();
  auto shorter = std::min(js.string_size, sSize);
  auto base = strncmp(js.string, s.c_str(), shorter);
  return ((base != 0) || (sSize == js.string_size)) ? base : ((js.string_size < sSize) ?
    -s[shorter] : js.string[shorter]);
}

void Validate(const json_value_s & jv, json_type_e type)
{
  if (jv.type != type)
  {
    throw std::runtime_error("Invalid type; expected: " + std::to_string(type) + ", got: " + std::to_string(jv.type));
  }
}

json_value_s* FindObjectChild(const std::string& key, json_object_s& obj)
{
  auto i = obj.start;
  while (i)
  {
    if (0 == StrCmp(*i->name, key))
    {
      return i->value;
    }
    i = i->next;
  }
  return nullptr;
}

}
